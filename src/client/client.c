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

int main() {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char servaddress[32];
    char port[32];
    char input_buffer[MAXSTR];
    char client_name[MAXSTR];
    char username[MAXSTR];

    pthread_t receive_thread;

    printf("Enter your username: ");
    fgets(client_name, sizeof(client_name), stdin);

    printf("Enter server address: ");
    scanf("%s", servaddress);
        
    printf("Enter port number: ");
    scanf("%s", port);

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(servaddress, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    //inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
    printf("Connected\n");

    freeaddrinfo(servinfo);

    client_name[strcspn(client_name, "\n")] = 0;

    strcat(client_name, ": ");
   
    pthread_create(&receive_thread, NULL, receive_message, (void *) &sockfd);

    while(1) {
        memset(username, 0, sizeof(username));

        strcpy(username, client_name);

        fgets(input_buffer, sizeof(input_buffer), stdin);

        if (strncmp(input_buffer, "exit", 4) == 0) {
            printf("exiting...\n");
            close(sockfd);
            return 1;
        }
 
        strcat(username, input_buffer);

        username[strcspn(username, "\n")] = 'A';
 
        if (send(sockfd, username, strlen(username), 0) == -1) {
            perror("send");
            continue;
        }   

    }

    close(sockfd);

    return 0;
}   
