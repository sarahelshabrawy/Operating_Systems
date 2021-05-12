#include "header.h"

int main(int argc, char *argv[])
{
    //register signal
    signal (SIGCHLD, childExit);
    //delete log file
    remove("log.txt");
  while(1){
    //scanning input
    char line[COMMAND_LENGTH];
    scanf("%[^\n]",line);
    getchar();
    char *p = strtok(line, " ");
    char *command[COMMAND_SIZE];
    memset(command, 0, sizeof command);
    int size = 0;
    //split string into tokens with delimitter white space
    while(p) {
    command[size] = malloc(sizeof(p)+1);
    strcpy(command[size],p);
    p = strtok(NULL, " ");
    size++;
    }

    //define signal for waitpid
     int status;
     pid_t childID, endID;
     //flag to decide wheter the process is backgroud or not
     int waitFlag = 0;
    if(strcmp(command[0], "exit") == 0)
       exit(EXIT_SUCCESS);
       //if there is no "&" then we wait
    if(strcmp(command[size-1], "&") != 0){
        waitFlag = 1;
    }
    //else remove "&" from the command and the wait flag is already = 0
    else
      command[size-1] = NULL;
    
     if ((childID = fork()) == -1) {    
        perror("fork error");
        exit(EXIT_FAILURE);
     }
      else if (childID == 0) {            /* This is the child.          */
            executeCommand(argc,argv,command,size);         
          }
       else {                              /*parent process */
            if(waitFlag){
              while ((endID = waitpid(childID, &status,WCONTINUED)) > 0);  
            }         
     }
    
 

  }
    return 0;
}


void childExit(){
  FILE *file;
  file=fopen("log.txt","a");
  fprintf(file, "%s", "Child process was terminated\n");
  fclose(file);
  int status;
  //collect zombie children
while ((waitpid(-1, &status, WNOHANG)) > 0) ;
}
void executeCommand(int argc,char *argv[], char *command[], int size){
    if(strcmp(command[0], "ls") == 0)
        list(argc,argv,command,size);
      else if(strcmp(command[0], "mkdir") == 0)
          makeDirectory(command,size);
      else{
              //command other than the implemented ones
              execvp(command[0],command);
              //if the function returns then an error has occured..print an error
              perror("An error occured");
              exit(EXIT_FAILURE);
        }
}
 void printFilePermissions(struct stat fileInfo){
     printf( (S_ISDIR(fileInfo.st_mode)) ? "d" : "-");
     printf( (fileInfo.st_mode & S_IRUSR) ? "r" : "-");
     printf( (fileInfo.st_mode & S_IWUSR) ? "w" : "-");
     printf( (fileInfo.st_mode & S_IXUSR) ? "x" : "-");
     printf( (fileInfo.st_mode & S_IRGRP) ? "r" : "-");
     printf( (fileInfo.st_mode & S_IWGRP) ? "w" : "-");
     printf( (fileInfo.st_mode & S_IXGRP) ? "x" : "-");
     printf( (fileInfo.st_mode & S_IROTH) ? "r" : "-");
     printf( (fileInfo.st_mode & S_IWOTH) ? "w" : "-");
     printf( (fileInfo.st_mode & S_IXOTH) ? "x " : "- ");
 }

 int isHidden(const char *name)
 {
   return name[0] == '.' || strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
 }

 void formatSize(int  size)
 {                   
     static const char *SIZES[] = { "B", "kB", "MB", "GB" };
     //holds how many multiples of 1024 is the size .. to determine its size unit
     int div = 0;
     int rem = 0;

     while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES)) {
         rem = (size % 1024);
         div++;   
         size /= 1024;
     }

     printf("%.1f %s ", (float)size + (float)rem / 1024.0, SIZES[div]);
 }

 void printFileInfo(struct dirent *namelist,int format){
   struct stat fileInfo;
    if (!stat(namelist->d_name,&fileInfo))
         {
           printFilePermissions(fileInfo);
           struct passwd *pw = getpwuid(fileInfo.st_uid);
           struct group  *gr = getgrgid(fileInfo.st_gid);
           printf("%s %s ",pw->pw_name,gr->gr_name);
           if(format)
           formatSize(fileInfo.st_size);
           else printf("%ld ",fileInfo.st_size);
           char* time = asctime(localtime(&fileInfo.st_mtime));
           //to remove the new line
           time[strlen(time) - 1] = 0;
           printf("%s %s\n",time,namelist->d_name);
           }
 }

 void listFiles(struct dirent **namelist, int size, int modeFlags[]){
   int n = 0;
   while (n<size) {
     //if file details flag is on
     if(modeFlags[2]){
       // if show hidden files flag is off and not hidden file or if show hidden files is on
       if((!modeFlags[0]&&!isHidden(namelist[n]-> d_name)) || modeFlags[0])
         printFileInfo(namelist[n],modeFlags[1]);
     }
     else{
       if((!modeFlags[0]&&!isHidden(namelist[n]-> d_name)) || modeFlags[0])
         printf("%s ",namelist[n]-> d_name); 
     } 
     free(namelist[n]);
     n++;  
   }
   if(!modeFlags[2])
     printf("\n");
 }


 void list(int argc,char *argv[], char *command[], int size){
   //built in struct type for holding directory entries
     struct dirent **namelist;
     int n;

 //checking on length of argv[] to prevent segmentation fault
   if(argc < 1)
     exit(EXIT_FAILURE);  
   else if (argc == 1) 
     n=scandir(".",&namelist,NULL,alphasort);  
    else  
    n = scandir(argv[1], &namelist, NULL, alphasort); // returns number of items in namelist
     //alphasort > sorts files alphabitically
    if(n < 0) {exit(EXIT_FAILURE); } 
    else { 
       //modeFlag[0] is for a mode
       //modeFlag[0] is for h mode
       //modeFlag[0] is for l mode
       int modeFlags[] = {0,0,0};
       if(size > 1){
           int i = 1;
           while(i<size){
               if(strcmp(command[i], "-a") == 0)
                 modeFlags[0] = 1;
               else if(strcmp(command[i], "-h") == 0)
               modeFlags[1] = 1;
               else if(strcmp(command[i], "-l") == 0)
               modeFlags[2] = 1;
               else{
                  perror("Unavailable argument");
                  exit(EXIT_FAILURE);
               }
                 
               i++;
           }
       }
       listFiles(namelist,n,modeFlags);
    
       } 
 }

 void makeDirectory(char *command[], int size){
       if(size == 1){
          perror("Please specify the directory's name\n");
          exit(EXIT_FAILURE);
       }
         
       else if(mkdir(command[1], 0777)==-1){
         perror("Couldn't create folder");
         exit(EXIT_FAILURE);
       }
 }


