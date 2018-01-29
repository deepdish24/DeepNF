/**
* @author deepans
* Orchestrator implementation
**/

#include "Orchestrator.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ios>
#include <tuple>

Orchestrator::Orchestrator(std::string filepath, std::string action_file_path) {
    std::ifstream fileInput(filepath);
    std::ifstream actionTableInput(action_file_path);

    fileInput >> userInput;
    actionTableInput >> actionTable;
    //userInput << fileInput;
    //actionTable << actionTableInput;

    std::vector<std::string> functions = userInput["functions"];
    std::vector<std::string> ips = userInput["ips"];
    std::vector<int> ports = userInput["ports"];

    /* setting up sockaddr data structures to connect 
     to ip + port of all available machines */
    for (int i = 0; i < (int) ips.size(); i++) {
        std::string ip = ips[i];
        int port_num = ports[i];
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in servaddr;
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port_num);
        inet_pton(AF_INET, ip.c_str(), &(servaddr.sin_addr));
        connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
        sockets[ip] = sockfd;
    }

    std::vector<std::vector<std::string>> positionals = userInput["positional"];
    std::vector<std::vector<std::string>> dependencies = userInput["orderDependencies"];
    std::vector<std::vector<std::string>> priorities = userInput["priorities"];
    parseOrderDependencies(dependencies);
    parsePriorityDependencies(priorities);
    

    //std::vector<std::tuple<std::string, std::string>> parsedOrder = {};
    //std::vector<std::tuple<std::string, std::string>> parsedPriorities = {};

    //Need to check for pairwise conflicts between priority functions


    std::string ip_one = ips[0];
    for (int i = 0; i < (int) dependencies.size(); i++) {
        struct DependencyPair dp;
        dp.function1 = dependencies[i][0];
        dp.function2 = dependencies[i][1];
        func_to_ip[dp.function1] = ip_one;
        func_to_ip[dp.function2] = ip_one;
    }
}

/*std::vector<Intermediary> parsePolicies(std::vector<std::vector<std::string>> dependencies, 
    std::vector<std::vector<std::string>> priorities) {
    return {};
}*/

/* function checks if nf1 before nf2 can be parallelized */

void Orchestrator::parsePriorityDependencies(std::vector<std::vector<std::string>> priorities) {
    for (int i = 0; i < (int) priorities.size(); i++) {
        std::vector<Field> conflictingActions = {};
        bool parallel = isParallelizable(priorities[i], actionTable, conflictingActions);
        std::string f1 = priorities[i][0];
        std::string f2 = priorities[i][1];
        if (parallel) {
            parsedPriorities.push_back(std::make_tuple(f1, f2));
            if ((int) conflictingActions.size() > 0) {
                pair_to_conflicts[f1][f2] = conflictingActions;
            }
        } else {
            perror("priority constraint not parallelizable");
        }
    }
}

void Orchestrator::parseOrderDependencies(std::vector<std::vector<std::string>> dependencies) {
    for (int i = 0; i < (int) dependencies.size(); i++) {
        std::vector<Field> conflictingActions = {};
        bool parallel = isParallelizable(dependencies[i], actionTable, conflictingActions);
        std::string f1 = dependencies[i][0];
        std::string f2 = dependencies[i][1];
        if (parallel) {
            parsedPriorities.push_back(std::make_tuple(f1, f2));
            if ((int) conflictingActions.size() > 0) {
                pair_to_conflicts[f1][f2] = conflictingActions;
            }
        } else {
            parsedOrder.push_back(std::make_tuple(f1, f2));
        }
    }
}

Action stringToAction(std::string action) {
    Action a;
    if (action.compare("R") == 0) {
        a = READ;
    } else {
        a = WRITE;
    }
    return a;
}

std::string Orchestrator::fieldToString(Field a) {
    switch(a) {
        case SIP: 
            return std::string("SIP");
        case DIP:
            return std::string("DIP");
        case SPORT:
            return std::string("SPORT");
        case DPORT:
            return std::string("DPORT");
        default:
            return std::string("PAYLOAD");
    }
}

Field stringToField(std::string field) {
    Field f;
    if (field.compare("SIP") == 0) {
        f = SIP;
    } else if (field.compare("DIP") == 0) {
        f = DIP;
    } else if (field.compare("SPORT") == 0) {
        f = SPORT;
    } else if (field.compare("DPORT") == 0) {
        f = DPORT;
    } else {
        f = PAYLOAD;
    }
    return f;
}

bool Orchestrator::isParallelizable(std::vector<std::string> orderDep, json actionTable,
    std::vector<Field> &conflictingActions) {
    std::map<std::string, std::string> nf1 = actionTable[orderDep[0]];
    std::map<std::string, std::string> nf2 = actionTable[orderDep[1]];
    for (auto it = nf1.begin(); it != nf1.end(); ++it) {
        std::string packetLocation = it->first;
        std::string val = it->second;
        std::string val2 = nf2[packetLocation];
        if ((packetLocation.compare("Drop") == 0 || packetLocation.compare("Add/Rm") == 0) && val.compare("T") == 0) {
            return false;
        } else if (val.compare("null") != 0 && val2.compare("null") != 0) {
            Action a1 = stringToAction(it->second);
            Action a2 = stringToAction(nf2[packetLocation]);
            if ((a1 == READ || a1 == WRITE) && a2 == WRITE) {
                std::string loc(packetLocation);
                //std::cout << "loc: " << packetLocation << "\n";
                conflictingActions.push_back(stringToField(packetLocation));
            } else if (a1 == WRITE) {
                return false;
            }
        }
        //std::cout << "~~~~~~~~~~~~" << "\n";
    }
    return true;
}

void Orchestrator::setup_containers() {
    for (auto it = func_to_ip.begin(); it != func_to_ip.end(); ++it) {
        std::string function = it->first;
        std::string ip_addr = it->second;
        int sockfd = sockets[ip_addr];
        std::string x = function + "\r\n";
        const char* buf = x.c_str();
        char buffer[100];
        while (true) {
            write(sockfd, buf, strlen(buf));
            read(sockfd, buffer, sizeof(buffer) - 1);
            break;
        }
    }
}