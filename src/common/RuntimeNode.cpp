#include "RuntimeNode.h"

RuntimeNode::RuntimeNode(int n, NF f) {
	this->id = n; 
	nf = f;
}

void RuntimeNode::add_neighbor(int node_id) {
	neighbors.push_back(node_id);
}

std::vector<int> RuntimeNode::get_neighbors() { return neighbors; }

int RuntimeNode::get_id() { return id; }

NF RuntimeNode::get_nf() { return nf; }

