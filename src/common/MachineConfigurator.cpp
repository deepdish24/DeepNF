#include <errno.h>
#include <sys/stat.h>

#include "MachineConfigurator.h"


MachineConfigurator::MachineConfigurator(Machine* m) {
	machine_id = m->get_id();
	machine_map.insert(std::make_pair(machine_id, m));
}

int MachineConfigurator::get_machine_id() { return machine_id; }

void MachineConfigurator::make_config_dir(int node_id) {
	std::string path = get_config_dir(node_id);

	if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == EEXIST) {
		system(("rm -r " + path).c_str());
		mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
}

std::string MachineConfigurator::get_config_dir(int node_id) {
	return "/home/ec2-user/" + std::to_string(node_id) + "/";
}

std::string MachineConfigurator::get_docker_image_name(int node_id, NF nf) {
	switch(nf) {
		case snort: return std::to_string(node_id) + "_snort";
		break;
		case haproxy: return std::to_string(node_id) + "_haproxy";
		break;
		default: return "";
	}
}

std::string MachineConfigurator::get_dockerfile(NF nf) {
	std::string path = "/home/ec2-user/DeepNF/src/runtime/nf_configs/";
	switch(nf) {
		case snort: path += "snort";
		break;
		case haproxy: path += "haproxy";
		break;
		default: return "";
	}

	path += "/*";

	return path;
}

Machine* MachineConfigurator::get_machine_with_id(int mac_id) {
	return machine_map.at(mac_id);
}

RuntimeNode* MachineConfigurator::get_node_with_id(int node_id) {
	return node_map.at(node_id);
}

std::vector<RuntimeNode*> MachineConfigurator::get_nodes_for_machine(int mac_id) {
	std::vector<RuntimeNode*> result;
	
	Machine* m = machine_map.at(mac_id);
	for (int i : m->get_node_ids()) {
		result.push_back(node_map.at(i));
	}

	return result;
}

std::map<int, Machine*> MachineConfigurator::get_machine_map() {
    return machine_map;
}

std::map<int, RuntimeNode*> MachineConfigurator::get_node_map() {
    return node_map;
}


void MachineConfigurator::add_machine(Machine* m) {
	machine_map.insert(std::make_pair(m->get_id(), m));
}

void MachineConfigurator::add_node(RuntimeNode* n) {
	node_map.insert(std::make_pair(n->get_id(), n));
}




