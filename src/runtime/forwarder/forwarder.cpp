#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

typedef struct threadParams {
    int real_port;
    int virtual_port;
    char* virtual_ip;
} THREAD_PARAMS;


void *run_thread(void *arg) {
    auto *tp = (THREAD_PARAMS*) arg;
    printf("initializing thread with port: %d, ip: %s:%d\n", tp->real_port, tp->virtual_ip, tp->virtual_port);

    // open socket to listen for requests
    int r_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (r_sockfd < 0) {
        fprintf(stderr, "Cannot open socket (%s)\n", strerror(errno));
        exit(1);
    }

    // open connection to virtual port/IP
    int v_sockfd;
    struct sockaddr_in v_servaddr;

    // open real port and listen for incoming requests
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(tp->real_port);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(r_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("ERROR on binding");
        exit(-1);
    }

    size_t bufsize = 65535;
    char buffer[bufsize];
    listen(r_sockfd, 1000);

    struct sockaddr_in cli_addr;
    socklen_t clilen;
    int newsockfd;

    while (true) {
        printf("\nStarting to listen for requests....\n");
        memset(buffer, 0, bufsize);
        newsockfd = accept(r_sockfd, (struct sockaddr *) &cli_addr, &clilen);
        int buf_i = 0;
        ssize_t cur_i = read(newsockfd, buffer + buf_i, bufsize - buf_i);

        while (cur_i > 0) {
            buf_i += cur_i;
            cur_i = read(newsockfd, buffer + buf_i, bufsize - buf_i);
        }
        close(newsockfd);
        printf("Received message on %d: [%s] (%d bytes)\n", tp->real_port, buffer, (int) strlen(buffer));

        // open connection to virtual port/IP
        v_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        bzero(&v_servaddr, sizeof(v_servaddr));
        v_servaddr.sin_family = AF_INET;
        v_servaddr.sin_port = htons(tp->virtual_port);
        inet_pton(AF_INET, tp->virtual_ip, &(v_servaddr.sin_addr));

        // forward message to virtual IP/port
        if (connect(v_sockfd, (struct sockaddr*)&v_servaddr, sizeof(v_servaddr)) >= 0) {
            write(v_sockfd, buffer, strlen(buffer));
            close(v_sockfd);
            printf("Finished message to virtual port\n");
        }
        else {
           perror("Failed to connect: %s\n");
        }
    }
}


int main(int argc, char *argv[]) {
    FILE *fp = fopen(argv[1], "r");
    if (fp < (FILE*) 0) {
        perror("Cannot open file (%s)\n");
        exit(1);
    }

    // count number of containers to spawn threads for
    int num_threads = 0;
    size_t len = 2048;
    ssize_t read;
    char *line = nullptr;
    while ((read = getline(&line, &len, fp)) != -1) {
        num_threads++;
    }

    printf("Read %d lines\n", num_threads);


    pthread_t threads[num_threads];

    // read in input mapping port to virtual IP and port
    rewind(fp);
    num_threads = 0;
    char *ptr;
    while ((read = getline(&line, &len, fp)) != -1) {
        auto *tp = (THREAD_PARAMS *) malloc(sizeof(THREAD_PARAMS));

        // read real port
        ptr = strtok(line, ";");
        tp->real_port = std::stoi(ptr);

        //read virtual ip
        ptr = strtok(nullptr, ";");
        ptr = strtok(ptr, ":");
        char *virtual_ip = (char *) malloc(strlen(ptr));
        strcpy(virtual_ip, ptr);
        tp->virtual_ip = virtual_ip;

        // read virtual port
        ptr = strtok(nullptr, ":");
        tp->virtual_port = std::stoi(ptr);

        pthread_create(&threads[num_threads++], nullptr, run_thread, tp);
    }

    void *status;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], &status);
    }
}
#pragma clang diagnostic pop