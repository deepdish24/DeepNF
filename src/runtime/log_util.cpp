//
// Created by Victoria on 2018-04-09.
//

#include "log_util.h"

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
    void log_nf(std::ofstream log, packet* p, std::string name, std::string action) {

        int count = 0;
        struct timeval tv;
        time_t nowtime;
        struct tm *nowtm;
        char tmbuf[64], buf[64];

        // print time into log
        gettimeofday(&tv, NULL);
        nowtime = tv.tv_sec;
        nowtm = localtime(&nowtime);
        strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
        snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, tv.tv_usec);
        printf("Printing into log\n");
        log << tmbuf << "." << tv.tv_usec << ",";

        // print packet id into log
        log << p->get_pkt_id() << ",";

        // print NF name and msg into log
        log << name << "," << action << std::endl;
    }
}

