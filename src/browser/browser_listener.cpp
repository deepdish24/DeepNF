#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <netdb.h>
#include <ctype.h>
#include <vector>
#include <signal.h>
//#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	int c;
	int port_num = 10000;
	opterr = 0;
	char buffer[256];

	while ((c = getopt(argc, argv, "p:a")) != -1)
	{
		switch(c)
		{
		case 'p':
			port_num = atoi(optarg);
			break;
		case 'a':
			fprintf(stderr, "Deepan Saravanan (deepans)\n");
			exit(1);
		}
	}

	//open socket as a file descriptor
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		fprintf(stderr, "Cannot open socket\n");
		exit(1);
	}


	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port_num);
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);

	bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	listen(sockfd, 100);

	struct sockaddr_in clientaddr;
	socklen_t clientaddrlen = sizeof(clientaddr);
	int fd = accept(sockfd, (struct sockaddr*) &clientaddr, &clientaddrlen);
	printf("connection set\n");
	char ack[15];
	strcpy(ack, "ok good!\n");

	bool system_call_processed = false;

	while (true) {
		bzero(buffer, sizeof(buffer));
		int x = read(fd, buffer, sizeof(buffer) - 1);
		if (x == 0) {
			break;
		} else {
			printf("Here is the message: %s", buffer);
			/*if (!system_call_processed) {
				system_call_processed = true;
				system("myFile.sh");
			}*/
			write(fd, ack, strlen(ack));
		}
	}
}
