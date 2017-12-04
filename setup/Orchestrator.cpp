/**
* @author deepans
* Orchestrator implementation
**/

#include "Orchestrator.h"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

using json = nlohmann::json;

Orchestrator::Orchestrator(std::string filepath) {
    json j;
    std::ifstream fileInput(filepath);
    j << fileInput;

    std::vector<std::string> ips = j["ips"];
    std::vector<int> ports = j["ports"];
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

    //TODO: partition functions to ip's in an efficient manner (support for more than 1 ip)
    std::vector<std::vector<std::string>> dependencies = j["dependencies"];
    std::string ip_one = ips[0];
    for (int i = 0; i < (int) dependencies.size(); i++) {
        struct DependencyPair dp;
        dp.function1 = dependencies[i][0];
        dp.function2 = dependencies[i][1];
        func_to_ip[dp.function1] = ip_one;
        func_to_ip[dp.function2] = ip_one;
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