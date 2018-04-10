/**
* @author deepans
* Orchestrator implementation
**/

#include "Orchestrator.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ios>
#include <tuple>
#include <typeinfo>
#include "../common/RuntimeNode.h"
#include "../common/MachineConfigurator.h"
#include "../common/ServiceGraphUtil.h"
#include "../common/Field.h"
#include "../runtime/merger/ConflictItem.h"

Orchestrator::Orchestrator(std::string filepath, std::string action_file_path) {
    std::ifstream fileInput(filepath);
    std::ifstream actionTableInput(action_file_path);

    fileInput >> userInput;
    actionTableInput >> actionTable;

    std::vector<std::string> functions = userInput["functions"];
    std::vector<std::string> ips_tmp = userInput["ips"];
    std::vector<int> ports_tmp = userInput["ports"];
    dest_ip = userInput["receiver_ip"];
    dest_port = userInput["receiver_port"];
    ips = ips_tmp;
    ports = ports_tmp;

    /* setting up sockaddr data structures to connect 
     to ip + port of all available machines */
   /* for (int i = 0; i < (int) ips.size(); i++) {
        std::string ip = ips[i];
        int port_num = ports[i];
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in servaddr;
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port_num);
        inet_pton(AF_INET, ip.c_str(), &(servaddr.sin_addr));
        connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
        sockets[ip] = sockfd;
    }*/

    std::vector<std::vector<std::string>> positionals = userInput["positional"];
    std::vector<std::vector<std::string>> dependencies = userInput["orderDependencies"];
    std::vector<std::vector<std::string>> priorities = userInput["priorities"];

    bool hasPositionals = true;

    // Check size of the positional to make sure it is positive and at most 2
    if (positionals.size() < 1 || positionals.size() > 2) {
        hasPositionals = false;
        //throw std::invalid_argument("Incorrect number of positional NFs detected");
    }

    if (hasPositionals && positionals.size() == 2 && positionals[0][1].compare(positionals[1][1]) == 0) {
        hasPositionals = false;
        //throw std::invalid_argument("Two NFS assigned to same position");
    }

    if ((int) dependencies.size() != 0) {
        parseOrderDependencies(dependencies);
    }

    if ((int) priorities.size() != 0) {
        parsePriorityDependencies(priorities);
    }

     /* SERVICE GRAPH CONSTRUCTION */
    for (int i = 0; i < (int) functions.size(); i++) {
        //creating on stack
        //ServiceGraphNode n(functions[i]);
        ServiceGraphNode *s = new ServiceGraphNode(functions[i]);
        //std::cout << "Function " << functions[i] << " node created with address: " << s << std::endl;
        func_to_nodes[functions[i]] = s;
    }

    std::set<ServiceGraphNode*> orderTreeNodes = {};
    std::set<ServiceGraphNode*> priorityNodes = {};

    // Create Forest describing order dependencies
    for (int i = 0; i < (int) parsedOrder.size(); i++) {
        std::string nf1 = std::get<0>(parsedOrder[i]);
        std::string nf2 = std::get<1>(parsedOrder[i]);
        ServiceGraphNode *node1 = func_to_nodes[nf1];
        ServiceGraphNode *node2 = func_to_nodes[nf2];
        (*node1).add_neighbor(node2);
        (*node2).add_parent(node1);
        orderTreeNodes.insert(node1);
        orderTreeNodes.insert(node2);
    }

    for (int i = 0; i < (int) parsedPriorities.size(); i++) {
        std::string nf1 = std::get<0>(parsedPriorities[i]);
        std::string nf2 = std::get<1>(parsedPriorities[i]);
        ServiceGraphNode *node1 = func_to_nodes[nf1];
        ServiceGraphNode *node2 = func_to_nodes[nf2];
        priorityNodes.insert(node1);
        priorityNodes.insert(node2);
    }

    std::set<ServiceGraphNode*> rootNodes = {};
    std::set<ServiceGraphNode*> leafNodes = {};

    // loop finds all root nodes of each tree
    for (auto node : orderTreeNodes) {
        if (node->isRoot()) {
            rootNodes.insert(node);
        }
    }

    // loop proceeds through each tree and checks if leaves can be run in parallel
    std::cout << "===================parsing tree nodes=======================" << std::endl;
    for (auto node : rootNodes) {
        std::set<ServiceGraphNode*> leavesForRoot = {};
        std::cout << "root node: " << node->nf << std::endl;
        std::cout << "num neighbors for root: " << node->neighbors.size() << std::endl;
        std::set<ServiceGraphNode*> neighbors = node->neighbors;
        for (auto neighbor : neighbors) {
            std::cout << "neighbor: " << neighbor->nf << std::endl;
            std::cout << "is neighbor leaf: " << neighbor->isLeaf() << std::endl;
        }
        findAllLeaves(node, leavesForRoot);
        checkLevelParallelizability(leavesForRoot);
        for (auto leaf : leavesForRoot) {
            leafNodes.insert(leaf);
        }
    }
    std::cout << "===================done parsing tree nodes=======================" << std::endl;

    //check all pairwise parallelizability of priorityNF's
    checkLevelParallelizability(priorityNodes);

    ServiceGraphNode* first = NULL;
    ServiceGraphNode* last = NULL;

    for (int i = 0; i < (int) positionals.size(); i++) {
        std::vector<std::string> pos = positionals[i];
        if (pos[1].compare("first") == 0) {
            first = func_to_nodes[pos[0]];
        }
        if (pos[1].compare("last") == 0) {
            last = func_to_nodes[pos[0]];
        }
    }

    if (hasPositionals) {
        // attaching root nodes to first node in chain
        for (auto node : rootNodes) {
            first->add_neighbor(node);
            node->add_parent(first);
        }

        // attaching all free floating nodes to 
        // both first and last nodes
        for (auto node : priorityNodes) {
            first->add_neighbor(node);
            node->add_parent(first);
            if (last != NULL) {
                node->add_neighbor(last);
                last->add_parent(node);
            }
        }

        // attaching all leaf nodes to last node
        if (last != NULL) {
            for (auto node : leafNodes) {
                node->add_neighbor(last);
                last->add_parent(node);
            }
        }
    }

    /* NODE PARTITIONING */
    round_robin_partitioning(ips, functions);
    //single_node_partitioning(ips, functions);


    /* MACHINE CONFIGURATION */
    std::unordered_map<std::string, Machine*> ip_to_machines;

    // id of runtime node -> runtime node (node_map)
    std::unordered_map<int, RuntimeNode*> idToRuntimeNode = {};


    std::unordered_map<std::string, int> func_to_inx = {};

    //configure each machine
    for (int i = 0; i < (int) ips.size(); i++) {
        Machine *m = new Machine(i);
        m->set_ip(ips[i]);
        m->set_bridge_ip("173.16.1.1");
        ip_to_machines[ips[i]] = m;
    }

    //configuring for each node
    for (int i = 0; i < (int) functions.size(); i++) {
        NF nf = stringToNF(functions[i]);
        RuntimeNode *rnode = new RuntimeNode(i, nf);
        idToRuntimeNode[i] = rnode;
        func_to_inx[functions[i]] = i;
        Machine *machine = ip_to_machines[func_to_ip[functions[i]]];
        machine->add_node_id(rnode->get_id());
    }

    for (int i = 0; i < (int) functions.size(); i++) {
        ServiceGraphNode *node = func_to_nodes[functions[i]];
        RuntimeNode *rnode = idToRuntimeNode[i];
        std::set<ServiceGraphNode*> neighbors = node->neighbors;
        for (auto neighbor : neighbors) {
            int neighborInx = func_to_inx[neighbor->nf];
            rnode->add_neighbor(neighborInx);
        }
    }

    // creating machine configurator objects and serializing them to send somewhere?
    for (int i = 0; i < (int) ips.size(); i++) {
        Machine* m = ip_to_machines[ips[i]];
        MachineConfigurator* mc = new MachineConfigurator(m);
        //mc->get_nodes_for_machine(mc->get_machine_id());
        for (int j = 0; j  < (int) ips.size(); j++) {
            if (i != j) {
                mc->add_machine(ip_to_machines[ips[j]]);
            }
        }
        
        for (auto it = idToRuntimeNode.begin(); it != idToRuntimeNode.end(); it++) {
            std::cout << "wtf node: " << it->second->get_id() << std::endl;
            mc->add_node(it->second);
        }

        std::vector<RuntimeNode*> nodes = mc->get_nodes_for_machine(0);
        std::cout << "Number of nodes before serialization: " << nodes.size() << std::endl;

        /*std::vector<int> node_ids = m->get_node_ids();
        for (int id : node_ids) {
            mc->add_node(idToRuntimeNode[id]);
        }*/
        std::string serializedConfig = service_graph_util::machine_configurator_to_string(mc);
        MachineConfigurator *mc2 = service_graph_util::string_to_machine_configurator(serializedConfig);
        std::vector<RuntimeNode*> newNodes = mc2->get_nodes_for_machine(0);
        std::cout << "Number of nodes after serialization: " << newNodes.size() << std::endl;
        ip_to_mc[ips[i]] = serializedConfig;
    }

    //port to node id
    std::map<int, int> port_to_node_map;
    int port;
    for (int i = 0; i < (int) functions.size(); i++) {
        RuntimeNode *rnode = idToRuntimeNode[i];
        if (rnode->get_neighbors().size() == 0) {
            port = 8000 + rnode->get_id() + 1;
            port_to_node_map[port] = rnode->get_id();
        }
    }

    write_graph_format(idToRuntimeNode, functions);

    std::vector<ConflictItem*> conflicts_list = create_conflicts_list(func_to_inx);

    //SETUP MERGER
    std::map<int, RuntimeNode*> node_map;
    for (auto it = idToRuntimeNode.begin(); it != idToRuntimeNode.end(); ++it) {
        node_map.insert(std::make_pair(it->first, it->second));
    }
    MergerInfo *merger_info = new MergerInfo(port_to_node_map, conflicts_list, node_map, dest_ip, dest_port);

    this->merger_operator = new MergerOperator(merger_info);
}

void Orchestrator::run_merger()
{
    merger_operator->run();
}

void Orchestrator::round_robin_partitioning(std::vector<std::string> &ips, std::vector<std::string> &functions) {
    int currInx = 0;
    for (int i = 0; i < (int) functions.size(); i++) {
        func_to_ip[functions[i]] = ips[currInx];
        currInx = (currInx + 1) % (int) ips.size();
    }
}

void Orchestrator::single_node_partitioning(std::vector<std::string> &ips, 
    std::vector<std::string> &functions) {
    std::string ip_one = ips[0];
    for (int i = 0; i < (int) functions.size(); i++) {
        func_to_ip[functions[i]] = ip_one;
    }
}

/* Function to parse priority dependencies and produce conflicting actions */
void Orchestrator::parsePriorityDependencies(std::vector<std::vector<std::string>> priorities) {
    for (int i = 0; i < (int) priorities.size(); i++) {
        std::vector<Field> conflictingActions = {};
        bool parallel = isParallelizable(priorities[i], actionTable, conflictingActions);
        std::string f1 = priorities[i][0];
        std::string f2 = priorities[i][1];
        if (parallel) {
            parsedPriorities.push_back(std::make_tuple(f1, f2));
            if ((int) conflictingActions.size() > 0) {
                pair_to_conflicts[f1][f2] = conflictingActions;
            }
        } else {
            perror("priority constraint not parallelizable");
        }
    }
}

/* Function that parses order dependencies to determine which can be run in parallel.
   Adds those that can be parallelized as priority dependencies. Conflicting actions
   noted. */
void Orchestrator::parseOrderDependencies(std::vector<std::vector<std::string>> dependencies) {
    for (int i = 0; i < (int) dependencies.size(); i++) {
        std::vector<Field> conflictingActions = {};
        bool parallel = isParallelizable(dependencies[i], actionTable, conflictingActions);
        std::string f1 = dependencies[i][0];
        std::string f2 = dependencies[i][1];
        if (parallel) {
            std::cout << "CAN BE RUN IN PARALLEL\n";
            parsedPriorities.push_back(std::make_tuple(f1, f2));
            if ((int) conflictingActions.size() > 0) {
                pair_to_conflicts[f1][f2] = conflictingActions;
            }
        } else {
            std::cout << "CANNOT BE RUN IN PARALLEL\n";
            parsedOrder.push_back(std::make_tuple(f1, f2));
        }
    }
}

NF Orchestrator::stringToNF(std::string function) {
    NF nf = dnf_firewall;
    if (function.compare("dnf_firewall") == 0) {
        nf = dnf_firewall;
    } if (function.compare("dnf_loadbalancer") == 0) {
        nf = dnf_loadbalancer;
    } if (function.compare("proxy") == 0) {
        nf = proxy;
    } if (function.compare("compressor") == 0) {
        nf = compressor;
    } if (function.compare("pktgen") == 0) {
        nf = pktgen;
    } else {
        perror("stringToNf called on unknown function");
    }
    return nf;
}

Action stringToAction(std::string action) {
    Action a;
    if (action.compare("R") == 0) {
        a = READ;
    } else {
        a = WRITE;
    }
    return a;
}

std::string Orchestrator::fieldToString(Field a) {
    switch(a) {
        case SIP: 
            return std::string("SIP");
        case DIP:
            return std::string("DIP");
        case SPORT:
            return std::string("SPORT");
        case DPORT:
            return std::string("DPORT");
        default:
            return std::string("PAYLOAD");
    }
}

Field stringToField(std::string field) {
    Field f;
    if (field.compare("SIP") == 0) {
        f = SIP;
    } else if (field.compare("DIP") == 0) {
        f = DIP;
    } else if (field.compare("SPORT") == 0) {
        f = SPORT;
    } else if (field.compare("DPORT") == 0) {
        f = DPORT;
    } else {
        f = PAYLOAD;
    }
    return f;
}

/* function checks if nf1 before nf2 can be parallelized */
bool Orchestrator::isParallelizable(std::vector<std::string> orderDep, json actionTable,
    std::vector<Field> &conflictingActions) {
    //function gets actions for each function
    std::map<std::string, std::string> nf1 = actionTable[orderDep[0]];
    std::map<std::string, std::string> nf2 = actionTable[orderDep[1]];

    //function iterates through all packet locations for nf1
    for (auto it = nf1.begin(); it != nf1.end(); ++it) {
        std::string packetLocation = it->first;
        std::string val = it->second;
        std::string val2 = nf2[packetLocation];
        /*std::cout << "nf1 packet location: " << packetLocation << std::endl;
        std::cout << "nf1 val: " << val << std::endl;
        std::cout << "nf2 val: " << val2 << std::endl;*/

        // if nf1 drops/adds/removes then it cannot be run in parallel with nf2
        if ((packetLocation.compare("Drop") == 0 || packetLocation.compare("Add/Rm") == 0) && val.compare("T") == 0) {
            return false;
        } else if (val.compare("null") != 0 && val2.compare("null") != 0) {
            Action a1 = stringToAction(it->second);
            Action a2 = stringToAction(nf2[packetLocation]);
            if ((a1 == READ || a1 == WRITE) && a2 == WRITE) {
                //std::cout << "adding to conflicting actions" << std::endl;
                std::string loc(packetLocation);
                conflictingActions.push_back(stringToField(packetLocation));
            } else if (a1 == WRITE) {
                return false;
            }
        }
    }
    return true;
}


/* Function takes in root of tree and performs a depth-first search to find all leaves in tree.
   Leaves are added to set passed in */
void Orchestrator::findAllLeaves(ServiceGraphNode* root, std::set<ServiceGraphNode*> &leaves) {
    if (root->isLeaf()) {
        leaves.insert(root);
    } else {
        std::set<ServiceGraphNode*> neighbors = root->neighbors;
        for (auto neighbor : neighbors) {
            findAllLeaves(neighbor, leaves);
        }
    }
}

/* Function takes in set of nodes and checks to see if all are mutually parallelizable */
void Orchestrator::checkLevelParallelizability(std::set<ServiceGraphNode*> nodes) {
    std::unordered_map<std::string, std::unordered_map<std::string, bool>> visited_pair = {};
    for (auto node1 : nodes) {
        for (auto node2 : nodes) {
            if (node1 != node2) {
                std::string nf1 = node1->nf;
                std::string nf2 = node2->nf;
                std::vector<std::string> pair = {};
                pair.push_back(nf1);
                pair.push_back(nf2);
                std::vector<Field> conflictingActions = {};
                bool parallelizable = isParallelizable(pair, actionTable, conflictingActions);
                visited_pair[nf1][nf2] = parallelizable;
                if (!parallelizable) {
                    if (visited_pair.find(nf2) != visited_pair.end() && !visited_pair[nf2][nf1]) {
                        perror("nodes specified are not parallelizable");
                    }
                } else {
                    pair_to_conflicts[nf1][nf2] = conflictingActions;
                }
            }
        }
    }
}

std::vector<ConflictItem*> Orchestrator::create_conflicts_list(std::unordered_map<std::string, int> func_to_inx) {
    //std::vector<ConflictPairInfo> conflictPairs = {};
    // auto arr = json::array();
    std::vector<ConflictItem*> conflicts_list;
    for (auto it = pair_to_conflicts.begin(); it != pair_to_conflicts.end(); ++it) {
        std::string major = it->first;
        std::unordered_map<std::string, std::vector<Field>> map = it->second;
        ServiceGraphNode* node = func_to_nodes[major];
        ServiceGraphNode* parentNode = node->get_parent();
        int parentId = -1;
        if (parentNode != NULL) {
            std::string parent = parentNode->nf;
            parentId = func_to_inx[parent];
        }
        for (auto nextIt = map.begin(); nextIt != map.end(); ++nextIt) {
            // auto object = json::object();

            //auto fieldArr = json::array();
            std::string minor = nextIt->first;
            //std::vector<Field> fields = nextIt->second;
            // object["major"] = func_to_inx[major];
            // object["minor"] = func_to_inx[minor];
            // object["parent"] = parentId;
            ConflictItem *item = new ConflictItem(func_to_inx[major], func_to_inx[minor], parentId);
            conflicts_list.push_back(item);
            /*for (Field f : fields) {
                fieldArr.push_back(fieldToString(f));
            }*/
            //object["conflicts"] = fieldArr;
            // arr.push_back(object);
        }
    }

    return conflicts_list;
    // std::ofstream out("../../../src/common/conflict_pairs.json");
    // out << arr;
}

void Orchestrator::setup_containers() {
    std::cout << "proceeding to setup containers" << std::endl;
    for (int i = 0; i < (int) ips.size(); i++) {
        std::cout << "current ip: " << ips[i] << std::endl;
        std::cout << "current port: " << ports[i] << std::endl;
        struct sockaddr_in servaddr;
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        bzero(&servaddr, sizeof(servaddr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(ports[i]);

        inet_pton(AF_INET, ips[i].c_str(), &(servaddr.sin_addr));
        connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

        std::string serializedConfig = ip_to_mc[ips[i]];
        char conf_str[serializedConfig.size()+1];
        serializedConfig.copy(conf_str, serializedConfig.size(), 0);

        /*std::string config(conf_str, sizeof(conf_str));
        MachineConfigurator *mc = service_graph_util::string_to_machine_configurator(config);
        MachineConfigurator conf = *(mc);
        int machineId = conf.get_machine_id();
        Machine* mac = conf.get_machine_with_id(machineId);
        std::cout << "TEST SERIALIZATION: " << mac->get_bridge_ip() << std::endl;
        std::cout << "=======================" << std::endl;*/

        char recvline[100];
        std::string ack = "ACK";
        bzero(recvline, 100);
        while (true) {
            std::cout << "writing serialized data" << std::endl;
            write(sockfd, conf_str, sizeof(conf_str));
            std::cout << "proceeding to read from connection" << std::endl;
            read(sockfd, recvline, 100);
            std::cout << "RECEIVED: " << recvline << std::endl;
            if (strcmp(recvline, ack.c_str()) == 0) {
                break;
            }
        }
    }
}

void Orchestrator::write_graph_format(std::unordered_map<int, RuntimeNode*> idToRuntimeNode, 
    std::vector<std::string> functions) {
    std::string output_dir = "../inputs/";
    auto nodes = json::array();
    auto edges = json::array();
    for (auto it = idToRuntimeNode.begin(); it != idToRuntimeNode.end(); ++it) {
        int id = it->first;
        RuntimeNode* node = it->second;
        auto node_obj = json::object();

        node_obj["id"] = id;
        node_obj["label"] = functions[id];
        node_obj["group"] = func_to_ip[functions[id]];
        nodes.push_back(node_obj);
        for (int id2 : node->get_neighbors()) {
            auto edge_obj = json::object();
            edge_obj["from"] = id;
            edge_obj["to"] = id2;
            edge_obj["arrows"] = "to";
            auto colorObj = json::object();
            colorObj["inherit"] = "from";
            edge_obj["color"] = colorObj;
            edges.push_back(edge_obj);
        }
    }

    std::ofstream node_out(output_dir + "nodes.txt");
    std::ofstream edge_out(output_dir + "edges.txt");
    node_out << nodes;
    edge_out << edges;
    node_out.close();
    edge_out.close();
    std::cout << "GRAPH SERIALIZED TO TXT FILE" << std::endl;
}

// Given a service graph node, determine if it is a root node
bool Orchestrator::isLeaf(ServiceGraphNode *n) {
    return !(*n).neighbors.empty();
}