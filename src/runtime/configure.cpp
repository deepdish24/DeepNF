#ifndef CONFIGURE_CPP
#define CONFIGURE_CPP


#include <stdlib.h>
#include <iostream>

#include "MachineConfigurator.h"
#include "RuntimeServiceGraph.cpp"

using namespace std;


MachineConfigurator get_machine_configurator() {
	MachineConfigurator c ("0");
	RuntimeNode n1 ("n1", snort);
	RuntimeNode n2 ("n2", haproxy);
	RuntimeNode n2 ("n3", snort);
	vector<RuntimeNode> nodes { n1, n2, n3 };
	c.nodes = nodes;

	return c;
}

/**
 * Creates the containers using Dockerfiles.
 */
void setup_nodes(MachineConfigurator conf) {

	// create new containers for classifier and merger
	system("docker run -t -i --name classifier ubuntu echo \"classifier created\"");
	system("docker run -t -i --name merger ubuntu echo \"merger created\"");	
	
	for (int i = 0; i < conf.nodes.size(); i++) {
		RuntimeNode n = conf.nodes[i];
		if (get_node_machine_id(n.name) != conf.machine_id) {
			continue;
		}

		string dockerfile_path = conf.get_dockerfile(n.nf);
		if (dockerfile_path == NULL) {
			// TODO: raise exception
		}

		string config_dir = conf.get_config_dir(n.name);

		string image_name = conf.get_docker_image_name(n.name, n.nf);
		if (image_name == NULL) {
			// TODO: raise exception
		}

		// store Dockerfile in node config directory
		system(("cp " + dockerfile_path + " " + config_dir).c_str());
		
		// build Docker image with the network function installed and configured
		system(("docker build -t=" + image_name + " " + config_dir).c_str());

		// create a new Docker container for the node
		system(("docker run --name " + n.name + " -t -i " + image_name + ":latest").c_str());
	}
}

void setup_bridge_ports(MachineConfigurator conf) {
	
	// create a bridge
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl del-br ovs-br1");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl add-br ovs-br1");
	
	string bridge_ip = conf.get_bridge_ip();
	if (bridge_ip == NULL) {
		// TODO: raise exception
	}
	system("sudo ifconfig ovs-br1 " + bridge_ip + " netmask 255.255.255.0 up");
	
	// connect containers to the bridge
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 classifier");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 merger");

	int ofport = 3;
	for (int i = 0; i < conf.nodes.size(); i++) {
		RuntimeNode n = conf.nodes[i];
		if (get_node_machine_id(n.name) != conf.machine_id) {
			continue;
		}

		string add_port_command = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1";
		switch(n.get_nf()) {
			case snort:
			system((add_port_command + " eth1 " + n.name).c_str());
			n.inport = ofport ++;
			system((add_port_command + " eth2 " + n.name).c_str());
			n.outport = ofport ++;
			break;
			
			case haproxy:
			system((add_port_command + " eth1 " + n.name + " --ipaddress=173.16.1." + to_string(ip_suffix ++)).c_str());
			n.inport = ofport ++;
			n.outport = n.inport;
			break;
			
			default: break;
		}
	}
}

/**
 * Adds OVS flow rules between the containers.
 * reference: https://paper.dropbox.com/doc/Flows-in-OpenVSwitch-nVRg9phHBr5JSZO2vFwCJ?_tk=share_copylink
 */
void make_flow_rules(ServiceGraph g) {
	string add_flow_command = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-ofctl add-flow ovs-br1 in_port=";
	vector<Node> nodes = g.get_nodes();
	for (int i = 0; i < nodes.size(); i++) {
		Node n = nodes[i];
		if (g.is_source(n)) { // flow from classifier to this node
			system((add_flow_command + "1,actions=" + to_string(n.inport)).c_str());
		} else if (g.is_sink(n)) { // flow from this node to merger
			system((add_flow_command + to_string(n.outport) + ",actions=2").c_str());
		} else { // flow from output port of this node to all its successors ports
			vector<Node> neighbors = g.get_neighbors(n);
			string outport_ports = "";
			for (int j = 0; j < neighbors.size(); j++) {
				outport_ports += to_string(neighbors[j].inport);
				if (j < neighbors.size() - 1) {
					outport_ports += ",";
				}
			}
			system((add_flow_command + to_string(n.outport) + ",actions=" + outport_ports).c_str());
		}
	}
}

/**
 * Runs the NF on each node
 */
void start_network_functions() {
	// TODO: implement
}

/**
 * Takes in node info, NF config files and flow rules from user and automates the setup of runtime components.
 */
int main(int argc, char *argv[]) {
	
	// making a dummy service graph
	MachineConfigurator conf = get_machine_configurator();
	
	setup_nodes(conf);
	setup_bridge_ports(g);
	
	make_flow_rules(g);
	start_network_functions();
	return 0;
}

#endif