#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "port.h"

typedef struct socket_thread{
    int fd;
    port_info* port_pool;
    const char* cwd;
    char* ip;
} socket_thread_t;

/*
    This function creates a socket on specific port passed as argument.
    returns: int fd corresponds to the socket file descriptor
*/
int create_socket(char* port);

/* 
    This function takes socket fd and starts accepting connections. It's called 
    only after a connection has been created.
    returns: -1 on error, else 0.

 */
int accept_connections(int sockfd, const char*, port_info*);


/* 
    This function is an extra layer to handle buffer length and flags
    returns: -1 on error, else 0 

 */
int _send(int fd, const char* buf);

/* 
    This function takes a fd and a pathname and attempts to send the file
    returns: -1 if error, else 0
 */
int send_file(int fd, char*);

/* 
    This function converts an integer to a ip format "a.b.c.d"
    return: char*
        Note: The caller is responsible for freeing the pointer
/ */
char* int_to_ip(int addr);


/* 
    Takes a fd and returns the ip of the socket connected to the fd
    returns: int
 */
int get_ip_from_fd(int fd);


void* socket_thread_handler(void *);


#endif