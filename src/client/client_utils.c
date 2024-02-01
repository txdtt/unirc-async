#include <asm-generic/socket.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>

#include "../../include/client_utils.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

void *receive_message(void *args) {
    int sockfd = *(int *)args;

    char server_buffer[MAXSTR];

    while(1){
        memset(server_buffer, 0, sizeof(server_buffer));

        ssize_t bytes_received = recv(sockfd, server_buffer, MAXSTR + 1, 0);
        if (bytes_received == -1) {
            // No data received, it's non-blocking, so this can happen
            perror("recv");
            continue;
        } else if (bytes_received == 0) {
            // Connection closed by the server
            printf("Server closed the connection\n");
            break;
        }

        server_buffer[strcspn(server_buffer, "\n")]   = 0;

        pthread_mutex_lock(&lock);

        //DEBUGGIN
        //printf("Received %zd bytes: '%s'\n", bytes_received, server_buffer);

        printf("%s\n", server_buffer);

        fflush(stdout);

        pthread_mutex_unlock(&lock);
    }

    return NULL;
}
