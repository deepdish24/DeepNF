#pragma once

#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#include "ServiceGraph.h"


namespace service_graph_util
{
    /**
     * Takes in a string representing the protobuf-encoded ServiceGraph and returns the
     * corresponding ServiceGraph object.
     */
    ServiceGraph bytes_to_graph(const std::string bytes);

    /**
     * Takes in a ServiceGraph and encodes it into a protobuf string.
     */
    std::string graph_to_bytes(ServiceGraph graph);

}