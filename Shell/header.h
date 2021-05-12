#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define COMMAND_SIZE 10
#define COMMAND_LENGTH 50

#include <unistd.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

void printFilePermissions(struct stat fileInfo);
int isHidden(const char *name);
void formatSize(int  size);
void printFileInfo(struct dirent *namelist,int format);
void listFiles(struct dirent **namelist, int size, int modeFlags[]);
void list(int argc,char *argv[], char *command[], int size);
void makeDirectory(char *command[], int size);
void childExit();
void executeCommand(int argc,char *argv[], char *command[], int size);