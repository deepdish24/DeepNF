#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <string>
#include <iostream>

#include "packet.h"

int main()
{
	int listen_fd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(800);
	
	if (bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		std::cerr << "bind error: " << strerror(errno) << std::endl;
		return -1;
	}
	listen(listen_fd, 10);
	
	while (true) {
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof(clientaddr);
		int comm_fd = accept(listen_fd, (struct sockaddr*)&clientaddr, &clientaddrlen);    
		printf("Connection from %s\n", inet_ntoa(clientaddr.sin_addr));


		char buf[1024];
		int n = read(comm_fd, buf, 1023);
		if (n < 0) {
			std::cerr << "read error: " << strerror(errno) << std::endl;
			return -1;
		}
		std::cout << "read " << n << " bytes\n";
		buf[n] = 0;
		u_char *pkt_char = (u_char*)malloc(n);
		memcpy(pkt_char, (void *)buf, n);
		struct packet p(pkt_char, n);
		p.print_info();
		free(pkt_char);
		
		close(comm_fd);
	}

	return 0;
}
