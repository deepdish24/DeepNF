#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string>
#include <cmath>
#include <cstdlib>

#include "../address_util.h"
#include "../socket_util.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(int argc, char *argv[]) {
    int portno = std::stoi(argv[2]);

    // argc[1] == "0" is a sender
    if (strcmp(argv[1], "0")) {
        int sockfd = open_socket();
        if (sockfd < 0) {
            fprintf(stderr, "Cannot open socket: %s", strerror(errno));
            exit(-1);
        }
        char* ip = argv[3];
        char* msg = argv[4];

        // prepare destination address
        char* addr_str = (char*) malloc(strlen(ip)+strlen(argv[2])+2);
        strcpy(addr_str, ip);
        strcat(addr_str, ":");
        strcat(addr_str, argv[2]);
        address* addr = address_from_string(addr_str);

        // prepare packet
        std::string data(msg);
        std::string sip = "127.0.0.1";
        int sp = 8001;
        std::string dip(argv[3]);
        int dp = portno;
        struct packet p(sip, sp, dip, dp, (unsigned short) rand(), data);

        if (send_packet(&p, sockfd, addr) < 0) {
            fprintf(stderr, "Send packet error: %s", strerror(errno));
            exit(-1);
        }
    }

    // argc[1] == "1" is a receiver
    else if (strcmp(argv[1], "1")) {
        printf("Opening receiver to listening on port %d\n", portno);

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
            printf("listening for data...\n")
            sockdata *pkt_data = receive_data(sockfd);

            printf("Echo: [%s] (%d bytes)\n", pkt_data->buffer, pkt_data->size);
            delete pkt_data;
        }
    }

    printf("Invalid 1st argument, should be 1 for receiver or 0 for sender");
}

#pragma clang diagnostic pop