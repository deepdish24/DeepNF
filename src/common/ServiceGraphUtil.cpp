
#include "src/proto/Machine.pb.h"
#include "ServiceGraphUtil.h"

using namespace google::protobuf::io;

namespace service_graph_util
{

    // Functions in this anonymous namespace can only be accessed by functions in service_graph_util
    namespace {
        MachineConfigurator* machine_configurator_from_message(MachineConfiguratorMsg* message) {
            int machine_id = message->machine_id();

            google::protobuf::Map<google::protobuf::uint64, MachineMsg> machine_map =
                    message->machine_map();

            MachineMsg msg = machine_map[machine_id];


            return NULL;
        }

//        Machine* machine_from_message(MachineMsg* message) {
//            Machine* m = new Machine(message->id());
//            m->set_ip(message->ip());
//            m->set_bridge_ip(message->bridge_ip());
//            for (const int node : message->nodes()) {
//                m->add_node_id(node);
//            }
//
//            return m;
//        }
    }

//    std::string machine_configurator_to_string(MachineConfigurator* mc) {
//
//        return NULL;
//    }

    MachineConfigurator* string_to_machine_configurator(std::string msg) {
        CodedInputStream* cis = new CodedInputStream(reinterpret_cast<const uint8_t*>(msg.data()), msg.size());

        // read size of message
        uint32_t size;
        cis->ReadLittleEndian32(&size);

        // construct MachineConfigurationMsg
        MachineConfiguratorMsg message;
        CodedInputStream::Limit limit = cis->PushLimit(size);
        message.ParseFromCodedStream(cis);
        cis->PopLimit(limit);

        MachineConfigurator* mc = machine_configurator_from_message(&message);

        delete cis;
        return mc;
    }
}