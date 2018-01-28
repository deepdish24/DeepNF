#include <string>
#include <set>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#include "NF.cpp"

using namespace std;

class ServiceGraphNode {

	// name of container
	string node_id;
	// network function running on the node
	NF nf;
	// set of node IDs, representing out neighbors of this node
	set<string> neighbors;

public:

	// ID of the machine that this container will be on
	string machine_id;
	
	ServiceGraphNode(string n, NF f) {
		node_id = n; 
		nf = f;
	}

	string get_id() const { return node_id; }
	
	NF get_nf() { return nf; }

	set<string> get_neighbors() { return neighbors; }

	void add_neighbor(string neighbor) {
		neighbors.insert(neighbor);
	}

	void remove_neighbor(string neighbor) {
		neighbors.erase(neighbor);
	}
};