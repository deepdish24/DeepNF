#include <map>
#include <set>
#include <vector>
#include <stdexcept> 
#include <iostream>

#include "ServiceGraphNode.cpp"

using namespace std;

class ServiceGraph {

	map<string, ServiceGraphNode> node_map;

	// map<string, set<string> > adjList;

	// map<string, int> indegree;

	void raise_illegal_node_exception(string n) {
		if (node_map.count(n) == 0) {
			throw invalid_argument("Given node is not present in the service graph.");  
		}
	}

public:

	bool add_node(ServiceGraphNode n) {
		string node_name = n.get_id();
		if (node_map.count(node_name) == 0) {
			node_map.insert(make_pair(node_name, n));
			return true;
		}
		return false;
	}

	void remove_node(string name) {
		raise_illegal_node_exception(name);
		// // remove out edges
		// if (adjList.count(name) > 0) {
		// 	adjList.erase(name);
		// }
		// remove in edges
		map<string, ServiceGraphNode>::iterator i;
		for (i = node_map.begin(); i != node_map.end(); i++) {
			ServiceGraphNode v = i->second;
			v.remove_neighbor(name);
		}
		// remove node from all data structures
		// indegree.erase(name);
		node_map.erase(name);
	}

	vector<ServiceGraphNode> get_node_list() {
		vector<ServiceGraphNode> result;
		map<string, ServiceGraphNode>::iterator i;
		for (i = node_map.begin(); i != node_map.end(); i++) {
			ServiceGraphNode n = i->second;
			result.push_back(n);
		}
		return result;
	}

	map<string, ServiceGraphNode> get_node_map() {
		return node_map;
	}

	void add_edge(string a, string b) {
		raise_illegal_node_exception(a);
		raise_illegal_node_exception(b);
		ServiceGraphNode *from_node = &(node_map.find(a)->second);
		from_node->add_neighbor(b);
		// node_map[a] = from_node;
	}

	void remove_edge(string a, string b) {
		raise_illegal_node_exception(a);
		raise_illegal_node_exception(b);
		ServiceGraphNode *from_node = &(node_map.find(a)->second);
		from_node->remove_neighbor(b);
	}

	vector<ServiceGraphNode> get_neighbors(string id) {
		raise_illegal_node_exception(id);
		ServiceGraphNode n = node_map.find(id)->second;
		set<string> neighbor_ids = n.get_neighbors();
		vector<ServiceGraphNode> result;
		for (string id : neighbor_ids) {
			map<string, ServiceGraphNode>::iterator i = node_map.find(id);
			if (i == node_map.end()) {
				throw invalid_argument("Service Graph Node with id " + id + " does not exist in service graph");
			}
			ServiceGraphNode neighbor = i->second;
			result.push_back(neighbor);
		}
		return result;
	}

	int size() {
		return node_map.size();
	}

	// bool is_source(ServiceGraphNode n) {
	// 	raise_illegal_node_exception(n);
	// 	return indegree.count(n.get_id()) == 0;
	// }

	// bool is_sink(ServiceGraphNode n) {
	// 	raise_illegal_node_exception(n);
	// 	return adjList.count(n.get_id()) == 0;
	// }
};

// int main(int argc, char *argv[]) {
// 	ServiceGraph g;
// 	ServiceGraphNode n1 ("n1", snort);
// 	ServiceGraphNode n2 ("n2", haproxy);
// 	g.add_node(n1);
// 	g.add_node(n2);
// 	string a = n1.get_id();
// 	string b = n2.get_id();
// 	g.add_edge(a, b);
// 	cout << g.get_neighbors(a).size() << endl;
// 	for (ServiceGraphNode n : g.get_neighbors(a)) {
// 		cout << n.get_id() << endl;
// 	}
// 	g.remove_edge(a, b);
// 	cout << g.get_neighbors(a).size() << endl;
// 	return 0;
// }


