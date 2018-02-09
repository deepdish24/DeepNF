
#include "ServiceGraphUtil.h"
//#include "src/protobuf/Node.proto"

//using namespace google::protobuf::io;

namespace service_graph_util
{
    /**
     * Takes in a string representing the protobuf-encoded ServiceGraph and returns the
     * corresponding ServiceGraph object.
     */
    ServiceGraph bytes_to_graph(const std::string bytes) {
//        CodedInputStream* codedInput = new CodedInputStream(
//            reinterpret_cast<const uint8_t*>(bytes.data()), bytes.size()
//        );
//
//        uint32_t size;
//        while (codedInput->ReadLittleEndian32(&size)) {
//
//            BackendRequest message;
//            CodedInputStream::Limit limit = codedInput->PushLimit(size);
//            message.ParseFromCodedStream(codedInput);
//            codedInput->PopLimit(limit);
//
//            server->handleRequest(&message, connection);
//        }
    }

    /**
     * Takes in a ServiceGraph and encodes it into a protobuf string.
     */
    std::string graph_to_bytes(ServiceGraph node) {

    }

}