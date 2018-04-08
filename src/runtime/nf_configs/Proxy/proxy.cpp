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

#include "../../address_util.h"
#include "../../socket_util.h"



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
    char* dest_ip = argv[4];
    int dest_port = std::stoi(argv[5]);

    std::string dest_str = stringify(std::string(dest_ip), dest_port);
    address *dest_addr = address_from_string(dest_str);

    // create socket
    int sockfd = open_socket();

    // bind onto a port
    if (bind_socket(sockfd, bind_port) == -1) {
        std::cerr << "bind failure: " << strerror(errno) << std::endl;
        return -1;
    }


    // listen for packets
    while (true) {
        sockdata *pkt_data = receive_data(sockfd);
        if (pkt_data == NULL || pkt_data->size == 0) {
            std::cerr << "packet receive error: " << strerror(errno) << std::endl;
            continue;
        }
        packet* p = packet_from_data(pkt_data);

        // process packet
        printf("\nReceived packet:\n");
        p->print_info();



        // forward packet
        for (address *addr : addresses) {
            send_data(pkt_data->buffer, pkt_data->size, sockfd, addr);
        }

    }

    return 0;
}

void int_handler(int signo)
{
    close(sockfd);
    exit(0);
}


#pragma clang diagnostic pop