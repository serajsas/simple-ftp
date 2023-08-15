#ifndef _DIRH__
#define _DIRH__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

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
//function prototypes
int listFiles(int, char*);

void concatPaths(char** str1, char *str2);

int isParent(char* path, const char* root);

char* change_dir_to_parent(char * path);

int check_if_file(char *pathname);

int is_dir(const char *path);

int is_path_valid(char* str);

#endif
