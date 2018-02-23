#include <stdlib.h>
#include <iostream>

#include "../common/RuntimeNode.h"
#include "../common/MachineConfigurator.h"

MachineConfigurator get_machine_configurator() {
	RuntimeNode n1 (1, snort);
	RuntimeNode n2 (2, haproxy);
	RuntimeNode n3 (3, snort);
	n1.ip = "173.16.1.2";
	n2.ip = "173.16.1.3";
	n3.ip = "173.16.1.4";
	n1.add_neighbor(2);
	n2.add_neighbor(3);

	Machine m (0);
	m.set_bridge_ip("173.16.1.1");
	m.add_node_id(1);
	m.add_node_id(2);
	m.add_node_id(3);

	MachineConfigurator c (m);
	
	c.add_node(n1);
	c.add_node(n2);
	c.add_node(n3);

	return c;
}

/**
 * Returns a list of nodes on this machine
 */
std::vector<RuntimeNode> get_internal_nodes(MachineConfigurator c) {
	return c.get_nodes_for_machine(c.get_machine_id());
}

bool is_source_node(RuntimeNode n, vector<RuntimeNode> nodes) {
	std::set<int> nbrs;
	for (RuntimeNode n : nodes) {
		std::vector<int> ns = n.get_neighbors();
		nbrs.insert(ns.begin(), ns.end());
	}

	return nbrs.count(n.get_id()) == 0;
}

/**
 * Creates the containers using Dockerfiles.
 */
void setup_nodes(MachineConfigurator conf) {

	std::vector<RuntimeNode> nodes = get_internal_nodes(conf);

	// create new containers for classifier and merger
	system("docker run -t -i --name classifier ubuntu echo \"classifier created\"");
	system("docker run -t -i --name merger ubuntu echo \"merger created\"");	
	
	for (RuntimeNode n : nodes) {
		int node_id = n.get_id();
		NF node_nf = n.get_nf();

		std::string dockerfile_path = conf.get_dockerfile(node_nf);
		if (dockerfile_path == "") {
			// TODO: raise exception
		}

		std::string config_dir = conf.get_config_dir(node_id);
		conf.make_config_dir(node_id);

		std::string image_name = conf.get_docker_image_name(node_id, node_nf);
		if (image_name == "") {
			// TODO: raise exception
		}
		// store Dockerfile in node config directory
		system(("cp " + dockerfile_path + " " + config_dir).c_str());
		
		// build Docker image with the network function installed and configured
		system(("docker build -t=" + image_name + " " + config_dir).c_str());

		// create a new Docker container for the node
		system(("docker run --name " + n.get_name() + " -d -t -i " + image_name + ":latest").c_str());
	}
}

MachineConfigurator setup_bridge_ports(MachineConfigurator conf) {

	// create a bridge
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl del-br ovs-br1");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl add-br ovs-br1");
	
	// get bridge ip
	Machine cur_machine = conf.get_machine_with_id(conf.get_machine_id());
	std::string bridge_ip = cur_machine.get_bridge_ip();
	
	system(("sudo ifconfig ovs-br1 " + bridge_ip + " netmask 255.255.255.0 up").c_str());
	
	// connect containers to the bridge
	std::string add_port_classifier = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 classifier";
	std::string add_port_merger = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 merger";
	system(add_port_classifier.c_str());
	system(add_port_merger.c_str());

	std::vector<RuntimeNode> nodes = get_internal_nodes(conf);

	int ofport = 3;
	for (RuntimeNode n : nodes) {
		
		std::string add_port_command = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1";
		// std::string node_id = std::to_string(n.get_id());
		switch(n.get_nf()) {
			case snort:
			system((add_port_command + " eth3 " + n.get_name()).c_str());
			n.inport = ofport ++;
			system((add_port_command + " eth4 " + n.get_name()).c_str());
			n.outport = ofport ++;
			break;
			
			case haproxy:
			system((add_port_command + " eth1 " + n.get_name() + " --ipaddress=" + n.ip).c_str());
			n.inport = ofport ++;
			n.outport = n.inport;
			break;
			
			default: break;
		}
		conf.update_node(n);
	}

	return conf;
}

/**
 * Adds OVS flow rules between the containers.
 * reference: https://paper.dropbox.com/doc/Flows-in-OpenVSwitch-nVRg9phHBr5JSZO2vFwCJ?_tk=share_copylink
 */
void make_flow_rules(MachineConfigurator conf) {
	std::string add_flow_command = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-ofctl add-flow ovs-br1 in_port=";
	
	std::vector<RuntimeNode> nodes = get_internal_nodes(conf);
	std::vector<int> source_node_inports;
	
	for (RuntimeNode n : nodes) {
		
		if (is_source_node(n, nodes)) { // flow from classifier to this node
			std::cout << n.inport << std::endl;
			source_node_inports.push_back(n.inport);
			// system((add_flow_command + "1,actions=" + std::to_string(n.inport)).c_str());
		}

		if (n.get_neighbors().size() == 0) { // if node is a sink, flow from this node to merger
			system((add_flow_command + std::to_string(n.outport) + ",actions=2").c_str());
		} else { // flow from output port of this node to all its successors ports
			std::vector<int> neighbors = n.get_neighbors();
			std::string outport_ports = "";
			for (int j = 0; j < (int) neighbors.size(); j++) {
				RuntimeNode neighbor = conf.get_node_with_id(neighbors[j]);
				outport_ports += std::to_string(neighbor.inport);
				if (j < (int)neighbors.size() - 1) {
					outport_ports += ",";
				}
			}
			std::cout << (add_flow_command + std::to_string(n.outport) + ",actions=" + outport_ports) << std::endl;
			system((add_flow_command + std::to_string(n.outport) + ",actions=" + outport_ports).c_str());
		}
	}
	std::string outport_ports = "";
	int i = 0;
	for (int p : source_node_inports) {
		if (i == source_node_inports.size() - 1) {
			outport_ports += std::to_string(p);
		} else {
			outport_ports += std::to_string(p) + ",";
		}
		i++;
	}
	std::cout << (add_flow_command + "1,actions=" + outport_ports) << std::endl;
	system((add_flow_command + "1,actions=" + outport_ports).c_str());
}

/**
 * Runs the NF on each node
 */
void start_network_functions(MachineConfigurator c) {
	std::string docker_exec_command = "docker run -it ";
	std::vector<RuntimeNode> nodes = get_internal_nodes(c);
	std::string exec_nf_cmd;
	for (RuntimeNode n : nodes) {
		std::string exec_nf_cmd = docker_exec_command + c.get_docker_image_name(n.get_id(), n.get_nf()) + " ";
		switch(n.get_nf()) {
		case snort:
			exec_nf_cmd += "snort -N -A console -q -c /etc/snort/snort.conf -Q -i eth3:eth4"; 
			break;
		case haproxy:
			exec_nf_cmd += "service haproxy start";
			break;
		}
		system(exec_nf_cmd.c_str());
	}
}

/**
 * Takes in node info, NF config files and flow rules from user and automates the setup of runtime components.
 */
int main(int argc, char *argv[]) {
	
	// making a dummy service graph
	MachineConfigurator conf = get_machine_configurator();
	setup_nodes(conf);
	conf = setup_bridge_ports(conf);
	make_flow_rules(conf);
	start_network_functions(conf);
	return 0;
}