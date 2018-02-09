#pragma once

#include <string>
#include <stdlib.h>
#include <set>

#include "NF.h"


using namespace std;

class RuntimeNode {

public:
	
	// name of container
	string name;
	// network function running on the node
	NF nf;
	// name of docker image
	string image_name;
	// directory containing the rules and config files for the network function
	string config_dir;
    // set of pointers to node neighbors
    set<RuntimeNode*> neighbors;
  
    
    RuntimeNode(string n, NF f);
    void add_neighbor(RuntimeNode *n);

};
