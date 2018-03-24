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
#include <typeinfo>
#include "../"

Orchestrator::Orchestrator(std::string filepath, std::string action_file_path) {
    std::ifstream fileInput(filepath);
    std::ifstream actionTableInput(action_file_path);

    fileInput >> userInput;
    actionTableInput >> actionTable;

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

    // Check size of the positional to make sure it is positive and at most 2
    if (positionals.size() < 1 || positionals.size() > 2) {
        throw std::invalid_argument("Incorrect number of positional NFs detected");
    }

    if (positionals.size() == 2 && positionals[0][1].compare(positionals[1][1]) == 0) {
        throw std::invalid_argument("Two NFS assigned to same position");
    }

    parseOrderDependencies(dependencies);
    parsePriorityDependencies(priorities);

    for (int i = 0; i < (int) functions.size(); i++) {
        //creating on stack
        //ServiceGraphNode n(functions[i]);
        ServiceGraphNode *s = new ServiceGraphNode(functions[i]);
        //std::cout << "Function " << functions[i] << " node created with address: " << s << std::endl;
        func_to_nodes[functions[i]] = s;
    }

    std::set<ServiceGraphNode*> orderTreeNodes = {};
    std::set<ServiceGraphNode*> priorityNodes = {};

    // Create Forest describing order dependencies
    for (int i = 0; i < (int) parsedOrder.size(); i++) {
        std::string nf1 = std::get<0>(parsedOrder[i]);
        std::string nf2 = std::get<1>(parsedOrder[i]);
        ServiceGraphNode *node1 = func_to_nodes[nf1];
        ServiceGraphNode *node2 = func_to_nodes[nf2];
        (*node1).add_neighbor(node2);
        (*node2).add_parent(node1);
        orderTreeNodes.insert(node1);
        orderTreeNodes.insert(node2);
    }

    for (int i = 0; i < (int) parsedPriorities.size(); i++) {
        std::string nf1 = std::get<0>(parsedPriorities[i]);
        std::string nf2 = std::get<1>(parsedPriorities[i]);
        ServiceGraphNode *node1 = func_to_nodes[nf1];
        ServiceGraphNode *node2 = func_to_nodes[nf2];
        priorityNodes.insert(node1);
        priorityNodes.insert(node2);
    }

    std::set<ServiceGraphNode*> rootNodes = {};
    std::set<ServiceGraphNode*> leafNodes = {};

    // loop finds all root nodes of each tree
    for (auto node : orderTreeNodes) {
        if (node->isRoot()) {
            rootNodes.insert(node);
        }
    }

    // loop proceeds through each tree and checks if leaves can be run in parallel
    std::cout << "===================parsing tree nodes=======================" << std::endl;
    for (auto node : rootNodes) {
        std::set<ServiceGraphNode*> leavesForRoot = {};
        std::cout << "root node: " << node->nf << std::endl;
        std::cout << "num neighbors for root: " << node->neighbors.size() << std::endl;
        std::set<ServiceGraphNode*> neighbors = node->neighbors;
        for (auto neighbor : neighbors) {
            std::cout << "neighbor: " << neighbor->nf << std::endl;
            std::cout << "is neighbor leaf: " << neighbor->isLeaf() << std::endl;
        }
        findAllLeaves(node, leavesForRoot);
        checkLevelParallelizability(leavesForRoot);
        for (auto leaf : leavesForRoot) {
            leafNodes.insert(leaf);
        }
    }
    std::cout << "===================done parsing tree nodes=======================" << std::endl;

    //check all pairwise parallelizability of priorityNF's
    checkLevelParallelizability(priorityNodes);

    ServiceGraphNode* first = NULL;
    ServiceGraphNode* last = NULL;

    for (int i = 0; i < (int) positionals.size(); i++) {
        std::vector<std::string> pos = positionals[i];
        if (pos[1].compare("first") == 0) {
            first = func_to_nodes[pos[0]];
        }
        if (pos[1].compare("last") == 0) {
            last = func_to_nodes[pos[0]];
        }
    }

    // attaching root nodes to first node in chain
    for (auto node : rootNodes) {
        first->add_neighbor(node);
        node->add_parent(first);
    }

    // attaching all free floating nodes to 
    // both first and last nodes
    for (auto node : priorityNodes) {
        first->add_neighbor(node);
        node->add_parent(first);
        node->add_neighbor(last);
        last->add_parent(node);
    }

    // attaching all leaf nodes to last node
    for (auto node : leafNodes) {
        node->add_neighbor(last);
        last->add_parent(node);
    }



    // =======SERVICE GRAPH CONSTRUCTION DONE=========

    //partitioning...uniformly partition to machines



    //Testing Code for graph of order dependencies;
    //std::cout << "priority set size: " << priorityNodes.size() << std::endl;
    ServiceGraphNode *node1 = func_to_nodes["openvpn"];
    std::cout << (*node1).neighbors.size() << std::endl;
    //std::cout << "Current Node: " << (*node1).nf << std::endl;
    //std::cout << ((func_to_nodes["iptables"])->neighbors).size() << std::endl;

    for (auto it : (*node1).neighbors) {
        std::cout << (*it).nf;
    }




    std::string ip_one = ips[0];
    for (int i = 0; i < (int) dependencies.size(); i++) {
        struct DependencyPair dp;
        dp.function1 = dependencies[i][0];
        dp.function2 = dependencies[i][1];
        func_to_ip[dp.function1] = ip_one;
        func_to_ip[dp.function2] = ip_one;
    }
}

/* Function to parse priority dependencies and produce conflicting actions */
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

/* Function that parses order dependencies to determine which can be run in parallel.
   Adds those that can be parallelized as priority dependencies. Conflicting actions
   noted. */
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

/* function checks if nf1 before nf2 can be parallelized */
bool Orchestrator::isParallelizable(std::vector<std::string> orderDep, json actionTable,
    std::vector<Field> &conflictingActions) {
    //function gets actions for each function
    std::map<std::string, std::string> nf1 = actionTable[orderDep[0]];
    std::map<std::string, std::string> nf2 = actionTable[orderDep[1]];

    //function iterates through all packet locations for nf1
    for (auto it = nf1.begin(); it != nf1.end(); ++it) {
        std::string packetLocation = it->first;
        std::string val = it->second;
        std::string val2 = nf2[packetLocation];
        // if nf1 drops/adds/removes then it cannot be run in parallel with nf2
        if ((packetLocation.compare("Drop") == 0 || packetLocation.compare("Add/Rm") == 0) && val.compare("T") == 0) {
            return false;
        } else if (val.compare("null") != 0 && val2.compare("null") != 0) {
            Action a1 = stringToAction(it->second);
            Action a2 = stringToAction(nf2[packetLocation]);
            if ((a1 == READ || a1 == WRITE) && a2 == WRITE) {
                std::string loc(packetLocation);
                conflictingActions.push_back(stringToField(packetLocation));
            } else if (a1 == WRITE) {
                return false;
            }
        }
    }
    return true;
}


/* Function takes in root of tree and performs a depth-first search to find all leaves in tree.
   Leaves are added to set passed in */
void Orchestrator::findAllLeaves(ServiceGraphNode* root, std::set<ServiceGraphNode*> &leaves) {
    if (root->isLeaf()) {
        leaves.insert(root);
    } else {
        std::set<ServiceGraphNode*> neighbors = root->neighbors;
        for (auto neighbor : neighbors) {
            findAllLeaves(neighbor, leaves);
        }
    }
}

/* Function takes in set of nodes and checks to see if all are mutually parallelizable */
void Orchestrator::checkLevelParallelizability(std::set<ServiceGraphNode*> nodes) {
    std::unordered_map<std::string, std::unordered_map<std::string, bool>> visited_pair = {};
    for (auto node1 : nodes) {
        for (auto node2 : nodes) {
            if (node1 != node2) {
                std::string nf1 = node1->nf;
                std::string nf2 = node2->nf;
                //std::cout << "node1: " << nf1 << std::endl;
                //std::cout << "node2: " << nf1 << std::endl;
                //std::cout << "===============" << std::endl;
                std::vector<std::string> pair = {};
                pair.push_back(nf1);
                pair.push_back(nf2);
                std::vector<Field> conflictingActions = {};
                bool parallelizable = isParallelizable(pair, actionTable, conflictingActions);
                visited_pair[nf1][nf2] = parallelizable;
                if (!parallelizable) {
                    if (visited_pair.find(nf2) != visited_pair.end() && !visited_pair[nf2][nf1]) {
                        perror("nodes specified are not parallelizable");
                    }
                }
            }
        }
    }
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

// Given a service graph node, determine if it is a root node
bool Orchestrator::isLeaf(ServiceGraphNode *n) {
    return !(*n).neighbors.empty();
}

// Given a service graph node, determine the root node pointing to it
/*ServiceGraphNode* Orchestrator::findRootNode(ServiceGraphNode* n) {
    ServiceGraphNode *parentOfN = (*n).parent;
    if (parentOfN == NULL){
        return n;
    }
    else {
        ServiceGraphNode *currNode;
        while (parentOfN != NULL){
            currNode = parentOfN;
            parentOfN = (*currNode).parent;
        }
        return currNode;
    }
}*/