/*
* @author deepans
* main program for orchestrator
*/

#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

int main(int argc, char *argv[]) {
    json j;
    j["pi"] = 3.14;
    std::cout << j["pi"] << "\n";
    std::cout << "Deepan Saravanan (deepans)";
    return 0;
}