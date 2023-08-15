
#include "socket.h"
#include "command_handler.h"

int create_socket(char* port){
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        
        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}


int accept_connections(int sockfd, const char* cwd, port_info* port_pool){
    int new_fd;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    while(1) {  
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            break;
        }
        char* ip = int_to_ip(get_ip_from_fd(new_fd));

        socket_thread_t* socket_thread = malloc(sizeof(socket_thread_t));
        // printf("new fd value is %d\n", new_fd);
        socket_thread->fd = new_fd;
        socket_thread->cwd = cwd;
        socket_thread->ip = ip;
        socket_thread->port_pool = port_pool;
        pthread_t child;
        dump_pool(port_pool);
        pthread_create(&child, NULL, socket_thread_handler, (void*) socket_thread);
        pthread_detach(child);
    }        
    return 0;
}


void* socket_thread_handler(void* socket_thread){
    socket_thread_t* socket = socket_thread;
    int is_error = 0;
    if((is_error = _send(socket->fd, "220 Welcome\r\n")) != -1){
        // printf("socket fd is %d and cwd is %s and ip is %s\n", socket->fd, socket->cwd, socket->ip);
        handle_commands(socket->fd, socket->cwd, socket->ip, socket->port_pool);
    }
    free(socket->ip);
    close(socket->fd);
    free(socket);
}


int _send(int fd, const char* buf){
    // printf("size of msg %s %ld\n", buf, strlen(buf));
    if (send(fd, buf, strlen(buf), 0) == -1){
        return -1;
    }
    return 0;
}


int send_file(int sock, char* filename)
{
    FILE* file = fopen(filename, "rb");
    if(file != NULL) {
        struct stat fileStat;
        fstat(fileno(file), &fileStat);

        if(sendfile(sock, fileno(file), NULL, fileStat.st_size) == -1) {
            return -2;
        }
        fclose(file);
    } else {
        return -1;
    }
    return 0;
}

int get_ip_from_fd(int fd){
    struct sockaddr_in sockaddr;
    // memset(&sockaddr, 0, sizeof(sockaddr));
    unsigned int len = sizeof(sockaddr);
    getsockname(fd, (struct sockaddr *) &sockaddr, &len);
    return sockaddr.sin_addr.s_addr;
}

/* 
    The caller is responsible for freeing the pointer
 */
char* int_to_ip(int addr) 
{
    int byte3 = addr & 0xFF;
    int byte2 = (addr >> 8) & 0xFF;
    int byte1 = (addr >> 16) & 0xFF;
    int byte0 = (addr >> 24) & 0xFF;

    char* ip = malloc(sizeof(char) * 20);
    sprintf(ip, "%d.%d.%d.%d", byte3, byte2, byte1, byte0);
    
    return ip;
}