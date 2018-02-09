#pragma once
#include <map>
#include <vector>

#include "RuntimeNode.h"
#include "Machine.h"

class MachineConfigurator {
	
private:

    // maps machine id to machine object
    std::map<string, Machine> machine_map;
    // maps node name to machine object
    std::map<string, Machine> node_machine_map;
	// maps node name to node ip address
	std::map<string, string> node_ip_map;

public:
	// machine id of the machine to be configured
	string machine_id;
	// list of nodes in the entire system
	std::vector<RuntimeNode> nodes;

	
	MachineConfigurator(string id);

	void make_config_dir(string node_name);

	string get_config_dir(string node_name);

	string get_docker_image_name(string node_name, NF nf);

	string get_dockerfile(NF nf);

	string get_bridge_ip();

	string get_node_machine_id(string node_name);

	string get_node_ip(string node_name);

};
