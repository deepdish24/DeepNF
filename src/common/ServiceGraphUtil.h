#pragma once

#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "MachineConfigurator.h"


namespace service_graph_util
{
    /**
     * Serializes the given MachineConfigurator into a string
     *
     * @param mc		The MachineConfigurator* to convert
     * @return  A string-serialized version of the MachineConfigurator
     */
    std::string machine_configurator_to_string(MachineConfigurator* mc);

    /**
     * Deserializes the given string into a MachineConfigurator
     *
     * @param msg       The string to deserialize
     * @return A pointer to the deserialized MachineConfigurator object
     */
    MachineConfigurator* string_to_machine_configurator(std::string msg);
}