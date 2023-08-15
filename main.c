#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include "dir.h"
#include "usage.h"
#include "socket.h"
#include "port.h"
#include <pthread.h>


int main(int argc, char **argv) {
    
    // Check the command line arguments
    if (argc != 2) {
      usage(argv[0]);
      return -1;
    }

    int socket = create_socket(argv[1]);
    port_info port_pool[MAX_PORT - MIN_PORT + 1];
    initialize_port_pool(port_pool);

    if (socket == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
      perror("getcwd() error");

    const char* cwd_val = cwd;

    if(accept_connections(socket, cwd_val, port_pool) == -1){
      return -1;
    }

    close(socket);

    return 0;

}
