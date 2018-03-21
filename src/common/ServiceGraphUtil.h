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
    std::string machine_configurator_to_string(MachineConfigurator* mc);

    MachineConfigurator* string_to_machine_configurator(std::string msg);
}