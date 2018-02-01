#pragma once

#include <string>
#include <vector>
#include <map>

#include "Node.h"
#include "Machine.h"

using namespace std;

class ServiceGraph
{
    public:
        ServiceGraph();
        ~ServiceGraph();

    private:
        // map of node ids to nodes contained in the graph
        map<int, Node> nodes;

        // map of machine ids to machines in the system
        map<int, Machine> machines;
 };