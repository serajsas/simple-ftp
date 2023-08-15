#include "dir.h"

/* 
   Arguments: 
      fd - a valid open file descriptor. This is not checked for validity
           or for errors with it is used.
      directory - a pointer to a null terminated string that names a 
                  directory

   Returns
      -1 the named directory does not exist or you don't have permission
         to read it.
      -2 insufficient resources to perform request

 
   This function takes the name of a directory and lists all the regular
   files and directoies in the directory. 
 

 */

int listFiles(int fd, char * directory) {

  // Get resources to see if the directory can be opened for reading
  
  DIR * dir = NULL;
  
  dir = opendir(directory);
  if (!dir) return -1;
  
  // Setup to read the directory. When printing the directory
  // only print regular files and directories. 

  struct dirent *dirEntry;
  int entriesPrinted = 0;
  
  for (dirEntry = readdir(dir);
       dirEntry != NULL;
       dirEntry = readdir(dir)) {
    if (dirEntry->d_type == DT_REG) {  // Regular file
      struct stat buf = {};

      // This call really should check the return value
      // stat returns a structure with the properties of a file
      stat(dirEntry->d_name, &buf);
      
      /* 
          I modified the dprintf statement because it was resulting in bare linefeeds received
       */
      dprintf(fd, "F    %-20s     %ld\r\n", dirEntry->d_name, buf.st_size);
    } else if (dirEntry->d_type == DT_DIR) { // Directory
      dprintf(fd, "D        %s\r\n", dirEntry->d_name);
    } else {
      dprintf(fd, "U        %s\r\n", dirEntry->d_name);
    }
    entriesPrinted++;
  }
  
  // Release resources
  closedir(dir);
  return entriesPrinted;
}

void concatPaths(char **path1, char *path2) {
    int len1 = strlen(*path1);
    int len2 = strlen(path2);

    // Make sure there is enough space in path1 to hold both paths
    *path1 = realloc(*path1, len1 + len2 + 1);
    if (*path1 == NULL) {
    printf("Error: failed to allocate memory for path1!\n");
    return;
    }

    // Concatenate path2 to the end of path1
    strcat(*path1, path2);
}

int isParent(char* path, const char* root) 
{ 
    // Length of both the Paths 
    int rootlen = strlen(root); 
    int pathlen = strlen(path); 
  
    // If length of root directory is greater than path 
    // then return false as root directory cannot be a 
    // parent of path 
    if (rootlen > pathlen) 
        return -1; 
  
    // Compare string till the length of root directory 
    for (int i = 0; i < rootlen; i++) 
    { 
        if (path[i] != root[i]) 
            return -1; 
    } 
  
    // Return true if all characters are matched 
    return 0; 
} 

char* change_dir_to_parent(char * path)
{
    //parent directory is one level above the given path
    char parent_dir[1024];
    int i = 0;
    for (i = 0; path[i] != 0; i++);
    for(i=i-1; i>= 0 && path[i] != '/'; i--);
    int j = 0;
    for (j=0; j<i; j++)
        parent_dir[j] = path[j];
    parent_dir[j] = '\0';
    return strdup(parent_dir);
}

int is_dir(const char *path) {
  struct stat path_stat = {};
  if (stat(path, &path_stat) != 0) {
    return 0;
  }
  return S_ISDIR(path_stat.st_mode);
}

int check_if_file(char *pathname) {
    struct stat path_stat = {};
    stat(pathname, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int is_path_valid(char* str) 
{ 
    int len = strlen(str);
    if(len >= 2){
        if (str[0] == '.' && str[1] == '/') 
            return 0; 
        if (str[0] == '.' && str[1] == '.' && str[2] == '/') 
            return 0; 
        if(len == 2){
            if (str[0] == '.' && str[1] == '.') 
                return 0; 
        }
    }else if(len == 1){
        if (str[0] == '.') 
            return 0; 
    }
    for (int i = 0; i < len - 2; i++){ 
        if (str[i] == '.' && str[i + 1] == '.' && str[i + 2] == '/') 
            return 0; 
        if(str[i] == '.' && str[i + 1] == '/')
            return 0;
    }
    return 1; 
}

   
