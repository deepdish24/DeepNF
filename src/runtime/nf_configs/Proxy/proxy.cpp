#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include <signal.h>
#include<unistd.h>
// #include <sys/socket.h>

#include "address_util.h"
#include "socket_util.h"
#include "log_util.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void int_handler(int signo);


int sockfd;
std::vector<address*> addresses;

/**
 * The proxy takes in a server ip and port. For any packets that the proxy receives, it changes the ip and port of
 * the packet to that of the server ip and port.
 *
 * @return
 */
int main(int argc,char **argv)
{
    signal(SIGINT, int_handler);

    // get this server's bind port
    int bind_port = std::stoi(argv[1]);
    char* server_ip = argv[2];
    int server_port = std::stoi(argv[3]);

    for (int i = 4; i < argc; i++) {
        address *addr = address_from_string(argv[i]);
        if (addr != NULL) {
            addresses.push_back(addr);
        }
    }

    if (addresses.size() == 0) {
        std::cerr << "No valid destination addresses\n";
        return -1;
    }

    // setup log for this NF
    std::ofstream log;
    log.open("/log/log.txt", std::ios::out);
    if (!log) std::cerr << "Could not open the file!" << strerror(errno) << std::endl;

    // create socket
    int sockfd = open_socket();

    // bind onto a port
    if (bind_socket(sockfd, bind_port) == -1) {
        std::cerr << "bind failure: " << strerror(errno) << std::endl;
        close(sockfd);
        return -1;
    }
    printf("Proxy listening for packets on port: %d\n", bind_port);


    // listen for packets
    while (true) {
        sockdata *pkt_data = receive_data(sockfd);
        if (pkt_data == NULL || pkt_data->size == 0) {
            std::cerr << "packet receive error: " << strerror(errno) << std::endl;
            continue;
        }
        packet* p = packet_from_data(pkt_data);

        // process packet
        printf("\n-------------------------------------\n");
        printf("\nReceived packet:\n");
        p->print_info();

        // overwrite received packet to point to server ip and port
        p->write_dest_ip(std::string(server_ip));
        p->write_dest_port(server_port);

        printf("\nSending modified packet:\n");
        p->print_info();

        // forward packet
        for (address *addr : addresses) {
            if (send_packet(p, sockfd, addr) < 0) {
                fprintf(stderr, "Send packet error: %s", strerror(errno));
                close(sockfd);
                exit(-1);
            }
        }

        log_util::log_nf(log, p, "proxy",
            "rewrote packet destination to " + std::string(server_ip) + ":" + std::to_string(server_port));

    }
    close(sockfd);
    return 0;
}

void int_handler(int signo)
{
    close(sockfd);
    exit(0);
}


#pragma clang diagnostic pop