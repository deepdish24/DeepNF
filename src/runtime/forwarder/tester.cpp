#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(int argc, char *argv[]) {

    // opens a datagram socket and returns the fd or -1 */
    int open_socket();

    // binds socket with given fd to given port */
    int bind_socket(int sockfd, int portno);


    while (true) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        int buf_i = 0;
        ssize_t cur_i = read(newsockfd, buffer + buf_i, (size_t) 65535 - buf_i);

        while (cur_i > 0) {
            buf_i += cur_i;
            cur_i = read(newsockfd, buffer + buf_i, (size_t) 65535 - buf_i);
        }
        close(newsockfd);

        printf("Echo: [%s] (%d bytes)\n", buffer, (int) strlen(buffer));
        close(newsockfd);
    }
}

#pragma clang diagnostic pop