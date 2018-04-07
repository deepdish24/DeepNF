#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string>
#include <cmath>
#include <cstdlib>
#include <runtime/socket_util.h>
#include <runtime/address_util.h>
#include <time.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(int argc, char *argv[]) {

    // argc[1] == "0" is a sender
    if (std::stoi(argv[1]) == 0) {
        int sockfd = open_socket();
        if (sockfd < 0) {
            fprintf(stderr, "Cannot open socket: %s", strerror(errno));
            exit(-1);
        }
        char* ip = argv[2];
        int portno = std::stoi(argv[3]);

        // prepare destination address
        char* addr_str = (char*) malloc(strlen(ip)+strlen(argv[2])+2);
        strncpy(addr_str, ip, strlen(ip));
        strcat(addr_str, ":");
        strcat(addr_str, argv[3]);
        printf("Address to send to: %s\n", addr_str);
        address* addr = address_from_string(addr_str);

        // prepare packet
        std::string data("null packetto");
        std::string sip = "127.0.0.1";
        int sp = 7777;
        std::string dip(argv[3]);
        int dp = portno;
        srand ( time(NULL) );
        struct packet p(sip, sp, dip, dp, (unsigned short) 17, data);
        printf("before nullifying buf: %02X\n, p->pkt);

        printf("Going to nullify packet now\n");
        p.nullify();
        bool is_null = p.is_null();
        if (is_null) {
            printf("null\n");
        } else {
            printf("not null\n");
        }

        if (send_packet(&p, sockfd, addr) < 0) {
            fprintf(stderr, "Send packet error: %s", strerror(errno));
            exit(-1);
        }
        printf("Sent null packet\n");
    }

    else {
        printf("Invalid 1st argument, should be 1 for receiver or 0 for sender\n");
    }
}

#pragma clang diagnostic pop