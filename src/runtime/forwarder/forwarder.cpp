#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <fstream>

#include <runtime/socket_util.h>
#include <runtime/address_util.h>
#include <runtime/log_util.h>
#include <iostream>


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"


std::ofstream log;
pthread_mutex_t log_mutex;

typedef struct threadParams {
    int real_port;
    char* v_addr_str; // string representation of virtual address
    address* virtual_addr; // string in the form [ip]:[portno], ie. 127.0.0.1:8000
} THREAD_PARAMS;


void *run_thread(void *arg) {
    auto *tp = (THREAD_PARAMS*) arg;
    //printf("initializing thread with port: %d, addr: %s\n", tp->real_port, tp->v_addr_str);

    // open socket to listen for requests
    int r_sockfd = open_socket();
    if (r_sockfd < 0) {
        fprintf(stderr, "Cannot open socket: %s", strerror(errno));
        exit(-1);
    }
    bind_socket(r_sockfd, tp->real_port);

    int v_sockfd;
    while (true) {
        // receive a packet
        //printf("\nStarting to listen for requests....\n");
        sockdata *pkt_data = receive_data(r_sockfd);
        packet* p = packet_from_data(pkt_data);

        //printf("Received message on %d: [%s] (%d bytes)\n", tp->real_port, p->data, p->data_size);

        // send received packet to virtual address
        v_sockfd = open_socket();
        if (v_sockfd < 0) {
            fprintf(stderr, "Cannot open socket: %s", strerror(errno));
            exit(-1);
        }

        if (send_packet(p, v_sockfd, tp->virtual_addr) < 0) {
            fprintf(stderr, "Send packet error: %s", strerror(errno));
            exit(-1);
        }

        pthread_mutex_lock(&log_mutex);
        log_util::log_nf(log, p, "forwarder", "Forwarding packet to " + std::string(tp->v_addr_str));
        pthread_mutex_unlock(&log_mutex);

        //printf("Sent packet to virtual address: %s\n", tp->v_addr_str);
    }
}


int main(int argc, char *argv[]) {
    FILE *fp = fopen(argv[1], "r");
    if (fp < 0) {
        perror("Cannot open file (%s)\n");
        exit(1);
    }

    // open log
    log_mutex = PTHREAD_MUTEX_INITIALIZER;
    log.open("log/forwarder/log.txt", std::ios::out);
    if (!log) std::cerr << "Could not open the file!" << std::endl;

    // count number of containers to spawn threads for
    int num_threads = 0;
    size_t len = 2048;
    ssize_t read;
    char *line = nullptr;
    while ((read = getline(&line, &len, fp)) != -1) {
        num_threads++;
    }

    //printf("Read %d lines\n", num_threads);


    pthread_t threads[num_threads];

    // read in input mapping port to virtual IP and port
    rewind(fp);
    num_threads = 0;
    char *ptr;
    while ((read = getline(&line, &len, fp)) != -1) {
        auto *tp = (THREAD_PARAMS *) malloc(sizeof(THREAD_PARAMS));

        // read real port
        ptr = strtok(line, ";");
        tp->real_port = std::stoi(ptr);

        //read virtual address
        ptr = strtok(nullptr, ";");
        tp->v_addr_str = (char *) malloc(strlen(ptr));
        strcpy(tp->v_addr_str, ptr);
        tp->virtual_addr = address_from_string(tp->v_addr_str);

        pthread_create(&threads[num_threads++], nullptr, run_thread, tp);
    }

    void *status;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], &status);
    }
}
#pragma clang diagnostic pop