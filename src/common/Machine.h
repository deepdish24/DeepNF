#pragma once

#pragma once

#include <string>
#include <vector>

using namespace std;

class Machine
{
    public:
        Machine();
        ~Machine();

        string get_id();
        string get_ip();
        string get_bridge_ip();

    private:
        string id;
        string ip; // ip address
        string bridge_ip; // OVS bridge IP address
        vector<int> nodes; // a list of nodes ids that should be run on this machine
 };