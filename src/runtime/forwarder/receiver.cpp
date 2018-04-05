#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string>

int main(int argc, char *argv[]) {
    // open socket
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Cannot open socket (%s)\n", strerror(errno));
        exit(1);
    }

    int portnum = std::stoi(argv[1]);
    printf("listening on port %d\n", portnum);

    // connect to server
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(portnum);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "ERROR on binding");
        exit(-1);
    }

    char buffer[65535];
    listen(sockfd, 1000);

    struct sockaddr_in cli_addr;
    socklen_t clilen;
    int newsockfd;

    while (true) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        int buf_i = 0;
        int cur_i = read(newsockfd, buffer + buf_i, 65535 - buf_i);

        while (cur_i > 0) {
            buf_i += cur_i;
            cur_i = read(newsockfd, buffer + buf_i, 65535 - buf_i);
        }
        close(newsockfd);

        printf("Echo: [%s] (%d bytes)\n", buffer, strlen(buffer));
        close(newsockfd);
    }
}
