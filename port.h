#ifndef PORT_INFO_H
#define PORT_INFO_H

#include <pthread.h>


#define MIN_PORT 5555
#define MAX_PORT 5580

typedef struct {
    int port;
    int is_allocated;
    pthread_mutex_t lock;  // Mutex for each port_info
} port_info;

void initialize_port_pool(port_info *port_pool);
char* allocate_port(port_info *port_pool);
void release_port(port_info *port_pool, int port);
void dump_pool(port_info *port_pool);

#endif
