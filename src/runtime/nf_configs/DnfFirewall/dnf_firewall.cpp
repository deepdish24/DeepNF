#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <string.h>
#include <errno.h>
// #include <sys/socket.h>

#include "../address_util.h"
#include "../socket_util.h"



void int_handler(int signal);


int sockfd;
std::vector<address*> addresses;

int main(int argc,char **argv)
{   
    signal(SIGINT, int_handler);

    if (argc < 3) {
        std::cerr << "./fw my_port destIP:port [destIP2:port2] .......\n";
        return -1;
    }

    // get this server's bind port
    int bind_port = atoi(argv[1]);
   
    for (int i = 2; i < argc; i++) {
        address *addr = address_from_string(argv[i]);
        if (addr != NULL) {
            addresses.push_back(addr);
        }
    }

    if (addresses.size() == 0) { 
        std::cerr << "No valid destination addresses\n";
        return -1; 
    }

    // create socket
    int sockfd = open_socket();

    // bind onto a port
    int res = bind_socket(bind_port);

    if (res == -1) {
        std::cerr << "bind failure: " << strerror(errno) << std::endl;
        return -1;
    }
    
    
    while (true) {

        sockdata *pkt_data = receive_data(sockfd);
        if (pkt_data == NULL || pkt_data->size == 0) { 
            std::cerr << "packet receive error: " << strerror(errno) << std::endl;
            continue;
        }
        
        // TODO: process packet

        // forward packet
        for (address *addr : addresses) {
            send_data(pkt_data->buffer, pkt_data->size, sockfd, addr);
        }
        
    }

    return 0;
}

void int_handler(int signal)
{
    close(sockfd);
    exit(0);
}



// void run_firewall(struct packet *pkt_info)
// {
//     /* if dip = 80, send a null packet (drop packet) */
//     if (htons(pkt_info->tcp_header->dest) == 8000) {
//         pkt_info->nullify();
//     }
//     forward_packet(pkt_info->pkt, pkt_info->size);
// }

