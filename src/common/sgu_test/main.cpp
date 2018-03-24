#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "../MachineConfigurator.h"
#include "../ServiceGraphUtil.h"

#define UNUSED(expr) (void)(expr); // remove unused warning

int main(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);

    printf("Test\n");

    Machine* m = new Machine(10);
    m->set_bridge_ip("bridge_ip");
    m->set_ip("127.0.0.1");
    m->add_node_id(0);
    m->add_node_id(1);
    m->add_node_id(10000);

    Machine* m1000 = new Machine(1000);
    m1000->set_bridge_ip("bridge_ip1000");
    m1000->set_ip("127.0.0.1.1000");
    m1000->add_node_id(6);
    m1000->add_node_id(6);
    m1000->add_node_id(6);

    RuntimeNode* rn = new RuntimeNode(0, NF::snort);
    rn->inport = 7;
    rn->outport = 7;
    rn->machine_id = 0;
    rn->ip = "15.15.15.15";

    RuntimeNode* rn5 = new RuntimeNode(5, NF::snort);
    rn5->inport = 5;
    rn5->outport = 5;
    rn5->machine_id = 5;
    rn5->ip = "5.5.5.5";

    MachineConfigurator* mc = new MachineConfigurator(m);
    mc->add_machine(m1000);
    mc->add_node(rn);
    mc->add_node(rn5);
    std::string serialized = service_graph_util::machine_configurator_to_string(mc);

    MachineConfigurator* new_mc = service_graph_util::string_to_machine_configurator(serialized);
    printf("new_mc->get_machine_id(): %d\n", new_mc->get_machine_id());

    std::map<int, RuntimeNode*> node_map = mc->get_node_map();
    RuntimeNode* rn_0 = node_map.at(0);
    printf("rn_0->inport: %d\n", rn_0->inport);
    printf("rn_0->outport: %d\n", rn_0->outport);
    printf("rn_0->machine_id: %d\n", rn_0->machine_id);
    printf("rn_0->ip %s\n\n", rn_0->ip.c_str());

    RuntimeNode* rn_5 = node_map.at(5);
    printf("rn_5->inport: %d\n", rn_5->inport);
    printf("rn_5->outport: %d\n", rn_5->outport);
    printf("rn_5->machine_id: %d\n", rn_5->machine_id);
    printf("rn_5->ip %s\n\n", rn_5->ip.c_str());

    std::map<int, Machine*> machine_map = new_mc->get_machine_map();
    Machine* machine_10 = machine_map.at(10);
    printf("machine_10->get_bridge_ip(): %s\n", machine_10->get_bridge_ip().c_str());
    printf("machine_10->get_ip(): %s\n", machine_10->get_ip().c_str());
    printf("nodes:\n");
    for (int node : machine_10->get_node_ids()) {
        printf("%d, ", node);
    }
    printf("\n\n");

    Machine* machine_1000 = machine_map.at(1000);
    printf("machine_1000->get_bridge_ip(): %s\n", machine_1000->get_bridge_ip().c_str());
    printf("machine_1000->get_ip(): %s\n", machine_1000->get_ip().c_str());
    printf("nodes:\n");
    for (int node : machine_1000->get_node_ids()) {
        printf("%d, ", node);
    }
    printf("\n\n");
}