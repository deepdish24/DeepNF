/*
* @author deepans
* Header file for Orchestrator of DeepNF
*/

#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <string>
#include "nlohmann/json.hpp"
#include <vector>
#include <unordered_map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <tuple>
#include "ServiceGraphNode.cpp"
#include "../common/NF.h"

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

struct ConflictPairInfo {
    int major;
    int minor;
    int parent;
    std::vector<std::string> confActions = {};
};

class Orchestrator {
private:
    json userInput;
    json actionTable;
    std::unordered_map<std::string, ServiceGraphNode*> func_to_nodes = {};
    std::unordered_map<std::string, std::string> ip_to_mc = {};
    std::vector<std::string> ips;
    std::vector<int> ports;

    // map if pair of functions (a, b) -> conflicts between a and b
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<Field>>> pair_to_conflicts = {};
    std::vector<std::tuple<std::string, std::string>> parsedOrder = {};
    std::vector<std::tuple<std::string, std::string>> parsedPriorities = {};

    std::unordered_map<std::string, int> sockets = {};
    std::unordered_map<std::string, std::string> func_to_ip = {};
    std::unordered_map<std::string, std::string> func_to_rules = {};

    //void setup_sockaddr(struct sockaddr_in &servaddr, std::string ip, int port);
    //void parse_file(const std::string file);
    bool isParallelizable(std::vector<std::string> orderDep, json actionTable, std::vector<Field> &conflictingActions);
    void parsePriorityDependencies(std::vector<std::vector<std::string>> priorities);
    void parseOrderDependencies(std::vector<std::vector<std::string>> dependencies);
    void checkLevelParallelizability(std::set<ServiceGraphNode*> nodes);
    void findAllLeaves(ServiceGraphNode* root, std::set<ServiceGraphNode*> &leaves);
    void write_json_dictionary(std::unordered_map<std::string, int> func_to_inx);
    void round_robin_partitioning(std::vector<std::string> &ips, std::vector<std::string> &functions);
    void single_node_partitioning(std::vector<std::string> &ips, std::vector<std::string> &functions);
    std::string fieldToString(Field a);
    NF stringToNF(std::string function);

    // Check if the given node is a leaf node
    bool isLeaf(ServiceGraphNode* n);

    // Returns the root node assoicated with n
    ServiceGraphNode* findRootNode(ServiceGraphNode* n);

    
public:
    Orchestrator(std::string filepath, std::string action_file_path);
    void setup_containers();
};

#endif
