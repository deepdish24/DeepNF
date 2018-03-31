#include <string>
#include <set>

#ifndef SERVICEGRAPHNODE_CPP
#define SERVICEGRAPHNODE_CPP

class ServiceGraphNode {
public:
	// network function running on the node
	std::string nf;
	// set of pointers to node neighbors
	std::set<ServiceGraphNode*> neighbors;
	// parent of node
	std::set<ServiceGraphNode*> parents;
	//ServiceGraphNode *parent;
	
	ServiceGraphNode(std::string f) {
		nf = f;
	}

	void set_name(std::string f) {
		nf = f;
	}

	void add_neighbor(ServiceGraphNode* n) {
		neighbors.insert(n);
	}

	void add_parent(ServiceGraphNode* n) {
		parents.insert(n);
	}

	bool isLeaf() {
		return neighbors.empty();
	}

	bool isRoot() {
		return parents.empty();
	}
};

#endif