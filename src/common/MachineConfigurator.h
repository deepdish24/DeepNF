#pragma once

#include <map>
#include <vector>

#include "RuntimeNode.h"
#include "Machine.h"

class MachineConfigurator {
	
private:
	
	// machine id of the machine to be configured
	int machine_id;

    // maps machine id to machine object
    std::map<int, Machine*> machine_map;

    // maps node id to node
    std::map<int, RuntimeNode*> node_map;

public:
	
	
	MachineConfigurator(Machine* m);

	void make_config_dir(int node_id);

	int get_machine_id();

	std::string get_config_dir(int node_id);

	std::string get_docker_image_name(int node_id, NF nf);

	std::string get_dockerfile(NF nf);

	Machine* get_machine_with_id(int mac_id);

	RuntimeNode* get_node_with_id(int node_id);

	std::vector<RuntimeNode*> get_nodes_for_machine(int mac_id);

	void add_machine(Machine* m);

	void add_node(RuntimeNode* n);

	void update_node(RuntimeNode* n);
};
