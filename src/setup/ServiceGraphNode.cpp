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
	ServiceGraphNode *parent;
	
	ServiceGraphNode(std::string f) {
		nf = f;
	}

	ServiceGraphNode() {
		nf = "default";
	}

	void set_name(std::string f) {
		nf = f;
	}

	void add_neighbor(ServiceGraphNode* n) {
		neighbors.insert(n);
	}

	void set_parent(ServiceGraphNode* n) {
		parent = n;
	}
};

#endif