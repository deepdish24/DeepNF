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


void* MergerOperator::run_node_thread_wrapper(void *arg) {
    auto *tp = (THREAD_PARAMS*) arg;
    MergerOperator *this_mo = tp->inst;
    this_mo->run_node_thread(tp->port, tp->node_id);

    return nullptr;
}


/**
 * Listens on the given port for packets from the given runtime_id, merging as necessary
 *
 * @param port          The port to listen for packets from runtime_id
 * @param node_id    The id of the runtime node leaf
 */
void MergerOperator::run_node_thread(int port, int node_id) {
    printf("Calling run_node_thread with port: %d, node_id: %d\n", port, node_id);

//    // open a port and start listening for packets
//    int portno = std::stoi(argv[2]);
//    printf("Opening receiver to listening on port %d\n", portno);

    // opens a datagram socket and returns the fd or -1 */
    int sockfd = open_socket();
    if (sockfd < 0) {
        fprintf(stderr, "Cannot open socket: %s", strerror(errno));
        exit(-1);
    }
    printf("opened socket\n");

    // binds socket with given fd to given port */
    bind_socket(sockfd, portno);
    printf("binded socket\n");

    while (true) {
        printf("\nlistening for data...\n");
        sockdata *pkt_data = receive_data(sockfd);
        packet* p = packet_from_data(pkt_data);

        printf("Echo: [%s] (%d bytes)\n", p->data, p->data_size);
        free(p);
        free(pkt_data);
    }
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
    for (auto it = port_to_node_map.begin(); it != port_to_node_map.end(); ++it) {
        auto * tp = (THREAD_PARAMS*) malloc(sizeof(THREAD_PARAMS));
        tp->inst = this;
        tp->port = it->first;
        tp->node_id = it->second;
        pthread_create(&threads[thread_i++], nullptr, MergerOperator::run_node_thread_wrapper, tp);
    }

    void *status;
    for (int i = 0; i < port_to_node_map.size(); i++) {
        pthread_join(threads[i], &status);
    }
}