#include <vector>

#include "ServiceGraphNode.cpp"
#include "NFConfigFile.cpp"

using namespace std;

class RuntimeNode: public ServiceGraphNode {

	// // name of docker image
	// string docker_image_name;

	// // directory containing the rules and config files for the network function
	// string docker_config_dir;

	// list of NF config files
	vector<NFConfigFile> nf_config_files;

	// void set_docker_image_name() {
	// 	switch(nf) {
	// 		case snort: docker_image_name = node_id + "_snort";
	// 		break;
	// 		case haproxy: docker_image_name = node_id + "_haproxy";
	// 		break;
	// 		default: break;
	// 	}
	// }

public:

	// OpenFlow input port number
	int inport;
	// OpenFlow output port number
	int outport;
	
	// IP address of the container
	string container_ip;

	RuntimeNode(string n, NF nf) : ServiceGraphNode (n, nf) {
		ServiceGraphNode(n, nf);
		// docker_config_dir = "/home/ec2-user/" + node_id;
		inport = 0;
		outport = 0;
		// set_docker_image_name();
	}

	// string get_docker_image_name() { return docker_image_name; }
	

	// string get_docker_config_dir() { return docker_config_dir; }

	void add_nf_config_file(NFConfigFile f) { nf_config_files.push_back(f); }

	vector<NFConfigFile> get_nf_config_files() { return nf_config_files; }

	// void create_docker_config_dir() {
	// 	if (mkdir(docker_config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == EEXIST) {
	// 		system(("rm -r " + docker_config_dir).c_str());
	// 		mkdir(docker_config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	// 	}
	// }
};