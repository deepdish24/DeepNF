#include <arpa/inet.h>
#include <string.h>
#include <strings.h> 
#include <iostream>

#include "socket_util.h"


#define BUFFER_SIZE 1000


int open_socket()
{
	return socket(PF_INET, SOCK_DGRAM, 0);
}

int bind_socket(int sockfd, int portno)
{
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
	 	return -1;
	}

	return 0;
}


int send_data(char *buf, int size, int sockfd, address *addr)
{
	struct sockaddr_in server_dest;
	bzero(&server_dest, sizeof(server_dest));
	server_dest.sin_family = AF_INET;
	server_dest.sin_port = htons(addr->port);
	inet_pton(AF_INET, addr->ip.c_str(), &(server_dest.sin_addr));

	int num_bytes = sendto(sockfd, buf, size, 0, 
			(struct sockaddr*)&server_dest, sizeof(struct sockaddr));
	printf("sending data buf: %02X, %s\n", buf, buf);
	if (num_bytes < 0) {
		return -1;
	}
	std::cout << "sent " << num_bytes << " bytes\n";
	return num_bytes;
}

int send_packet(packet *p, int sockfd, address *addr)
{
	std::cout << "packet size = " << p->size << "\n";
	char txt[100];
	strcpy ((char*) txt,"hi my dear friend");
	return send_data((char*) txt, p->size, sockfd, addr);
}


sockdata *receive_data(int sockfd)
{
	struct sockaddr_in src;
	socklen_t srclen = sizeof(src);
	
	char *buf = (char*)malloc(BUFFER_SIZE);
	int rlen = recvfrom(sockfd, buf, BUFFER_SIZE - 1, 0, (struct sockaddr*)&src, &srclen);
	if (rlen < 0) {
		return NULL;
	}
	printf("received data: buf: %02X, %s\n", buf, buf);
	std::cout << "received " << rlen << " bytes\n";
	buf[rlen] = 0;

	sockdata *d = new sockdata();
	d->size = rlen;
	d->buffer = buf;

	return d;
}

packet *packet_from_data(sockdata *d)
{
	int pkt_size = d->size;
	u_char *pkt_char = (u_char*)malloc(pkt_size);
	memcpy(pkt_char, (void *)d->buffer, pkt_size);
	packet *p = new packet(pkt_char, pkt_size);
	return p;
}