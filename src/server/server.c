#include <asm-generic/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <poll.h>

#include "../../include/server_utils.h"

struct client_status {
    char username[32];
    char status[32];
};

int main() {
    int listener;
    int clientfd;
    struct sockaddr_storage clientaddress;
    socklen_t addresslen;

    char *input_buffer = malloc(MAXSTR + 1);

    char clientIP[INET6_ADDRSTRLEN];

    // Server accepts 5 connections
    // if more is needed, we will realloc()
    
    int fd_count = 0;
    int fd_size  = 5;

    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);

    listener = get_listener_socket();

    if (listener == -1) {
        fprintf(stderr, "error getting listening socket");
        exit(1);
    }

    // Add the listener to set
    pfds[0].fd = listener; 
    pfds[0].events = POLLIN;  // Report ready to read on incoming connection
    
    fd_count = 1; // For the listener

    // Event loop
    for (;;) {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for (int i = 0; i < fd_count; i++) {

            // Checks if someone is ready to read
            if (pfds[i].revents & POLLIN) { // We got one
                                            
                if (pfds[i].fd == listener) {

                    // If listener is ready to read, handle new connection
                    addresslen = sizeof(clientaddress);
                    clientfd   = accept(listener, (struct sockaddr *) 
                            &clientaddress, &addresslen);

                    if (clientfd == -1) {
                        perror("accept");
                    } else {
                        add_to_pfds(&pfds, clientfd, &fd_count, &fd_size);     

                        printf("pollserver: new connections from %s on" 
                                "socket %d\n", inet_ntop(
                                    clientaddress.ss_family, 
                                    get_in_addr((
                                            struct sockaddr *)&clientaddress),
                                    clientIP, INET6_ADDRSTRLEN), clientfd);
                    }

                } else { // If not the listener, we're just a regular client

                    int nbytes = recv(pfds[i].fd, input_buffer, 
                            sizeof(input_buffer) + 1, 0);

                    int sender_fd = pfds[i].fd;

                    if (nbytes <= 0) {
                        // Got error or connection closed by client 
                        if (nbytes == 0) {
                            // connection closed 
                            printf("pollserver: socket %d disconnected\n",
                                    sender_fd);
                        } else {
                            perror("recv");
                        }

                        close(pfds[i].fd); 

                        del_from_pfds(pfds, i, &fd_count);

                    } else {

                        for (int j = 0; j < fd_count; j++) {
                            // Send to all clients
                            int dest_fd = pfds[j].fd;

                            if (dest_fd != listener && dest_fd != sender_fd) {
                                if (send(dest_fd, input_buffer, nbytes, 0)
                                        == -1) {
                                    perror("send");
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}
