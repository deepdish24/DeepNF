//
// Created by Victoria on 2018-04-04.
//

#include "MergerOperator.h"

typedef struct threadParams {
    MergerOperator* inst; // the MergerOperator instance to operate on
    int port; // port that the leaf node will send packets to
    int node_id; // the id of the runtime node for this thread
} THREAD_PARAMS;


MergerOperator::MergerOperator() {
    printf("MergerOperator::MergerOperator \n");

    // set up action table
    this->action_table = new ActionTable();
    this->merger_info = MergerInfo::get_dummy_merger_info();
}


void MergerOperator::run_node_thread() {
    printf("run_node_thread\n");
//    printf("initializing thread with port: %d, node_id: %d\n", tp->port, tp->node_id);
}

static void* MergerOperator::run_node_thread_wrapper(void *arg) {
    auto *tp = (THREAD_PARAMS*) arg;
    MergerOperator *this_mo = tp->inst;
    this_mo->run_node_thread();
}

/**
 * Setup MergerOperator to start listening and merging packets
 */
void MergerOperator::run() {
    printf("MergerOperator::run() \n");

    // send up one thread to handle each leaf node
    std::map<int, int> port_to_node_map = this->merger_info->get_port_to_node_map();

    pthread_t threads[port_to_node_map.size()];
    int thread_i = 0;
    for (std::map<int, int>::iterator it = port_to_node_map.begin(); it != port_to_node_map.end(); ++it) {
        THREAD_PARAMS* tp = (THREAD_PARAMS*) malloc(sizeof(THREAD_PARAMS));
        tp->inst = this;
        tp->port = it->first;
        tp->node_id = it->second;
        pthread_create(&threads[thread_i++], NULL, MergerOperator::run_node_thread_wrapper, tp);
    }

}