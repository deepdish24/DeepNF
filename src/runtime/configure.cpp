
#include <stdlib.h>
#include <iostream>
#include "../common/Node.h"

using namespace std;



/**
 * Creates the containers using Dockerfiles.
 */
void setup_nodes(Node *nodes, int num_nodes) {
	// create new containers for classifier and merger
	system("docker run -t -i --name classifier ubuntu");
	system("docker run -t -i --name merger ubuntu");
	
	for (int i = 0; i < num_nodes; i += 1) {
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

/**
 * Adds OVS flow rules between the containers.
 * reference: https://paper.dropbox.com/doc/Flows-in-OpenVSwitch-nVRg9phHBr5JSZO2vFwCJ?_tk=share_copylink
 */
void make_flow_rules() {
	// TODO: get the format from Vicky and automate...

	// create a bridge
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl del-br ovs-br1");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl add-br ovs-br1");
	system("sudo ifconfig ovs-br1 173.16.1.1 netmask 255.255.255.0 up");

	// connect containers to bridge
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 classifier --ipaddress=173.16.1.2");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 container1");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth2 container1");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 containe2");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth2 container2");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 merger");

	// add flow rules
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-ofctl add-flow ovs-br1 in_port=1,actions=2");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-ofctl add-flow ovs-br1 in_port=3,actions=4");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-ofctl add-flow ovs-br1 in_port=5,actions=6");
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
	
	Node n1 ("n1", snort);
	Node n2 ("n2", haproxy);
	Node nodes[2] = { n1, n2 };
	setup_nodes(nodes, 2);
	make_flow_rules();
	start_network_functions();
	return 0;
}