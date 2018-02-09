#ifndef MACHINECONFIGURATOR_CPP
#define MACHINECONFIGURATOR_CPP

#include <errno.h>
#include <sys/stat.h>

#include "MachineConfigurator.h"


MachineConfigurator::MachineConfigurator(string id) {
	machine_id = id;
}

void MachineConfigurator::make_config_dir(string node_name) {
	string path = get_config_dir(node_name);

	if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == EEXIST) {
		system(("rm -r " + path).c_str());
		mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
}

string MachineConfigurator::get_config_dir(string node_name) {
	return "/home/ec2-user/" + node_name;
}

string MachineConfigurator::get_docker_image_name(string node_name, NF nf) {
	switch(nf) {
		case snort: return node_name + "_snort";
		break;
		case haproxy: return node_name + "_haproxy";
		break;
		default: return NULL;
	}
}

string MachineConfigurator::get_dockerfile(NF nf) {
	string path = "/home/ec2-user/DeepNF/runtime/nf_configs/";
	switch(nf) {
		case snort: path += "snort";
		break;
		case haproxy: path += "haproxy";
		break;
		default: return NULL;
	}

	path += "Dockerfile";

	return path;
}

string MachineConfigurator::get_bridge_ip() {
	map<string, string>::iterator it = machine_bridge_ip_map.find(machine_id);
	if (it == machine_bridge_ip_map.end()) {
		return NULL;
	}
	return it->second;
}

string MachineConfigurator::get_node_machine_id(string node_name) {
	map<string, string>::iterator it = node_machine_map.find(node_name);
	if (it == node_machine_map.end()) {
		return NULL;
	}
	return it->second;
}

#endif