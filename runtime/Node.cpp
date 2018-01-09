#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#include "NF.cpp"

using namespace std;

class Node {

// name of container
string name;
// network function running on the node
NF nf;
// name of docker image
string image_name;
// directory containing the rules and config files for the network function
string config_dir;

// successor ports in the flow graph
int *successors;
int num_successors;

// vector<string> config_files;

void set_image_name() {
	switch(nf) {
		case snort: image_name = name + "_snort";
		break;
		case haproxy: image_name = name + "_haproxy";
		break;
		default: break;
	}
}

public:

	// OpenFlow input port number
	int inport;
	// OpenFlow output port number
	int outport;
	
	Node(string n, NF f) {
		name = n; 
		nf = f;
		config_dir = "/home/ec2-user/" + name;
		inport = 0;
		outport = 0;
		successors = NULL;
		set_image_name();
	}

	string get_name() { return name; }
	
	NF get_nf() { return nf; }

	string get_image_name() { return image_name; }
	
	string get_config_dir() { return config_dir; }

	int* get_successors() { return successors; }

	int get_num_successors() { return num_successors; }

	void set_successors(int *s, int n) {
		successors = new int[n];
		num_successors = n;
		for (int i = 0; i < n; i++) {
			successors[i] = s[i];
		}
	}

	void create_config_dir() {
		if (mkdir(config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == EEXIST) {
			system(("rm -r " + config_dir).c_str());
			mkdir(config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		}
	}

	// vector<string> get_config_files() { return config_files; };

	// void add_config_file(string path) { config_files.push_back(path); };
};
