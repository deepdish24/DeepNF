#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h> 
#include <string>
#include <iostream>
#include <math.h>  
#include <vector>

#include "../address_util.h"
#include "../socket_util.h"

// list of addresses to forward packets to
std::vector<address*> addresses;

int main(int argc, char *argv[]) 
{
	if (argc < 2) {
		std::cerr << "./sender [-n <number of packets>] IP:port [IP:port] .....\n";
		return -1;
	}

	// TODO: addresses shouldn't be hardcoded
	std::string sip = "173.16.1.2";
	int sp = 800;
	std::string dip = "173.16.1.4";
	int dp = 800;
	
	int num_packets = 10;

	// processing program arguments
	// -n flag is used to specify number of packets
	static const char *optString = "n:";
	int opt = 0;
	opt = getopt(argc, argv, optString);
	
	if (opt == 'n') {
		num_packets = atoi(optarg);
	}
	
	// store the addresses to forward packets to
	if (optind <= argc - 1) {
		for (int i = optind; i < argc; i++) {
			addresses.push_back(address_from_string(argv[i]));
		}
	} else {
		std::cerr << "./sender [-n <number of packets>] IP:port [IP:port] .....\n";
		return -1;
	}

	// creating a datagram socket
	int sockfd = open_socket();
	if (sockfd < 0) {
		std::cerr << "Cannot open socket: " << strerror(errno) << std::endl;
		exit(-1);
	}  

	for (int i = 0; i < num_packets; i++) {
		// construct the packet
		std::string data = "hello" + std::to_string(i);
		struct packet p(sip, sp, dip, dp, (int)pow(2, 15) + i, data);
		
		for (address *addr : addresses) {
			if (send_packet(&p, sockfd, addr) < 0) {
				std::cerr << "packet send error: " << strerror(errno) << std::endl;
			}
		}	
	}
	
	close(sockfd);
	
	return 0;
}


