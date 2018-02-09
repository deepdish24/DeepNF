#ifndef RUNTIMENODE_CPP
#define RUNTIMENODE_CPP

#include "RuntimeNode.h"

RuntimeNode::RuntimeNode(string n, NF f) {
	name = n; 
	nf = f;
}

void RuntimeNode::add_neighbor(RuntimeNode *n) {
	neighbors.insert(n);
}

#endif