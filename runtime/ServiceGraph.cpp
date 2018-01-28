#include <map>
#include <set>
#include <vector>
#include <stdexcept> 
#include <iostream>

#include "ServiceGraphNode.cpp"

using namespace std;

class ServiceGraph {

	// map from node id to ServiceGraphNode
	map<string, ServiceGraphNode> node_map;

	// map of machine id to set of node ids in the machine 
	map<string, set<string>> machine_nodes;


	/**
	 * Throws exception if ServiceGraphNode with given name does not exist in graph.
	 */
	void raise_illegal_node_exception(string n) {
		if (node_map.count(n) == 0) {
			throw invalid_argument("Given node is not present in the service graph.");  
		}
	}

public:

	/**
	 * Add a node to the graph.
	 *
	 * @param n The ServiceGraphNode to insert into the graph.
	 * @return 	True if the node was inserted successfully, false otherwise.
	 */
	bool add_node(ServiceGraphNode n) {
		string node_name = n.get_id();
		if (node_map.count(node_name) == 0) {
			node_map.insert(make_pair(node_name, n));
			return true;
		}
		return false;
	}

	/**
	 * Remove a node from the graph.
	 *
	 * @param name The id of the ServiceGraphNode to remove.
	 */
	void remove_node(string name) {
		raise_illegal_node_exception(name);
		
		// remove in edges
		map<string, ServiceGraphNode>::iterator i;
		for (i = node_map.begin(); i != node_map.end(); i++) {
			ServiceGraphNode v = i->second;
			v.remove_neighbor(name);
		}
		// remove node from node map
		node_map.erase(name);
	}

	/**
	 * Gets a list of ServiceGraphNode objects that are nodes in the graph.
	 *
	 * @return A vector of ServiceGraphNode objects.
	 */
	vector<ServiceGraphNode> get_node_list() {
		vector<ServiceGraphNode> result;
		map<string, ServiceGraphNode>::iterator i;
		for (i = node_map.begin(); i != node_map.end(); i++) {
			ServiceGraphNode n = i->second;
			result.push_back(n);
		}
		return result;
	}

	/**
	 * Gets the ServiceGraphNode object with the given node id.
	 *
	 * @return A ServiceGraphNode.
	 */
	ServiceGraphNode get_node(string id) {
		raise_illegal_node_exception(id);
		return node_map.find(id)->second
	}

	/**
	 * Adds a directed edge from the first node to the second.
	 *
	 * @param a The id of a ServiceGraphNode in the graph.
	 * @param b The id of a ServiceGraphNode in the graph.
	 */
	void add_edge(string a, string b) {
		raise_illegal_node_exception(a);
		raise_illegal_node_exception(b);
		ServiceGraphNode *from_node = &(node_map.find(a)->second);
		from_node->add_neighbor(b);
	}

	/**
	 * Removes any edge from the first node to the second.
	 *
	 * @param a The id of a ServiceGraphNode in the graph.
	 * @param b The id of a ServiceGraphNode in the graph.
	 */
	void remove_edge(string a, string b) {
		raise_illegal_node_exception(a);
		raise_illegal_node_exception(b);
		ServiceGraphNode *from_node = &(node_map.find(a)->second);
		from_node->remove_neighbor(b);
	}

	/**
	 * Gets the neighbors of a ServiceGraphNode with given id.
	 *
	 * @param id The id of a ServiceGraphNode in the graph.
	 * @return A vector of ServiceGraphNode objects.
	 */
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

	/**
	 * Adds node with given id to the list of nodes in the machine with given machine id.
	 * Creates a new entry for machine with given id, if it does not currently exist.
	 *
	 * @param macid The id of a machine.
	 * @param nodeid The id of a ServiceGraphNode
	 */
	void assign_node_to_machine(string macid, string nodeid) {
		raise_illegal_node_exception(nodeid);
		map<string, set<string>>::iterator it = machine_nodes.find(macid)
		if (it == machine_nodes.end()) {
			set<string> v;
			v.insert(nodeid);
			machine_nodes.insert(make_pair(macid, v));
		} else {
			it->second.insert(nodeid);
		}
	}

};

/**
 * Testing
 */
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


