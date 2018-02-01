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

    private:
        int id;
        vector<int> nodes; // a list of nodes ids that should be run on this machine

 };