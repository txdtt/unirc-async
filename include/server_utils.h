#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#define MAXSTR 1638400

#define PORT "3940"

void *get_in_addr(struct sockaddr *sa);

int get_listener_socket();

void add_to_pfds(struct pollfd *pfds[], int clientfd, int *fd_count, int *fd_size);

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count);

#endif
