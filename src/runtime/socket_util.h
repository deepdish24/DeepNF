#pragma once

#include "address.h"
#include "packet.h"
 

struct sockdata
{
    int size;
    char *buffer;
};

/* opens a datagram socket and returns the fd or -1 */
int open_socket();

/* binds socket with given fd to given port */
int bind_socket(int sockfd, int portno);

/* sends data to the specified address over a datagram socket */
int send_data(char *buf, int size, int sockfd, address *addr);

 /* sends a packet to a specified address 
  * and returns 0 if successful, -1 otherwise */
int send_packet(packet *p, int sockfd, address *addr);

/* receives data from a socket with given fd and returns a sockdata struct */
sockdata *receive_data(int sockfd);


