
#include <stdlib.h>
#include <iostream>

#include "ServiceGraph.cpp"

using namespace std;


/**
 * Creates the containers using Dockerfiles.
 */
void setup_nodes(vector<Node> nodes) {

	// create new containers for classifier and merger
	system("docker run -t -i --name classifier ubuntu echo \"classifier created\"");
	system("docker run -t -i --name merger ubuntu echo \"merger created\"");	
	
	for (int i = 0; i < nodes.size(); i++) {
		Node n = nodes[i];
		// store Dockerfile in node config directory
		switch(n.get_nf()) {
			case snort:
			system(("cp /home/ec2-user/DeepNF/runtime/nf_configs/snort/Dockerfile " + n.get_config_dir()).c_str());
			break;
			case haproxy:
			system(("cp /home/ec2-user/DeepNF/runtime/nf_configs/haproxy/Dockerfile " + n.get_config_dir()).c_str());
			break;
			default:
			break;
		}
		// build Docker image with the network function installed and configured
		system(("docker build -t=" + n.get_image_name() + " " + n.get_config_dir()).c_str());

		// create a new Docker container for the node
		system(("docker run --name " + n.get_name() + " -t -i " + n.get_image_name() + ":latest").c_str());
	}
}

void setup_bridge_ports(vector<Node> nodes) {
	
	// create a bridge
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl del-br ovs-br1");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl add-br ovs-br1");
	system("sudo ifconfig ovs-br1 173.16.1.1 netmask 255.255.255.0 up");

	// connect containers to the bridge
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 classifier --ipaddress=173.16.1.2");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 merger");

	int ofport = 3;
	int ip_suffix = 3;
	for (int i = 0; i < nodes.size(); i++) {
		Node n = nodes[i];
		string add_port_command = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1";
		switch(n.get_nf()) {
			case snort:
			system((add_port_command + " eth1 " + n.get_name()).c_str());
			n.inport = ofport ++;
			system((add_port_command + " eth2 " + n.get_name()).c_str());
			n.outport = ofport ++;
			break;
			
			case haproxy:
			system((add_port_command + " eth1 " + n.get_name() + " --ipaddress=173.16.1." + to_string(ip_suffix ++)).c_str());
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
	ServiceGraph g;
	Node n1 ("n1", snort);
	Node n2 ("n2", haproxy);
	g.add_node(n1);
	g.add_node(n2);

	vector<Node> nodes = g.get_nodes();
	setup_nodes(nodes);
	setup_bridge_ports(nodes);
	
	make_flow_rules(g);
	start_network_functions();
	return 0;
}

