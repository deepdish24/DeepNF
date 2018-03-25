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
    if (argc == 2) {
        std::cerr << "No input json file given:\n Need to specify user_input.json and action_table.json\n";
        exit(1);
    }
    std::string filepath(argv[1]);
    std::string action_file_path(argv[2]);
    Orchestrator orca(filepath, action_file_path);
    orca.setup_containers();

    return 0;
}