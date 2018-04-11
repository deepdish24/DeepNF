#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <string>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <fstream>
#include <ctime>
#include <sys/time.h>

#include <runtime/log_util.h>
#include <runtime/socket_util.h>
#include <runtime/address_util.h>


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void int_handler(int sig);


int sockfd;

int main(int argc,char **argv)
{
    clock_t begin;
	signal(SIGINT, int_handler);

	if (argc != 3) {
		std::cerr << "./receiver portno";
		return -1;
	}

    std::ofstream log;
    log.open("log/receiver/log.txt", std::ios::out);
    if (!log) std::cerr << "Could not open the file!" << std::endl;

	// get this server's bind port
    int num_packets = atoi(argv[1]);
    int bind_port = atoi(argv[2]);
    printf("Listening on port %s\n", argv[2]);

	sockfd = open_socket();

    if (bind_socket(sockfd, bind_port) < 0) {
        std::cerr << "bind failure: " << strerror(errno) << std::endl;
        close(sockfd);
        return -1;
    }

    int count = 0;

    while (true) {
        printf("\nListening for packets...\n");

        sockdata *pkt_data = receive_data(sockfd);
        if (pkt_data == NULL || pkt_data->size == 0) { 
            std::cerr << "packet receive error: " << strerror(errno) << std::endl;
            continue;
        }
        count++;

        if (count == num_packets) {
            struct timeval tp;
            gettimeofday(&tp, NULL);
            long long mslong = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000; //get current timestamp in milliseconds

            std::cout << "TOTAL TIME: " << mslong << std::endl;
            exit(0);
        }

        packet *p = packet_from_data(pkt_data);

        log << p->get_pkt_id() << "," << p->get_src_ip() << "," << p->get_src_port() << ","
            << p->get_dest_ip() << "," << p->get_dest_port() << "," << p->get_payload() << std::endl;
        printf("Wrote to file\n");


        delete pkt_data;
        
        p->print_info();
        printf("Received %d packets in total\n", count);
    }

    close(sockfd);
	return 0;
}

void int_handler(int sig)
{
    close(sockfd);
    exit(0);
}

#pragma clang diagnostic pop