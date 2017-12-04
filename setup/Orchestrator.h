/*
* @author deepans
* Header file for Orchestrator of DeepNF
*/

#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <sys/socket.h>
#include <arpa/inet.h>

struct DependencyPair {
    std::string function1;
    std::string function2;
};

class Orchestrator {
private:
    std::vector<DependencyPair> userDependencies = {};
    std::unordered_map<std::string, int> sockets = {};
    std::unordered_map<std::string, std::string> func_to_ip = {};
    std::unordered_map<std::string, std::string> func_to_rules = {};
    //void setup_sockaddr(struct sockaddr_in &servaddr, std::string ip, int port);
    //void parse_file(const std::string file);
public:
    Orchestrator(std::string filepath);
    void setup_containers();
    void start_packet_stream();
};

#endif