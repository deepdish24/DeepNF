#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h> 
#include <string>
#include <iostream>

int main(int argc, char *argv[]) 
{
	if (argc != 3) {
		std::cerr << "need 2 arguments, ip and port\n";
		return -1;
	}

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);  
	if (sockfd < 0) {
	  fprintf(stderr, "Cannot open socket (%s)\n", strerror(errno));    
	  exit(1);
	}  
	
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(std::stoi(argv[2]));
	inet_pton(AF_INET, argv[1], &(servaddr.sin_addr));
	
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		std::cerr << "connect error: " << strerror(errno) << std::endl;
		return -1;
	}    
	
	std::string data = "hi this is akash\n";

	int num_bytes = write(sockfd, data.c_str(), data.size());
	if (num_bytes < 0) {
		std::cerr << "write error: " << strerror(errno) << "\n";
		return -1;
	}
	
	close(sockfd);

	return 0;
}
