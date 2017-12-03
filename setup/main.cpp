/*
* @author deepans
* main program for orchestrator
*/

#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

int main(int argc, char *argv[]) {
    if (argc == 0) {
        std::cerr << "No input json file given\n";
        exit(1);
    }
    json j;
    std::string filepath(argv[1]);
    std::ifstream fileInput(filepath);
    j << fileInput;
    
    std::cout << j["pi"] << "\n";
    std::cout << "Deepan Saravanan (deepans)";
    return 0;
}