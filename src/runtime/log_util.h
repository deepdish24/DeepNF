//
// Created by Victoria on 2018-04-09.
//

#ifndef DEEPNF_LOG_UTIL_H
#define DEEPNF_LOG_UTIL_H

#include "log_util.h"
#include "packet.h"

#include <fstream>
#include <sys/time.h>

namespace log_util
{
    /**
     * Prints a status update to the given log
     *
     * @param log       Ofstream representing the log file to write to
     * @param p         Reference to the packet to be logged
     * @param name      The name of the NF/program that is logging
     * @param action    A status update indicating the action performed on the packet
     */
    void log_nf(std::ofstream log, packet* p, std::string name, std::string action);
}

#endif //DEEPNF_LOG_UTIL_H