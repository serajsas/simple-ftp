#include "command_handler.h"

static const char* codes[] = {"530 Not logged in.\r\n", "501 Syntax error in parameters or arguments.\r\n","500 Syntax error, command unrecognized.\r\n", "230 User logged in, proceed.\r\n"
                            , "504 Command not implemented for that parameter.\r\n", "200 Command okay.\r\n", "221 Service closing control connection\r\n"
                            , "550 Requested action not taken. File unavailable (e.g., file not found, no access).\r\n", "250 Requested file action okay, completed.\r\n"
                            , "451 Requested action aborted. Local error in processing.\r\n", "425 Can't open data connection.\r\n", "226 Transfer complete\r\n"
                            , "125 Here it comes\r\n"};

static const char* commands[] = {"USER", "PASV", "NLST", "RETR", "MODE", "STRU", "TYPE", "CDUP", "CWD", "QUIT"};
int (*handlers[10]) (command_t*) = {handle_login, handle_pasv, handle_nlst, handle_retr, handle_mode, handle_stru, handle_type, handle_cdup, handle_cwd, handle_quit};

int is_command_valid(char* string_to_check)
{
    for(int i = 0; i < 10; i++)
    {
        if(strcasecmp(string_to_check, commands[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

void remove_rn(char *str) 
{ 
    int i = 0;
    while(str[i] != '\0') {
        if (str[i] == '\r'|| str[i] == '\n') {
            str[i] = '\0';
            i--;
        }
        i++;
    }
}

command_t* tokenize_string(char* command_string, command_t* command){
    char dup[strlen(command_string)];
    strcpy(dup, command_string);
    remove_rn(dup);
    char *tok;
    tok = strtok(dup, " ");
    int acc = 0;
    while (tok != NULL && acc < 10)
    {
        command->tokens[acc] = strdup(tok);
        tok = strtok(NULL, " ");
        acc++;
    }
    command->number_of_tokens = acc;
    // dump_command(command);
    return command;
}

int handle_quit(command_t* command){
    if(command->number_of_tokens != 1){
        if (_send(command->control_fd, codes[1]) == -1) {
            return -1;
        }
        return 0;
    }
    if (_send(command->control_fd, codes[6]) == -1){
        return -1;
    }
    if(command->control_fd != -1){
        close(command->control_fd);
        command->is_logged_in = 0;
    }
    return -4;
}

int handle_login(command_t* command){
    // printf("inside handle_login\n");
    if(command->is_logged_in) {
        if (_send(command->control_fd, codes[3]) == -1) {
            return -1;
        }
        return 0;
    }

    if(command->number_of_tokens != 2) {
        _send(command->control_fd, codes[1]);
        return -1;
    }

    char* username = command->tokens[1];

    if (strcasecmp(username, "test") == 0) {
        command->is_logged_in = 1;
        if (_send(command->control_fd, codes[3]) == -1) {
            return -1;
        }
    } else {
        _send(command->control_fd, codes[0]);
        return -1;  
    }
    return 0;
}

int handle_mode(command_t* command){
    if(!command->is_logged_in){
        _send(command->control_fd, codes[0]);
        return -1;
    }
    if(command->number_of_tokens != 2){
        _send(command->control_fd, codes[1]);
        return -1;
    }
    char* mode = command->tokens[1];
    if(strcasecmp(mode, "S") == 0){
        if (_send(command->control_fd, codes[5]) == -1) {
            return -1;
        }
    }else{
        _send(command->control_fd, codes[4]);
        return -1; 
    }
    return 0;
}

int handle_stru(command_t* command){
    if(!command->is_logged_in){
        _send(command->control_fd, codes[0]);
        return -1;
    }
    if(command->number_of_tokens != 2){
        _send(command->control_fd, codes[1]);
        return -1;
    }
    char* struc = command->tokens[1];
    if(strcasecmp(struc, "F") == 0){
        if (_send(command->control_fd, codes[5]) == -1) {
            return -1;
        }
    }else{
        _send(command->control_fd, codes[4]);
        return -1; 
    }
    return 0;
}

int handle_type(command_t* command){
    if(!command->is_logged_in){
        _send(command->control_fd, codes[0]);
        return -1;
    }
    if(command->number_of_tokens != 2 && command->number_of_tokens != 3){
        _send(command->control_fd, codes[1]);
        return -1;
    }
    char* main_type = command->tokens[1];
    char* optional_ascii;
    if(command->number_of_tokens > 2)
        optional_ascii = command->tokens[2];
    if(strcasecmp(main_type, "A") == 0){
        if(command->number_of_tokens > 2)
            if(strcasecmp(optional_ascii, "N") != 0 && strcasecmp(optional_ascii, "T") != 0 && strcasecmp(optional_ascii, "C") != 0){
                _send(command->control_fd, codes[4]);
                return -1; 
            }
        if (_send(command->control_fd, codes[5]) == -1) {
            return -1;
        }
    }else if(strcasecmp(main_type, "I") == 0){
        if(command->number_of_tokens > 2){
            _send(command->control_fd, codes[1]);
            return -1; 
        }
        if (_send(command->control_fd, codes[5]) == -1) {
            return -1;
        }
    }else{
        _send(command->control_fd, codes[4]);
        return -1; 
    }
    return 0;
}

int handle_cwd(command_t* command){
    if(!command->is_logged_in){
        _send(command->control_fd, codes[0]);
        return -1;
    }
    if(command->number_of_tokens != 2){
        _send(command->control_fd, codes[1]);
        return -1;
    }

    if(!is_path_valid(command->tokens[1])){
        _send(command->control_fd, codes[7]);
        return -1;
    }

    concatPaths(&command->cwd, "/");
    concatPaths(&command->cwd, command->tokens[1]);

    if(isParent(command->cwd, command->root_dir) == 0){
        if(is_dir(command->cwd) == 0){
            _send(command->control_fd, codes[7]);
            return -1;
        }
        if (_send(command->control_fd, codes[8]) == -1) {
            return -1;
        }
        return 0;
    }else{
        _send(command->control_fd, codes[7]);
        return -1; 
    }
    return 0;
}

int handle_cdup(command_t* command){
    if(!command->is_logged_in){
        _send(command->control_fd, codes[0]);
        return -1;
    }
    if(command->number_of_tokens != 1){
        _send(command->control_fd, codes[1]);
        return -1;
    }
    // printf("root directory is %s and cwd is %s\n", command->root_dir, command->cwd);
    char* cwd_parent_dir = change_dir_to_parent(command->cwd);
    if(isParent(cwd_parent_dir, command->root_dir) == 0){ // && chdir("..") == 0
        // printf("changing root directory to %s\n", cwd_parent_dir);
        free(command->cwd);
        command->cwd = cwd_parent_dir;        
        _send(command->control_fd, codes[8]);
        return 0;
    }else{
        _send(command->control_fd, codes[7]);
        free(cwd_parent_dir);
        return -1; 
    }
    return 0;
}

int handle_pasv(command_t* command){
    if(!command->is_logged_in){
        _send(command->control_fd, codes[0]);
        return -1;
    }
    if(command->number_of_tokens != 1){
        _send(command->control_fd, codes[1]);
        return -1;
    }
    if(command->data_fd != -1){
        release_port(command->port_pool, command->data_port);
        close(command->data_fd);
        command->data_fd = -1;
    }

    while(1){
        char* port = allocate_port(command->port_pool);

        command->data_fd = create_socket(port);
        int a, b, c, d;
        sscanf(command->ip, "%d.%d.%d.%d", &a, &b, &c, &d);
        char address_and_port[256];
        sprintf(address_and_port, "227 Entering passive mode (%d,%d,%d,%d,%d)\r\n", a,
                b, c, d, atoi(port));
        if(command->data_fd != -1){
            if (_send(command->control_fd, address_and_port) == -1) {
                command->is_logged_in = 0;
                command->data_fd = -1;
                return -1;
            }
            command->data_port = atoi(port);
            break;
        }
    }
    return 0;
}

int handle_retr(command_t* command){
    if(!command->is_logged_in){
        _send(command->control_fd, codes[0]);
        return -1;
    }
    if(command->number_of_tokens != 2){
        _send(command->control_fd, codes[1]);
        return -1;
    }
    if(command->data_fd == -1){
        _send(command->control_fd, codes[10]);
        return -1;
    }

    concatPaths(&command->cwd, "/");
    concatPaths(&command->cwd, command->tokens[1]);

    // char* path_to_copy = command->tokens[1];
    if(!check_if_file(command->cwd)){
        close(command->data_fd);
        command->data_fd = 0;
        _send(command->control_fd, codes[7]);
        return -1;
    }
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    int fd;
    while(1) {
        // dump_command(command);
        sin_size = sizeof(their_addr);
        fd = accept(command->data_fd, (struct sockaddr *) &their_addr, &sin_size);
        if (fd != -1) {
            break;
        }
    }

    // printf("found a connected client\n");
    if (_send(command->control_fd, codes[12]) == -1) {
        close(fd);
        close(command->data_fd);
        command->data_fd = 0;
        release_port(command->port_pool, command->data_port);
        return -1;
    }

    int is_error = send_file(fd, command->cwd);
    if(is_error == -1){
        _send(command->control_fd, codes[7]);
        close(fd);
        close(command->data_fd);
        release_port(command->port_pool, command->data_port);
        command->data_fd = 0;
    }else if(is_error == -2){
        close(fd);
        close(command->data_fd);
        release_port(command->port_pool, command->data_port);
        command->data_fd = 0;
        _send(command->control_fd, codes[9]);
    }    
    else{
        _send(command->control_fd, codes[11]);
        close(fd);
        close(command->data_fd);
        release_port(command->port_pool, command->data_port);
        command->data_fd = -1;
    }
    return -1;
}

int handle_nlst(command_t* command){
    if(!command->is_logged_in){
        _send(command->control_fd, codes[0]);
        return -1;
    }
    if(command->number_of_tokens != 1){
        _send(command->control_fd, codes[1]);
        return -1;
    }
    if(command->data_fd == -1){
        _send(command->control_fd, codes[10]);
        return -1;
    }
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    int fd;
    while(1) {
        // dump_command(command);
        sin_size = sizeof(their_addr);
        fd = accept(command->data_fd, (struct sockaddr *) &their_addr, &sin_size);
        if (fd != -1) {
            break;
        }
    }
    if (_send(command->control_fd, codes[12]) == -1) {
        close(fd);
        close(command->data_fd);
        command->data_fd = 0;
        release_port(command->port_pool, command->data_port);
        return -1;
    }
    if (listFiles(fd, command->cwd) < 0) {
        if (_send(command->control_fd, codes[9]) == -1) {
            close(fd);
            close(command->data_fd);
            release_port(command->port_pool, command->data_port);
            return -1;
        }
    }else{
        _send(command->control_fd, codes[11]);
        close(fd);
        close(command->data_fd);
        command->data_fd = -1;
        release_port(command->port_pool, command->data_port);
    }
    return 0;
}

command_t* validate_command(char* command_string, command_t* command){
    tokenize_string(command_string, command);
    if(command->number_of_tokens > 0){
        int index = is_command_valid(command->tokens[0]);
        if(index < 0){
            command->is_valid = -1;
            _send(command->control_fd, codes[2]);
        }else{
            command->is_valid = 1;
            command->command_index = index;
        }
    }else{
        _send(command->control_fd, codes[2]);
    }
    // dump_command(command);
    return command;
}



int handle_commands(int fd, const char* cwd, char* ip, port_info* port_pool){
    int data_fd = -1;
    int is_logged_in = 0;
    char* current_dir = strdup(cwd);
    int data_port = -1;
    while(1){
        command_t* command = create_command(is_logged_in, data_fd, current_dir, cwd);
        command->control_fd = fd;
        command->ip = ip;
        command->port_pool = port_pool;
        command->data_port = data_port;
        char client_msg[1024] = {};
        if(recv(command->control_fd, &client_msg, 1024, 0) < 1){
            destroy_command(command);
            free(current_dir);
            printf("releasing data port %d\n", command->data_port);
            release_port(port_pool, command->data_port);
            return -1;
        }
        validate_command(client_msg, command);
        // dump_command(command);
        
        if(command->is_valid == -1){
            destroy_command(command);
            continue;
        }
        int result = (*handlers[command->command_index])(command);
        if(result == -1){
            destroy_command(command);
            continue;
        }else if(result != -1){
            data_fd = command->data_fd;
            is_logged_in = command->is_logged_in;
            data_port = command->data_port;
            free(current_dir);
            current_dir = strdup(command->cwd);
            destroy_command(command);
        }else if(result == -4){
            free(current_dir);
            destroy_command(command);
            break;
        }
    }
}

void destroy_command(command_t* command){
    for(int i = 0;i<10;i++){
        free(command->tokens[i]);
    }
    free(command->tokens);
    free(command->cwd);
    free(command);
}

command_t* create_command(int is_logged_in, int data_fd, char* current_dir, const char* cwd){
    command_t* command = malloc(sizeof(*command));
    command->tokens = malloc(sizeof(char**) * 10);
    for(int i = 0;i<10;i++){
        command->tokens[i] = NULL;
    }
    command->is_valid = -1;
    command->command_index = -1;
    command->control_fd = -1;
    command->is_logged_in = is_logged_in;
    command->data_fd = data_fd;
    command->cwd = strdup(current_dir);
    command->root_dir = cwd;
    command->ip = NULL;
    return command;
}


void dump_command(command_t* command){
    printf("\tindex: %d\tnumber_of_tokens: %d\tis_valid: %d\tcontrol_fd: %d\tdata_fd: %d\n",
     command->command_index, command->number_of_tokens,
      command->is_valid, command->control_fd, command->data_fd);

    for(int i = 0;i<command->number_of_tokens;i++)
        printf("\ttoken(%d): %s\n",i, command->tokens[i]);
}