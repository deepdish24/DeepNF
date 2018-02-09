#ifndef MACHINECONFIGURATOR_H
#define MACHINECONFIGURATOR_H

#include <map>
#include <vector>

#include "RuntimeNode.cpp"

class MachineConfigurator {
	
private:

	// maps machine id to IP address
	map<string, string> machine_ip_map;
	// maps machine id to the OVS bridge's IP address
	map<string, string> machine_bridge_ip_map;

public:
	// machine id of the machine to be configured
	string machine_id;
	// list of nodes in the entire system
	vector<RuntimeNode> nodes;

	
	MachineConfigurator(string id);

	void make_config_dir(string node_name);

	string get_config_dir(string node_name);

	string get_docker_image_name(string node_name, NF nf);

	string get_dockerfile(NF nf);

	string get_bridge_ip();
};

#endif