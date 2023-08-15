#ifndef _COMMAND_HANDLER_H_
#define _COMMAND_HANDLER_H_

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
#include <sys/stat.h>
#include <sys/sendfile.h>

#include "socket.h"
#include "dir.h"
#include "port.h"

typedef struct tokens{
    char** tokens;
    char* cwd;
    const char* root_dir;
    char* ip;
    int command_index;
    int number_of_tokens;
    int is_valid;
    int control_fd;
    int data_fd;
    int is_logged_in;
    port_info* port_pool;
    int data_port;
} command_t;

/* 
    This function checks if the command is from the list of supported commands.

    returns: -1 if error, else 0
 */
command_t* validate_command(char* command_string, command_t* command);
/* 
    This function checks if user is allowed to use the server.
    logged_in_session is a flag if the user has logged in before

    returns: -1 if error, else 0

 */
int handle_login(command_t*);
/* 
    This function handles the quit command
 */
int handle_quit(command_t*);

/* 
    This function marches all other functions to handle all the commands supported.
    returns: void
 */
int handle_commands(int fd, const char* cwd, char* ip, port_info* port_pool);

/* 
    This function handles the mode command
 */
int handle_mode(command_t* command);
/* 
    This function handles the structure command
 */
int handle_stru(command_t* command);
/* 
    This function handles the type command
 */
int handle_type(command_t* command);
/* 
    This function handles changing work directory command
 */
int handle_cwd(command_t* command);
/* 
    This function handles the cdup command
 */
int handle_cdup(command_t* command);
/* 
    This function handles the pasv command
 */
int handle_pasv(command_t* command);
/* 
    This function handles the retr command
 */
int handle_retr(command_t* command);
/* 
    This function handles the nlst command
 */
int handle_nlst(command_t* command);
/* 
    This function takes care of initializing a command_t and returns a pointer to it.
 */
command_t* create_command(int, int, char*, const char*);
/* 
    This command takes care of freeing command_t. This method does not consider a ref_count to command.
 */
void destroy_command(command_t* command);

/* 
    This function prints command
 */
void dump_command(command_t* command);

#endif