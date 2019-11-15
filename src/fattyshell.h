#include <stdint.h>
#include <string.h>

typedef enum op_t { EXIT,
                 INFO, 
                 CREATE, 
                 CLOSE, 
                 RM, 
                 OPEN, 
                 WRITE, 
                 READ, 
                 SIZE, 
                 LS, 
                 CD, 
                 MKDIR, 
                 RMDIR,
                 ERROR
} op;

typedef struct fat_t 
{
    


} fat;

void printMenu();
int getChoice(const char*);
uint8_t* cnvtEndian(uint8_t*);
void f_exit();
void f_info();
int f_size();
int f_open();


void printMenu()
{
    printf("creat FILENAME\n");
    printf("open FILENAME\n");
    printf("close FILENAME\n");
    printf("rm FILENAME\n");
    printf("write FILENAME OFFSET SIZE STRING\n");
    printf("read FILENAME OFFSET SIZE\n");
    printf("size FILENAME\n");    
    printf("ls DIRNAME\n");
    printf("cd DIRNAME\n");
    printf("mkdir DIRNAME\n");
    printf("rmdir DIRNAME\n");
    printf("info\n");
    printf("exit\n");
}

int getChoice(const char* tok)
{
    if (tok == NULL)                        return -1;
    if (strcmp(tok, "exit") == 0)           return EXIT;
    else if (strcmp(tok, "info") == 0)      return INFO;
    else if (strcmp(tok, "size") == 0)      return SIZE;
    else if (strcmp(tok, "creat") == 0)     return CREATE;
    else if (strcmp(tok, "open") == 0)      return OPEN;
    else if (strcmp(tok, "read") == 0)      return READ;
    else if (strcmp(tok, "close") == 0)     return CLOSE;
    else if (strcmp(tok, "write") == 0)     return WRITE;
    else if (strcmp(tok, "rm") == 0)        return RM;
    else if (strcmp(tok, "ls") == 0)        return LS;
    else if (strcmp(tok, "cd") == 0)        return CD;
    else if (strcmp(tok, "mkdir") == 0)     return MKDIR;
    else if (strcmp(tok, "rmdir") == 0)     return RMDIR;
    else                                    return ERROR;
}
