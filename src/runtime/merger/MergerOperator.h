//
// Created by Victoria on 2018-04-04.
//

#ifndef DEEPNF_MERGEROPERATOR_H
#define DEEPNF_MERGEROPERATOR_H

#include <arpa/inet.h>
#include <errno.h>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <vector>
#include <pthread.h>

#include "ActionTable.h"
#include "MergerInfo.h"

class MergerOperator {

public:

    MergerOperator();

    /**
     * Setup MergerOperator to start listening and merging packets
     */
    void run();

    /**
     * Listens on the given port for packets from the given runtime_id, merging as necessary
     *
     * @param port          The port to listen for packets from runtime_id
     * @param node_id    The id of the runtime node leaf
     */
    void run_node_thread(int port, int node_id);
    static void* run_node_thread_wrapper(void *arg);

private:

    // contains information about virtual network interfaces, conflicting NF pairs, etc.
    MergerInfo* merger_info;

    // contains information about the NF action table
    ActionTable* action_table;
};



#endif //DEEPNF_MERGEROPERATOR_H
