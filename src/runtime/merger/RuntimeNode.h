#pragma once

#include <string>
#include <stdlib.h>
#include <set>
#include <vector>

#include "NF.h"


using namespace std;

class RuntimeNode {

private:

    // node id
    int id;
    // network function running on the node
    NF nf;
    // list of IDs of the neighbors of this node
    std::vector<int> neighbors;

public:

    // input Open Flow port number
    int inport;
    // output OpenFlow port number
    int outport;
    // machine id
    int machine_id;
    // std::string IP address assigned to this node
    std::string ip;
  
    
    RuntimeNode(int id, NF f);
    
    void add_neighbor(int node_id);

    std::vector<int> get_neighbors();

    int get_id();

    std::string get_name();

    NF get_nf();

};
