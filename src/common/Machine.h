#pragma once

#include <string>
#include <vector>

class Machine
{
    public:
        Machine(int id);
        ~Machine();

        int get_id();
        std::string get_ip();
        std::string get_bridge_ip();
        std::vector<int> get_node_ids();
        void set_bridge_ip(std::string bridge_ip);
        void add_node_id(int n);
        void set_ip(std::string ip);

    private:
        int id;
        std::string ip; // ip address
        std::string bridge_ip; // OVS bridge IP address
        std::vector<int> nodes; // a list of nodes ids that should be run on this machine
 };