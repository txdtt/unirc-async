#include <asm-generic/socket.h>
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

void *receive_message(void *args) {
    int sockfd = *(int *)args;
    char server_buffer[1024];

    while(1){
        memset(server_buffer, 0, sizeof(server_buffer));

        ssize_t bytes_received = recv(sockfd, server_buffer, sizeof(server_buffer) - 1, 0);
        if (bytes_received == -1) {
            // No data received, it's non-blocking, so this can happen
            continue;
        } else if (bytes_received == 0) {
            // Connection closed by the server
            printf("Server closed the connection\n");
            break;
        }

        server_buffer[strcspn(server_buffer, "\n")] = '\0';

        puts(server_buffer);
    }

    return NULL;
}
