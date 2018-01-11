#include <map>
#include <set>
#include <stdexcept> 
#include <iostream>

#include "Node.cpp"

using namespace std;

class ServiceGraph {

	map<string, Node> nodes;

	map<string, set<string> > adjList;

	map<string, int> indegree;

	void raise_illegal_node_exceptions(Node n) {
		if (nodes.count(n.get_name()) == 0) {
			throw invalid_argument("Given node is not present in the service graph.");  
		}
	}

public:

	bool add_node(Node n) {
		string node_name = n.get_name();
		if (nodes.count(node_name) == 0) {
			nodes.insert(make_pair(node_name, n));
			return true;
		}
		return false;
	}

	void remove_node(Node n) {
		raise_illegal_node_exceptions(n);
		string name = n.get_name();
		// remove out edges
		if (adjList.count(name) > 0) {
			adjList.erase(name);
		}
		// remove in edges
		map<string, set<string> >::iterator i;
		for (i = adjList.begin(); i != adjList.end(); i++) {
			set<string> v = i->second;
			v.erase(name);
		}
		// remove node from all data structures
		indegree.erase(name);
		nodes.erase(name);
	}

	bool add_edge(Node a, Node b) {
		raise_illegal_node_exceptions(a);
		raise_illegal_node_exceptions(b);
		string a_name = a.get_name();
		string b_name = b.get_name();
		if (adjList.count(a_name) == 0) {
			set<string> s;
			s.insert(b_name);
			adjList.insert(make_pair(a_name, s));
			indegree[b_name]++;
			return true;
		} else {
			bool result = adjList[a_name].insert(b_name).second;
			if (result) {
				indegree[b_name]++;
			}
			return result;
		}
	}

	void remove_edge(Node a, Node b) {
		raise_illegal_node_exceptions(a);
		raise_illegal_node_exceptions(b);
		string a_name = a.get_name();
		string b_name = b.get_name();
		if (adjList.count(a_name) > 0) {
			if (adjList[a_name].count(b_name) > 0) {
				adjList[a_name].erase(b_name);
				if (indegree[b_name] <= 1) {
					indegree.erase(b_name);
				} else {
					indegree[b_name] --;
				}
			}
			
			if (adjList[a_name].size() == 0) {
				adjList.erase(a_name);
			}
		}
	}

	set<Node*> get_neighbors(Node n) {
		raise_illegal_node_exceptions(n);
		string node_name = n.get_name();
		set<Node*> result;
		if (adjList.count(node_name) > 0) {
			set<string>::iterator i;
			for (i = adjList[node_name].begin(); i != adjList[node_name].end(); i++) {
				Node *p = &nodes.find(*i)->second;
				result.insert(p);
			}
		}
		return result;
	}

	int size() {
		return nodes.size();
	}

	bool is_source(Node n) {
		raise_illegal_node_exceptions(n);
		return indegree.count(n.get_name()) == 0;
	}

	bool is_sink(Node n) {
		raise_illegal_node_exceptions(n);
		return adjList.count(n.get_name()) == 0;
	}
};

// int main(int argc, char *argv[]) {
// 	ServiceGraph g;
// 	Node n1 ("n1", snort);
// 	Node n2 ("n2", haproxy);
// 	g.add_node(n1);
// 	g.add_node(n2);
// 	g.add_edge(n1, n2);
// 	set<Node*>::iterator i = g.get_neighbors(n1).begin();
// 	Node *p = *i;
// 	cout << p->get_name() << endl;
// 	return 0;
// }


