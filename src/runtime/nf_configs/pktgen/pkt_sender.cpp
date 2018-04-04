#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h> 
#include <string>
#include <iostream>
#include <math.h>  

#include "packet.h"

int main(int argc, char *argv[]) 
{
	if (argc != 4) {
		std::cerr << "need 3 arguments, ip , port and number of fake packets\n";
		return -1;
	}

	std::string dip = argv[1];
	int dp = std::stoi(argv[2]);
	std::string sip = "173.16.1.2";
	int sp = dp;
	int n = std::stoi(argv[3]);

	for (int i = 0; i < n; i++) {
		int sockfd = socket(PF_INET, SOCK_STREAM, 0);  
		if (sockfd < 0) {
		  fprintf(stderr, "Cannot open socket (%s)\n", strerror(errno));    
		  exit(1);
		}  
		
		struct sockaddr_in servaddr;
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(dp);
		inet_pton(AF_INET, dip.c_str(), &(servaddr.sin_addr));

		if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
				std::cerr << "connect error: " << strerror(errno) << std::endl;
				return -1;
		}
		std::string data = "hello" + std::to_string(i);
		struct packet p(sip, sp, dip, dp, (int)pow(2, 15) + i, data);
		int num_bytes = write(sockfd, p.pkt, p.size);
		if (num_bytes < 0) {
			std::cerr << "write error: " << strerror(errno) << "\n";
			return -1;
		}
		std::cout << "wrote " << num_bytes << " bytes\n";
		close(sockfd);
	}
	

	return 0;
}


