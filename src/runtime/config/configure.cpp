#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <unordered_map>
#include "../../setup/json.hpp"

#include "../../common/RuntimeNode.h"
#include "../../common/MachineConfigurator.h"
#include "../../common/ServiceGraphUtil.h"

using json = nlohmann::json;

std::string merger_ip_port;
std::unordered_map<int, int> nodeid_to_port;
std::unordered_map<int, std::string> nodeid_to_network;

MachineConfigurator get_machine_configurator(int port) {
	std::cout << "get machine config called with port: " << port << std::endl;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "Cannot open socket\n");
		exit(1);
	}
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr*) &servaddr, sizeof(struct sockaddr)) < 0) {
		std::cout << "cannot bind!" << std::endl;
	}

	listen(sockfd, 10);

	const char* ack = "ACK";
	char buffer[1000];

	std::cout << "code here" << std::endl;

	while (true) {
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof(clientaddr);
		int fd = accept(sockfd, (struct sockaddr*) &clientaddr, &clientaddrlen);

		bzero(buffer, sizeof(buffer));
		std::cout << "starting to read from socket" << std::endl;
		int x = read(fd, buffer, sizeof(buffer));
		if (x == 0) {
			std::cout << "read failed" << std::endl;
			break;
		}
		write(fd, ack, strlen(ack));
		sleep(1);
		close(fd);
		break;
	}

	std::string config(buffer, sizeof(buffer));
	MachineConfigurator *mc = service_graph_util::string_to_machine_configurator(config);
	return *(mc);
}

/**
 * Returns a list of nodes on this machine
 */
std::vector<RuntimeNode*> get_internal_nodes(MachineConfigurator c) {
	return c.get_nodes_for_machine(c.get_machine_id());
}

bool is_source_node(RuntimeNode* n, std::vector<RuntimeNode*> nodes) {
	std::set<int> nbrs;
	for (RuntimeNode* n : nodes) {
		std::vector<int> ns = n->get_neighbors();
		nbrs.insert(ns.begin(), ns.end());
	}

	return nbrs.count(n->get_id()) == 0;
}

/**
* Function creates directory to setup docker container
* at path specified by config_dir
*/
void make_config_dir(std::string config_dir) {
    system(("mkdir " + config_dir).c_str());
}

/**
* Function copies specified file into config_dir
* (assuming such a directory exists)
*/
void copy_dockerfile(std::string file, std::string config_dir, std::string to_root, std::string dependencies) {
   // system(("cp -R " + to_root + "DeepNF/" + " " + config_dir).c_str());
    system(("cp " + file + " " + config_dir).c_str());
    std::string line;
    std::ifstream in(dependencies);
    while (std::getline(in, line)) {
        system(("cp " + to_root + line + " " + config_dir).c_str());
    }
    in.close();
}

/**
* Function builds docker image based in config directory
* (assuming such a directory exists)
*/
void build_docker_image(std::string image_name, std::string config_dir) {
    system(("docker build -t=" + image_name + " " + config_dir).c_str());
}

/**
* Function starts docker container
*/
void start_docker_container(std::string container_name, std::string image_name) {
    system(("docker run -d -t -i --name " + container_name + " " + image_name + ":latest /bin/bash").c_str());
}

/**
 * Creates the containers using Dockerfiles.
 */
void setup_nodes(MachineConfigurator conf) {
    // All config directories will be stored at root of instance
    // function assumes ./src/runtime/config/configure is run 
    // from build directory

	/*std::string to_root = "../../";
	std::string path_to_merger_dockerfile = to_root + "DeepNF/src/runtime/merger/Dockerfile";
    std::string path_to_fwd_dockerfile = to_root + "DeepnNF/src/runtime/forwarder/Dockerfile";
    std::string fwd_config_dir = to_root + "fwd_config";
	std::string merger_config_dir = to_root + "merger_config";*/

    /* Forwarder Container Setup */
    /*make_config_dir(merger_config_dir);
    copy_dockerfile(path_to_fwd_dockerfile, fwd_config_dir, to_root);
    start_docker_container("forwarder", "base_img");*/

    /* Merger Container Setup */
   /* make_config_dir(merger_config_dir);
    copy_dockerfile(path_to_merger_dockerfile, merger_config_dir, to_root);
    //build_docker_image("merger_image", merger_config_dir);
    start_docker_container("merger", "base_img");*/

    // list of nodes on this machine
	std::vector<RuntimeNode*> nodes = get_internal_nodes(conf);

    for (RuntimeNode* node : nodes) {
        std::string func_name = node->get_name();
        std::string func_config_dir = to_root + conf.get_config_dir(node->get_id()) + "_config";
        std::string path_to_dockerfile = to_root + conf.get_dockerfile(node->get_nf()) + "Dockerfile";
        std::string path_to_dependencies = to_root + conf.get_dockerfile(node->get_nf()) + "dependencies.txt";
        std::string image_name = conf.get_config_dir(node->get_id()) + "_img";
        std::string container_name = conf.get_config_dir(node->get_id());

        make_config_dir(func_config_dir);
        copy_dockerfile(path_to_dockerfile, func_config_dir, to_root, dependencies);
        build_docker_image(image_name, func_config_dir);
        start_docker_container(container_name, "base_img");
    }



	// create new containers for classifier and merger_old
	//system("docker run -d -t -i --name classifier ubuntu /bin/bash");
	//system("docker run -d -t -i --name merger_old ubuntu /bin/bash");
	
	/*for (RuntimeNode* n : nodes) {
		int node_id = n->get_id();
		NF node_nf = n->get_nf();

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
		system(("docker run -d -t -i --name " + n->get_name() + " " + image_name + ":latest /bin/bash").c_str());
	}*/
}

std::unordered_map<int, int> setup_bridge_ports(MachineConfigurator &conf) {
    system("sudo “PATH=$PATH” /home/ubuntu/ovs/utilities/ovs-ctl --system-id=random --no-ovs-vswitchd start");
    system("sudo “PATH=$PATH” /home/ubuntu/ovs/utilities/ovs-ctl --no-ovsdb-server start");

	// create a bridge
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl add-br ovs-br");
	
	// get bridge ip
	Machine* cur_machine = conf.get_machine_with_id(conf.get_machine_id());
	std::string bridge_ip = cur_machine->get_bridge_ip();
	
	system(("sudo ifconfig ovs-br " + bridge_ip + " netmask 255.255.255.0 up").c_str());

	// getting ip info from bridges
	int dotinx = bridge_ip.rfind(".");
	std::string ip_assign = bridge_ip.substr(0, dotinx+1);
	int ofport_inx = atoi(bridge_ip.substr(dotinx+1).c_str());
	int ip_inx = ofport_inx + 1;

	// connect containers to the bridge
	// std::string add_port_classifier = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br eth1 classifier";
	//std::string add_port_merger = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br eth1 merger_old";
	//system(add_port_classifier.c_str());
	//system(add_port_merger.c_str());

    /* Connect Forwarder to Bridge (with one ETH) */
    /*std::string add_ip_port_forwarder = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br eth1 --ipaddress=" + ip_assign + std::to_string(ip_inx) +  " forwarder ";
    system(add_ip_port_forwarder.c_str());
    ip_inx++;*/

	std::vector<RuntimeNode*> nodes = get_internal_nodes(conf);

	/* MERGER PORT SETUP */

	// node id to eth setup map
	/*std::unordered_map<int, int> nodeid_to_eth;
	std::string add_port_merger = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br eth";
    // inx of virtual ethernet port
	int eth_inx = 1;
	auto j = json::object();
	for (RuntimeNode* n : nodes) {
		if (n->get_neighbors().size() == 0) {
			std::string command = add_port_merger + std::to_string(eth_inx) + " --ipaddress=" + 
				ip_assign + std::to_string(ip_inx) + " " + "merger";
			std::cout << "command: " << command << std::endl;
			nodeid_to_eth[n->get_id()] = eth_inx;
			j["eth" + std::to_string(eth_inx)] = n->get_name();
			system(command.c_str());
			eth_inx++;
		}
	}
    ip_inx++;
    std::string to_root = "../../";
    std::string path_to_json = to_root + "DeepNF/src/common/eth_leaf_map.json";
	std::ofstream out(path_to_json);
    out << j;
    out.close();*/
   // int eth_inx = 1;
    int port  = 8000;
	std::string add_port_command = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker add-port ovs-br";
	for (RuntimeNode* n : nodes) {
        int nodeid = n->get_id();
        std::string container_name = conf.get_config_dir(n->get_id());
        std::string func_ip = ip_assign + std::to_string(ip_inx);
        nodeid_to_network[n->get_id()] = func_ip;
        nodeid_to_port[n->get_id()] = nodeid + port;
		std::string command1 = add_port_command + " eth1 --ipaddress=" + func_ip + " " + container_name;
		std::cout << "command: " << command1 << std::endl;
		system(command1.c_str());
		//n->inport = eth_inx++;
		ip_inx++;
	}

    //Assign Machine IP and PORT for all other Functions
    int currMachine = conf.get_machine_id();
    std::map<int, Machine*> machineMap = conf.get_machine_map();
    for (auto it = machineMap.begin(); it != machineMap.end(); ++it) {
        Machine* mac = it->second;
        std::vector<RuntimeNode*> nodes_for_mac = conf.get_nodes_for_machine(mac->get_id());
        for (RuntimeNode* n : nodes_for_mac) {
            std::string func_ip = mac->get_ip() + 
        }
    }
}

/**
 * Adds OVS flow rules between the containers.
 * reference: https://paper.dropbox.com/doc/Flows-in-OpenVSwitch-nVRg9phHBr5JSZO2vFwCJ?_tk=share_copylink
 */
void make_flow_rules(MachineConfigurator conf, std::unordered_map<int,int> leaf_to_eth) {
	std::string add_flow_command = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-ofctl add-flow ovs-br in_port=";
	
	std::vector<RuntimeNode*> nodes = get_internal_nodes(conf);
    /* for each node in machine set its outputs properly
        RULES FOR SETTING output
            1. if neighbor is within machine -> then set give container ip
            2. if neighbor is outside machine -> give ip/port of other machine
            3. if node is leaf point to merger -> give ip/port of merger

        RULES FOR FORWARDER
            1. Map port from forwarder to ip of container running function */










	/*std::vector<int> source_node_inports;
	
	for (RuntimeNode* n : nodes) {
		
		if (is_source_node(n, nodes)) { // flow from classifier to this node
			source_node_inports.push_back(n->inport);
			// system((add_flow_command + "1,actions=" + std::to_string(n.inport)).c_str());
		}

		if (n->get_neighbors().size() == 0) { // if node is a sink, flow from this node to merger_old
			int merger_port = leaf_to_eth[n->get_id()];
			system((add_flow_command + std::to_string(n->outport) + ",actions=" + std::to_string(merger_port)).c_str());
		} else { // flow from output port of this node to all its successors ports
			std::vector<int> neighbors = n->get_neighbors();
			std::string outport_ports = "";
			for (int j = 0; j < (int) neighbors.size(); j++) {
				RuntimeNode* neighbor = conf.get_node_with_id(neighbors[j]);
				outport_ports += std::to_string(neighbor->inport);
				if (j < (int)neighbors.size() - 1) {
					outport_ports += ",";
				}
			}
			system((add_flow_command + std::to_string(n->outport) + ",actions=" + outport_ports).c_str());
		}
	}
	std::string outport_ports = "";
	int i = 0;
	for (int p : source_node_inports) {
		if (i == (int) source_node_inports.size() - 1) {
			outport_ports += std::to_string(p);
		} else {
			outport_ports += std::to_string(p) + ",";
		}
		i++;
	}

	//system((add_flow_command + "1,actions=" + outport_ports).c_str());*/
}

/**
 * Runs the NF on each node
 */
void start_network_functions(MachineConfigurator c) {
	std::string docker_exec_command = "docker exec -d -t -i ";
	std::vector<RuntimeNode*> nodes = get_internal_nodes(c);
	std::string exec_nf_cmd;
	for (RuntimeNode* n : nodes) {
		std::string exec_nf_cmd = docker_exec_command + n->get_name() + " ";
		switch(n->get_nf()) {
			case dnf_firewall:
				// TODO (deepan): write correct command to setup firewall
				exec_nf_cmd += "";
				break;

			default:
				break;

//		case snort:
//			exec_nf_cmd += "snort -N -A console -q -c /etc/snort/snort.conf -Q -i eth1:eth2";
//			break;
//		case haproxy:
//			exec_nf_cmd += "service haproxy start";
//			break;
		}
		system(exec_nf_cmd.c_str());
	}
}

void reset(MachineConfigurator c) {
	std::string del_ports_cmd = "sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-docker del-ports ovs-br ";

	// clean up merger_old and classifier
	system((del_ports_cmd + "classifier").c_str());
	system((del_ports_cmd + "merger_old").c_str());
	system("docker stop classifier merger_old; docker rm classifier merger_old");

	std::vector<RuntimeNode*> nodes = get_internal_nodes(c);
	for (RuntimeNode* n : nodes) {
		// remove all veth pairs for this node
		system((del_ports_cmd + n->get_name()).c_str());
		// stop and remove the docker container for this node
		system(("docker stop " + n->get_name() + "; docker rm " + n->get_name()).c_str());
	}

	// delete the bridge
	system("sudo \"PATH=$PATH\" /home/ec2-user/ovs/utilities/ovs-vsctl del-br ovs-br");
}

/**
 * Takes in node info, NF config files and flow rules from user and automates the setup of runtime components.
 * Optional flag -r to remove all resources and undo the configuration.
 */
int main(int argc, char *argv[]) {
	int c;
	int port = 10000;
	opterr = 0;
	bool needReset = false;

	while ((c = getopt(argc, argv, "p:r")) != -1) {
		switch(c) {
			case 'p':
				port = atoi(optarg);
				break;
			case 'r':
				needReset = true;
				break;
		}
	}
    merger_ip_port = std::string merger_info(argv[c]);
	MachineConfigurator conf = get_machine_configurator(port);

    

	/*int machineId = conf.get_machine_id();
	Machine* mac = conf.get_machine_with_id(machineId);
	std::cout << mac->get_bridge_ip() << std::endl;*/

	if (needReset) {
		reset(conf);
	} else {
		// making a dummy service graph
        std::cout << "graph here!" << std::endl;
		setup_nodes(conf);
		/*std::unordered_map<int, int> leaf_to_eth = setup_bridge_ports(conf);
		make_flow_rules(conf, leaf_to_eth);
		start_network_functions(conf);*/
	}

	return 0;
}

