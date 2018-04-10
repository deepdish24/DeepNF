#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unordered_map>
#include "../../setup/json.hpp"

#include "../../common/RuntimeNode.h"
#include "../../common/MachineConfigurator.h"
#include "../../common/ServiceGraphUtil.h"

using json = nlohmann::json;

std::string merger_ip;

// port associated with each NF (leaf -> port to forward packet to in merger)
// (non-leaf node -> port of current function)
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
void start_docker_container(std::string container_name, std::string image_name, int log_port) {
    std::cout << "CONTAINER START COMMAND: " << ("docker run -d -t -i --name " + container_name + " -v /home/ubuntu/DeepNF/build/log/" + container_name + ":/log -p " + std::to_string(log_port) + ":8080 " + image_name + " /bin/bash");
    system(("docker run -d -t -i --name " + container_name + " -v /home/ubuntu/DeepNF/build/log/" + container_name + ":/log -p " + std::to_string(log_port) + ":8080 " + image_name + " /bin/bash").c_str());
    // system(("docker run -d -t -i --name " + container_name + " " + image_name + " /bin/bash").c_str());
}

/**
* Function starts network function via cmd in docker container
*/
void run_docker_command(std::string container_name, std::string cmd) {
    std::cout << ("docker exec -dit " + container_name + " " + cmd + " &") << std::endl;
    system(("docker exec -dit " + container_name + " " + cmd + " &").c_str());
}

void run_lst_docker_cmd(std::string container_name, std::string cmd) {
	std::cout << ("docker exec -dit " + container_name + " " + cmd) << std::endl;
    system(("docker exec -dit " + container_name + " " + cmd).c_str());
}

/**
 * Creates the containers using Dockerfiles.
 */
void setup_nodes(MachineConfigurator conf) {
    // All config directories will be stored at root of instance
    // function assumes ./src/runtime/config/configure is run 
    // from build directory

	std::string to_root = "../../";
    // list of nodes on this machine
	std::vector<RuntimeNode*> nodes = get_internal_nodes(conf);
    std::cout << "Number of Nodes: " << nodes.size() << std::endl;

    int log_port = 5000;
    for (RuntimeNode* node : nodes) {
        std::cout << "curr node id: " << node->get_id() << std::endl;
        std::string func_name = node->get_name();
        std::string func_config_dir = to_root + conf.get_config_dir(node->get_id()) + "_config";
        std::string path_to_dockerfile = to_root + conf.get_dockerfile(node->get_nf()) + "Dockerfile";
        std::string path_to_dependencies = to_root + conf.get_dockerfile(node->get_nf()) + "dependencies.txt";
        std::string image_name = conf.get_config_dir(node->get_id()) + "_img";
        std::string container_name = conf.get_config_dir(node->get_id());

        make_config_dir(func_config_dir);
        copy_dockerfile(path_to_dockerfile, func_config_dir, to_root, path_to_dependencies);
        build_docker_image(image_name, func_config_dir);
        start_docker_container(container_name, image_name, ++log_port);
    }
}

void setup_bridge_ports(MachineConfigurator &conf) {
    system("sudo “PATH=$PATH” /home/ubuntu/ovs/utilities/ovs-ctl --system-id=random --no-ovs-vswitchd start");
    system("sudo “PATH=$PATH” /home/ubuntu/ovs/utilities/ovs-ctl --no-ovsdb-server start");

	// create a bridge
	system("sudo \"PATH=$PATH\" /home/ubuntu/ovs/utilities/ovs-vsctl add-br ovs-br");
	
	// get bridge ip
	Machine* cur_machine = conf.get_machine_with_id(conf.get_machine_id());
	std::string bridge_ip = cur_machine->get_bridge_ip();
	
	system(("sudo ifconfig ovs-br " + bridge_ip + " netmask 255.255.255.0 up").c_str());

	// getting ip info from bridges
	int dotinx = bridge_ip.rfind(".");
	std::string ip_assign = bridge_ip.substr(0, dotinx+1);
	int ofport_inx = atoi(bridge_ip.substr(dotinx+1).c_str());
	int ip_inx = ofport_inx + 1;

	std::vector<RuntimeNode*> nodes = get_internal_nodes(conf);

    // Container IP + Port assignment (all functions bind to port 8000 on container)
    int port  = 8000;
    auto arr = json::array();
	std::string add_port_command = "sudo \"PATH=$PATH\" /home/ubuntu/ovs/utilities/ovs-docker add-port ovs-br";
	for (RuntimeNode* n : nodes) {
        int nodeid = n->get_id();
        std::string container_name = conf.get_config_dir(n->get_id());
        std::string func_ip = ip_assign + std::to_string(ip_inx);
        nodeid_to_network[n->get_id()] = func_ip;
        //nodeid_to_port[n->get_id()] = nodeid + port;
        nodeid_to_port[n->get_id()] = port;

        std::vector<int> neighbors = n->get_neighbors();
        if ((int) neighbors.size() == 0) {
            auto obj = json::object();
            obj["nodeid"] = nodeid;
            obj["port"] = 8000 + nodeid;
            arr.push_back(obj);
        }

		std::string command1 = add_port_command + " eth1 " + container_name +  " --ipaddress=" + func_ip + "/24";
		std::cout << "command: " << command1 << std::endl;
		system(command1.c_str());
		ip_inx++;
	}

    //Assign Machine IP and PORT for all other Functions
    //Forwarder ip + port assignment (function forwards to 8000 - node_id - 1)
    //which then forwards to container_ip:8000
    int curr_mac_id = conf.get_machine_id();
    std::map<int, Machine*> machineMap = conf.get_machine_map();
    for (auto it = machineMap.begin(); it != machineMap.end(); ++it) {
        int mac_id = it->first;
        if (mac_id != curr_mac_id) {
            Machine* mac = it->second;
            std::vector<RuntimeNode*> nodes_for_mac = conf.get_nodes_for_machine(mac->get_id());
            for (RuntimeNode* n : nodes_for_mac) {
                std::string func_ip = mac->get_ip();
                int nodeid = n->get_id();
                nodeid_to_network[nodeid] = func_ip;
                // this is sending to port forwarder is listening on
                nodeid_to_port[nodeid] = port - nodeid - 1;
                std::vector<int> neighbors = n->get_neighbors();
                if ((int) neighbors.size() == 0) {
                    auto obj = json::object();
                    obj["nodeid"] = nodeid;
                    obj["port"] = 8000 + nodeid + 1;
                    arr.push_back(obj);
                }
            }
        }
    }
    std::ofstream out("../src/common/port_leaf_map.json");
    out << arr;
    out.close();
}

/**
 * Adds OVS flow rules between the containers.
 * reference: https://paper.dropbox.com/doc/Flows-in-OpenVSwitch-nVRg9phHBr5JSZO2vFwCJ?_tk=share_copylink
 */
void make_flow_rules(MachineConfigurator conf) {
    std::cout << "MAKE FLOW RULES CALLED" << std::endl;
	//std::string add_flow_command = "sudo \"PATH=$PATH\" /home/ubuntu/ovs/utilities/ovs-ofctl add-flow ovs-br in_port=";

    /* Forwarder Setup */
    std::string to_root = "../../";
    std::string to_fwd_exe = "./DeepNF/build/src/runtime/forwarder/forwarder ";
	
	std::vector<RuntimeNode*> nodes = get_internal_nodes(conf);

    for (RuntimeNode* node : nodes) {
        int nodeid = node->get_id();
        std::string node_ip = nodeid_to_network[nodeid];
        std::string container_port = std::to_string(8000);
        std::string fwd_port = std::to_string(8000 - nodeid - 1);
        std::string cmd = "echo \"" + fwd_port + ";" + node_ip + ":" + container_port + "\" >> ../../forwarder.txt";
        std::cout << cmd << std::endl;
        system(cmd.c_str());
        //std::cout << "line appended to fowarder.txt" << std::endl;
    }

    //start forwarder here!
    pid_t childpid = fork();

    if (childpid == -1) {
        perror("Failed to fork!");
        exit(-1);
    } else if (childpid == 0) {
        system("./src/runtime/forwarder/forwarder ../../forwarder.txt");
    } else {
        RuntimeNode* pktgenNode = NULL;

    /* Function setup */
    for (RuntimeNode* node : nodes) {
        std::string container_name = conf.get_config_dir(node->get_id());
        std::string cmdArguments = "";
        std::string function_name = "";
        NF func = node->get_nf();

        if (func == pktgen) {
            //std::cout << "on pktgen node!" << std::endl;
            pktgenNode = node;
            continue;
        }

        int nodeid = node->get_id();
        int function_port = nodeid_to_port[nodeid];
        std::string node_ip = nodeid_to_network[nodeid];

        switch(func) {
            case pktgen:
            {
                function_name = "pktgen";
                cmdArguments += "./sender -n 10 ";
                break;
            }
            case dnf_firewall:
            {
                function_name = "dnf_firewall";
                cmdArguments += "./fw " + std::to_string(function_port) + " 1";
                break;
            }
            case dnf_loadbalancer:
            {
                function_name = "dnf_loadbalancer";
                cmdArguments += "./fw " + std::to_string(function_port);
                break;
            }
            case proxy: 
            {
                function_name = "proxy";
                std::string server_ip("127.0.0.1");
                std::string server_port = std::to_string(8000);
                cmdArguments += "./proxy " + std::to_string(function_port) + " " + server_ip + " " + server_port;
                break;
            }
            case compressor:
            {
                function_name = "compressor";
                std::string newMsg("Altered!");
                cmdArguments += "./compressor " + std::to_string(function_port) + " " +  newMsg;
                break;
            }
            default:
                perror("Undefined function encountered in flow setup");
                break;
        }

        std::vector<int> neighbors = node->get_neighbors();

        if ((int) neighbors.size() == 0) {
            cmdArguments += " " + merger_ip + ":" + std::to_string(8000 + nodeid + 1);
        }

        for (int neighbor : neighbors) {
            //neighbor in machine
            std::string neighbor_ip = nodeid_to_network[neighbor];
            std::string neighbor_port = std::to_string(nodeid_to_port[neighbor]);
            cmdArguments += " " + neighbor_ip + ":" + neighbor_port;
        }
        std::cout << "FUNCTION: " << function_name << " COMMAND RUN: " << cmdArguments << 
        " (on container with ip: " << node_ip << ")" << std::endl;
        run_docker_command(container_name, cmdArguments);
        std::cout << "=============================\n";
    }

    //Set up pktgen container
    if (pktgenNode != NULL) {
        std::string pktgen_container_name = conf.get_config_dir(pktgenNode->get_id());
        std::string pktgenArgs = "./sender -n 10 ";
        for (int neighbor : pktgenNode->get_neighbors()) {
            std::string neighbor_ip = nodeid_to_network[neighbor];
            std::string neighbor_port = std::to_string(nodeid_to_port[neighbor]);
            pktgenArgs += " " + neighbor_ip + ":" + neighbor_port;
        }

    /*std::string container_before2 = "c0";
    std::string cmdBefore2 = "./fw 8000 " + merger_ip + ":8001";

    std::string container_before = "c1";
    std::string cmdBefore = "./fw 8000 173.16.1.2:8000";

   // std::cout << "COMMAND BEFORE: " << cmdBefore << std::endl;

    pktgenArgs += "173.16.1.3:11000";
    //std::cout << "COMMADN FOR PKTGEN: " << pktgenArgs << std::endl;
    run_docker_command(container_before2, cmdBefore2);
    run_docker_command(container_before, cmdBefore);*/
        std::cout << "proceeding to wait for pktgen to startup" << std::endl;
        sleep(15);
        std::cout << "pktgen is woke" << std::endl;
        run_lst_docker_cmd(pktgen_container_name, pktgenArgs);
        std::cout << "=======================================\n";
    }
    
        int status = 0;
        wait(NULL);
    }
}

void reset(MachineConfigurator conf) {
	std::string del_ports_cmd = "sudo \"PATH=$PATH\" /home/ubuntu/ovs/utilities/ovs-docker del-ports ovs-br ";

    std::cout << "deleting config directories" << std::endl;
    std::string remove_config_folders = "rm -rf ../../*_config ../../forwarder.txt";
    system(remove_config_folders.c_str());
	// clean up merger_old and classifier
	/*system((del_ports_cmd + "classifier").c_str());
	system((del_ports_cmd + "merger_old").c_str());
	system("docker stop classifier merger_old; docker rm classifier merger_old");*/

	std::vector<RuntimeNode*> nodes = get_internal_nodes(conf);
	for (RuntimeNode* n : nodes) {
		// remove all veth pairs for this node
        std::string container_name = conf.get_config_dir(n->get_id());
		system((del_ports_cmd + container_name).c_str());
		// stop and remove the docker container for this node
		system(("docker stop " + container_name + "; docker rm " + container_name).c_str());
	}

	// delete the bridge
	system("sudo \"PATH=$PATH\" /home/ubuntu/ovs/utilities/ovs-vsctl del-br ovs-br");
    system("./../src/runtime/config/remveth");
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
                std::cout << "resetting after getting graph\n";
				needReset = true;
				break;
		}
	}

    if (argc < 2) {
        perror("not enough arguments: need to pass in ip and port of merger");
        exit(1);
    }

    merger_ip = argv[optind];
    std::cout << "Merger IP = " << merger_ip << std::endl;
	MachineConfigurator conf = get_machine_configurator(port);

    

	int machineId = conf.get_machine_id();
	Machine* mac = conf.get_machine_with_id(machineId);
	std::cout << mac->get_bridge_ip() << std::endl;

	if (needReset) {
		std::cout << "reset!" << std::endl;
		reset(conf);
	} 
	std::string remove_config_folders = "rm -rf ../../forwarder.txt";
    system(remove_config_folders.c_str());
	setup_nodes(conf);
	setup_bridge_ports(conf);
	make_flow_rules(conf);
    std::cout << "CONFIGURE FINISHED!!!!" << std::endl;
	return 0;
}

