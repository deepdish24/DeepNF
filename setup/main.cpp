/*
* @author deepans
* main program for orchestrator
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Orchestrator.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cerr << "No input json file given\n";
        exit(1);
    }
    std::string filepath(argv[1]);
    Orchestrator orca(filepath);
    orca.setup_containers();

    return 0;
}