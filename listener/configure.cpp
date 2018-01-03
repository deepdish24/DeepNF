
#include <stdlib.h>
#include <iostream>

using namespace std;

/**
 * Creates the containers using Dockerfiles.
 */
void make_containers() {
	// create new containers for classifier and merger
	system("docker run -t -i --name classifier ubuntu");
	system("docker run -t -i --name merger ubuntu");

	// build docker image with Snort installed and configured
	system("docker build -t=\"snort\" ../runtime/nf_configs/snort");
	
	// create 2 Snort containers
	system("docker run --name container1 -t -i snort:latest");
	system("docker run --name container2 -t -i snort:latest");
}

/**
 * Adds OVS flow rules between the containers.
 * reference: https://paper.dropbox.com/doc/Flows-in-OpenVSwitch-nVRg9phHBr5JSZO2vFwCJ?_tk=share_copylink
 */
void make_flow_rules() {
	// create a bridge
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl del-br ovs-br1");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl add-br ovs-br1");
	system("sudo ifconfig ovs-br1 173.16.1.1 netmask 255.255.255.0 up");

	// connect containers to bridge
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 classifier --ipaddress=173.16.1.2");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 container1");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth2 container1");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 container2");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth2 container2");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br1 eth1 merger");

	// add flow rules
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-ofctl add-flow ovs-br1 in_port=1,actions=2");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-ofctl add-flow ovs-br1 in_port=3,actions=4");
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-ofctl add-flow ovs-br1 in_port=5,actions=6");
}

/**
 * Runs Snort on each container.
 */
void start_network_functions() {
	
}

/**
 * Sets up 2 containers container1 and container2 with snort,
 * creates a flow from container1 to container2, 
 * and then runs snort on both containers.
 */
int main(int argc, char *argv[]) {
	make_containers();
	make_flow_rules();
	start_network_functions();
	return 0;
}