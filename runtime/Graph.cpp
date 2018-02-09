#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

template <class T>
class ServiceGraph {
	
	// map from node id to ServiceGraphNode
	map<string, T> node_map;

	// map of machine id to set of node ids in the machine 
	map<string, set<string> > machine_nodes;

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
	virtual bool add_node(T n) = 0;

	/**
	 * Remove a node from the graph.
	 *
	 * @param name The id of the ServiceGraphNode to remove.
	 */
	virtual void remove_node(string name) = 0;

	/**
	 * Gets a list of ServiceGraphNode objects that are nodes in the graph.
	 *
	 * @return A vector of ServiceGraphNode objects.
	 */
	virtual vector<T> get_node_list() = 0;

	/**
	 * Gets the ServiceGraphNode object with the given node id.
	 *
	 * @return A ServiceGraphNode.
	 */
	virtual T get_node(string id) = 0;

	/**
	 * Adds a directed edge from the first node to the second.
	 *
	 * @param a The id of a ServiceGraphNode in the graph.
	 * @param b The id of a ServiceGraphNode in the graph.
	 */
	virtual void add_edge(string a, string b) = 0;

	/**
	 * Removes any edge from the first node to the second.
	 *
	 * @param a The id of a ServiceGraphNode in the graph.
	 * @param b The id of a ServiceGraphNode in the graph.
	 */
	virtual void remove_edge(string a, string b) = 0;

	/**
	 * Gets the neighbors of a ServiceGraphNode with given id.
	 *
	 * @param id The id of a ServiceGraphNode in the graph.
	 * @return A vector of ServiceGraphNode objects.
	 */
	virtual vector<T> get_neighbors(string id) = 0;

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
		map<string, set<string> >::iterator it = machine_nodes.find(macid);
		if (it == machine_nodes.end()) {
			set<string> v;
			v.insert(nodeid);
			machine_nodes.insert(make_pair(macid, v));
		} else {
			it->second.insert(nodeid);
		}
	}

};