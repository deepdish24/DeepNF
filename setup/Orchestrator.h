/*
* @author deepans
* Header file for Orchestrator of DeepNF
*/

#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <string>
#include "json.hpp"
#include <vector>
#include <unordered_map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <tuple>

using json = nlohmann::json;

enum Action {
    READ = 1,
    WRITE = 2,
    ADD = 3,
    RM = 4,
    DROP = 5
};

enum Field {
    SIP,
    DIP,
    SPORT,
    DPORT,
    PAYLOAD
};

struct ActionPair {
    Action action;
    Field field;
};

struct DependencyPair {
    std::string function1;
    std::string function2;
};

struct Intermediary {
    std::string function1Name;
    std::string function2Name;
    bool isParallelizable;
    std::vector<ActionPair> actions = {};
};

class Orchestrator {
private:
    json userInput;
    json actionTable;
    //std::vector<std::string> functions = {};
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<Field>>> pair_to_conflicts = {};
    //std::unordered_map<DependencyPair, std::vector<Field>> pair_to_conflicts = {};
    std::vector<std::tuple<std::string, std::string>> parsedOrder = {};
    std::vector<std::tuple<std::string, std::string>> parsedPriorities = {};
    std::vector<DependencyPair> userDependencies = {};
    std::unordered_map<std::string, int> sockets = {};
    std::unordered_map<std::string, std::string> func_to_ip = {};
    std::unordered_map<std::string, std::string> func_to_rules = {};

    //void setup_sockaddr(struct sockaddr_in &servaddr, std::string ip, int port);
    //void parse_file(const std::string file);
    bool isParallelizable(std::vector<std::string> orderDep, json actionTable, std::vector<Field> &conflictingActions);
    void parsePriorityDependencies(std::vector<std::vector<std::string>> priorities);
    void parseOrderDependencies(std::vector<std::vector<std::string>> dependencies);
    std::string fieldToString(Field a);
public:
    Orchestrator(std::string filepath, std::string action_file_path);
    void setup_containers();
    void start_packet_stream();
};

#endif