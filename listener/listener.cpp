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

using namespace std;

//Struct used to pass in File descriptor and
//index in vector of FD's to child thread
struct arg_struct {
	int fd;
	int inx;
};

//vector of file descriptors
vector<int> comm_fds = {};

//vector of statuses for each FD in comm_fds
vector<bool> is_running = {};

//boolean denoting debug flag
bool debug = false;

//status of server
static volatile int serverRunning = 1;


/**
 * Function that reads into *buf from fd until
 * new line character is hit
 */
bool do_read(int fd, char *buf) {
	char c = 'a';
	int rcvd = 0;
	while (c != '\n') {
		int n = read(fd, &buf[rcvd], 1);
		if (n < 0) {
			return false;
		}
		rcvd += n;
		c = buf[rcvd - 1];
	}
	return true;
}


/**
 * Function that is called upon SIGINT. Function closes
 * all file descriptors and changes the running status
 * of each thread to false. Before closing the file
 * descriptor, function sends a shutdown message to all
 * clients currently connected
 */
void close_connections() {
	const char* err_message = "-ERR Server shutting down\n";
	for (int i = 0; i < comm_fds.size(); i++) {
		int comm_fd = comm_fds[i];
		write(comm_fd, err_message, strlen(err_message) + 1);
		close(comm_fd);
		is_running[i] = false;
	}
}

/**
 * Callback function to SIGINT. Function changes status
 * of servers, closes all connections to server, and then
 * exits.
 */
void intHandler(int dummy) {
	serverRunning = 0;
	close_connections();
	exit(1);
}

/**
 * Worker function that child thread runs. Function sends greeting
 * message to client and then runs while the client is connected to server
 */
/*void *worker(void *arg)
{
	struct arg_struct *args = (struct arg_struct *)arg;
	int comm_fd = (*args).fd;
	int inx = (*args).inx;
	if (debug) {
		printf("[%d] New connection\n", comm_fd);
	}
	char str[1000];
	char cmd[5];
	char txt[1000];
	char str2[1000];

	const char* done = "quit";
	const char* echo = "echo";
	const char* greeting = "+OK Server ready (Author: Deepan Saravanan / deepans)\r\n";
	const char* unrecog_cmd = "-ERR Unknown command\r\n";
	const char* header = "+OK ";

	if (debug) {
		printf("[%d] S: %s", comm_fd, greeting);
	}

	write(comm_fd, greeting, strlen(greeting));
	while (is_running[inx])
	{
		bzero(str, 1000);
		bzero(cmd, 5);
		bzero(txt, 1000);
		bzero(str2, 1000);

		do_read(comm_fd, str);

		if (debug) {
			printf("[%d] C: %s", comm_fd, str);
		}

		memcpy(cmd, &str[0], 4);
		memcpy(txt, &str[5], strlen(str));
		cmd[4] = '\0';
		standardize(cmd);

		if (strcmp(cmd, echo) == 0) {

			strcat(str2, header);
			strcat(str2, txt);

			if (debug) {
				printf("[%d] S: %s", comm_fd, str2);
			}
			write(comm_fd, str2, strlen(str2));
		} else if (strcmp(cmd, done) == 0) {
			const char* exit = "+OK Goodbye!\r\n";
			if (debug) {
				printf("[%d] S: %s", comm_fd, exit);
			}
			write(comm_fd, exit, strlen(exit));
			break;
		} else {
			if (debug) {
				printf("[%d] S: %s", comm_fd, unrecog_cmd);
			}
			write(comm_fd, unrecog_cmd, strlen(unrecog_cmd));
		}
	}
	if (debug) {
		printf("[%d] Connection closed\n", comm_fd);
	}

	close(comm_fd);
	is_running[inx] = false;
	pthread_exit(NULL);
}*/

int main(int argc, char *argv[])
{
	signal(SIGINT, intHandler);
	int c;
	int port_num = 10000;
	opterr = 0;
	char buffer[256];

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

	int i = 0;

	struct sockaddr_in clientaddr;
	socklen_t clientaddrlen = sizeof(clientaddr);
	//int *fd = (int*)malloc(sizeof(int));
	int fd = accept(sockfd, (struct sockaddr*) &clientaddr, &clientaddrlen);
	//struct arg_struct args;
	//args.fd = *fd;
	//args.inx = i;

	read(fd,buffer,255);
	printf("Here is the message: %s", buffer);

	//start child thread with new connection
	//passing in the FD and inx in vector of FD's
	//pthread_t thread;
	//pthread_create(&thread, NULL, worker, (void *)&args);
	i += 1;
	close_connections();
	exit(1);
}
