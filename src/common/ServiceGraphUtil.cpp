
#include "src/proto/Machine.pb.h"
#include "ServiceGraphUtil.h"

using namespace google::protobuf::io;

namespace service_graph_util
{

    // functions defined in anonymous namespaces cannot be called from other classes (kinda like private functions)
    namespace {

        /**
         * Initializes a Machine object that is equivalent to the MachineMsg* input and returns a pointer to it.
         *
         * @param message		The MachineMsg* to convert
         * @return  A pointer to a new Machine object equivalent to message
         */
        Machine* machine_from_message(MachineMsg* message) {
            Machine* m = new Machine(message->id());
            m->set_ip(message->ip());
            m->set_bridge_ip(message->bridge_ip());
            for (const int node : message->nodes()) {
                m->add_node_id(node);
            }

            return m;
        }

        /**
         * Takes in a pointer to a Machine object, then creates and returns a MachineMsg representation of that Machine
         *
         * @param m		The Machine object to convert
         * @return  A MachineMsg representation of m
         */
        MachineMsg message_from_machine(Machine* m) {
            printf("message_from_machine\n");
            MachineMsg message;
            message.set_id(m->get_id());
            message.set_ip(m->get_ip());
            message.set_bridge_ip(m->get_bridge_ip());

            for (int node : m->get_node_ids()) {
                message.add_nodes(node);
            }

            return message;
        }


        /**
          * Initializes a RuntimeNode object that is equivalent to the RuntimeNodeMsg* input and returns
          * a pointer to it.
          *
          * @param message		The RuntimeNodeMsg* to convert
          * @return  A pointer to a new RuntimeNode object equivalent to message
          */
        RuntimeNode* runtime_node_from_message(RuntimeNodeMsg* message) {
            NF nf;

            switch(message->nf()) {
                case RuntimeNodeMsg::DNF_FIREWALL:
                    nf = dnf_firewall;
                    break;

                case RuntimeNodeMsg::DNF_LOADBALANCER:
                    nf = dnf_loadbalancer;
                    break;

                default:
                    throw std::invalid_argument("Message has unknown NF type");
            }

            RuntimeNode* rn = new RuntimeNode(message->id(), nf);
            rn->inport = message->inport();
            rn->outport = message->outport();
            rn->machine_id = message->machine_id();
            rn->ip = message->ip();
            for (const int neighbor : message->neighbors()) {
                rn->add_neighbor(neighbor);
            }

            return rn;

            return NULL;
        }


        /**
          * Takes in a pointer to a RuntimeNode object, then creates and returns a RuntimeNodeMsg representation of that Machine
          *
          * @param rn		The RuntimeNode object to convert
          * @return  A RuntimeNodeMsg representation of m
          */
        RuntimeNodeMsg message_from_runtime_node(RuntimeNode* rn) {
            RuntimeNodeMsg message;

            switch(rn->get_nf()) {
                case dnf_firewall:
                    message.set_nf(RuntimeNodeMsg::DNF_FIREWALL);
                    break;

                case dnf_loadbalancer:
                    message.set_nf(RuntimeNodeMsg::DNF_LOADBALANCER);
                    break;

                default:
                    throw std::invalid_argument("RuntimeNode has unknown NF type");
            }

            message.set_inport(rn->inport);
            message.set_outport(rn->outport);
            message.set_machine_id(rn->machine_id);
            message.set_ip(rn->ip);


            for (const int neighbor : rn->get_neighbors()) {
                message.add_neighbors(neighbor);
            }

            return message;
        }


        /**
         * Initializes a MachineConfigurator object that is equivalent to the MachineConfiguratorMsg* input and returns
         * a pointer to it.
         *
         * @param message		The MachineConfiguratorMsg* to convert
         * @return  A pointer to a new MachineConfigurator object equivalent to message
         */
        MachineConfigurator* machine_configurator_from_message(MachineConfiguratorMsg* message) {
            int machine_id = message->machine_id();
//            Machine* m = new Machine(machine_id); // remove later

            google::protobuf::Map<google::protobuf::uint64, MachineMsg> machine_map =
                    message->machine_map();
            Machine* m = machine_from_message(&machine_map[machine_id]);

            MachineConfigurator* mc = new MachineConfigurator(m);

            // add machines to mc
            for (auto val : machine_map) {
                int key = val.first;
                if (key != machine_id) {
                    MachineMsg msg = val.second;
                    mc->add_machine(machine_from_message(&msg));
                }
            }

            // add runtime nodes to mc
            for (auto val : message->node_map()) {
                RuntimeNodeMsg msg = val.second;
                mc->add_node(runtime_node_from_message(&msg));
            }

            return mc;
        }


        /**
        * Takes in a pointer to a MachineConfigurator object, then creates and returns a MachineConfiguratorMsg
        * representation of that MachineConfigurator
        *
        * @param mc		The MachineConfigurtor object to convert
        * @return  A MachineConfiguratorMsg representation of m
        */
        MachineConfiguratorMsg message_from_machine_configurator(MachineConfigurator *mc) {
            printf("message_from_machine_configurator\n");

            MachineConfiguratorMsg message;

            message.set_machine_id(mc->get_machine_id());
            printf(" message->set_machine_id(mc->get_machine_id());\n");

            // add machines to message
            google::protobuf::Map<google::protobuf::uint64, MachineMsg>* machine_msg_map =
                    message.mutable_machine_map();
            for (auto val : mc->get_machine_map()) {
                (*machine_msg_map)[val.first] = message_from_machine(val.second);
            }

            // add runtime nodes to message
            google::protobuf::Map<google::protobuf::uint64, RuntimeNodeMsg>* node_msg_map =
                    message.mutable_node_map();
            for (auto val : mc->get_node_map()) {
                printf("Adding runtime node: %d", val.first);
                (*node_msg_map)[val.first] = message_from_runtime_node(val.second);
            }

            google::protobuf::Map<google::protobuf::uint64, RuntimeNodeMsg> node_msg_map2 = message.node_map();
            for (auto it = node_msg_map->begin(); it != node_msg_map->end(); ++it) {
                printf("Reading node_map, %d\n", it->first);
            }

            return message;
        }
    }


    /**
     * Serializes the given MachineConfigurator into a string
     *
     * @param mc		The MachineConfigurator* to convert
     * @return  A string-serialized version of the MachineConfigurator
     */
    std::string machine_configurator_to_string(MachineConfigurator* mc) {
        printf("machine_configurator_to_string\n");
        MachineConfiguratorMsg mc_msg = message_from_machine_configurator(mc);

        std::string msg;
        mc_msg.SerializeToString(&msg);

        return msg;
    }


    /**
     * Deserializes the given string into a MachineConfigurator
     *
     * @param msg       The string to deserialize
     * @return A pointer to the deserialized MachineConfigurator object
     */
    MachineConfigurator* string_to_machine_configurator(std::string msg) {
        MachineConfiguratorMsg message;
        message.ParseFromString(msg);
        MachineConfigurator* mc = machine_configurator_from_message(&message);

        return mc;
    }
}