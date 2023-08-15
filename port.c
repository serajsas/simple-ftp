#include "port.h"
#include <stdio.h>
#include <stdlib.h>


void initialize_port_pool(port_info *port_pool) {
    for (int i = 0; i < MAX_PORT - MIN_PORT + 1; i++) {
        port_pool[i].port = MIN_PORT + i;
        port_pool[i].is_allocated = 0;
        pthread_mutex_init(&port_pool[i].lock, NULL);  // Initialize mutex for each port_info
    }
}

char* allocate_port(port_info *port_pool) {
    for (int i = 0; i < MAX_PORT - MIN_PORT + 1; i++) {
        pthread_mutex_lock(&port_pool[i].lock);  // Acquire mutex before modifying port_pool

        if (!port_pool[i].is_allocated) {
            port_pool[i].is_allocated = 1;
            int port_number = port_pool[i].port;

            pthread_mutex_unlock(&port_pool[i].lock);  // Release mutex after modification

            // Convert the port number to a string
            char *port_string = (char *)malloc(6 * sizeof(char));  // Max port length is 5 digits + '\0'
            if (port_string == NULL) {
                // Handle allocation failure
                return NULL;
            }
            snprintf(port_string, 6, "%d", port_number);
            
            return port_string;
        }

        pthread_mutex_unlock(&port_pool[i].lock);  // Release mutex if port is already allocated
    }
    return NULL;  // No available ports
}

void release_port(port_info *port_pool, int port) {
    for (int i = 0; i < MAX_PORT - MIN_PORT + 1; i++) {
        pthread_mutex_lock(&port_pool[i].lock);  // Acquire mutex before modifying port_pool

        if (port_pool[i].port == port) {
            port_pool[i].is_allocated = 0;
            printf("releasing port %d\n", port);

            pthread_mutex_unlock(&port_pool[i].lock);  // Release mutex after modification
            break;
        }

        pthread_mutex_unlock(&port_pool[i].lock);  // Release mutex if port is not found
    }
}

void cleanup_port_pool(port_info *port_pool) {
    for (int i = 0; i < MAX_PORT - MIN_PORT + 1; i++) {
        pthread_mutex_destroy(&port_pool[i].lock);  // Destroy mutex for each port_info
    }
}

void dump_pool(port_info *port_pool) {
    for (int i = 0; i < MAX_PORT - MIN_PORT + 1; i++) {
        printf("port %d is %d\n", port_pool[i].port, port_pool[i].is_allocated);
    }
}