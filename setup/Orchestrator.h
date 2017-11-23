/*
* @author deepans
* Header file for Orchestrator of DeepNF
*/

#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <string>
#include <vector>
#include <unordered_map>

struct DependencyPair {
    std::string function1;
    std::string function2;
};

class Orchestrator {
private:
    std::vector<DependencyPair> userDependencies = {};
    std::unordered_map<std::string, std::string> func_to_rules = {};
    void parse_file(const std::string file);
    void setup_containers();
    void setup_classifier();
public:
    void handle_msg()
};

#endif