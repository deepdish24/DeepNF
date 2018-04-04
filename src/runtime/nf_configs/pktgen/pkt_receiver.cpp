#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <string>
#include <iostream>

int main()
{
	int listen_fd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(800);
	
	bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
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
		buf[1023] = 0;

		std::cout << buf << "\n";
		close(comm_fd);
	}

	return 0;
}
