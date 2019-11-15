#include <stdlib.h>
#include <stdio.h>
#include "fattyshell.h"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Correct usage: %s <path>\n", argv[0]);
        return -1;
    }

    if (strstr(argv[1], ".img") == NULL)
    {
        printf("Incompatible filetype. Expecting type of .img\n");
        return -1;
    }
    
    int run = 1;
    char op;
    char *cmd = NULL;
    char *tmp = NULL;
    printf("Fatty Shell\nEnter \"help\" or \"h\" to view available commands\n");
    
    do
    {
        printf("\] ");
        scanf("%ms", cmd);
        tmp = (char*) malloc((strlen(cmd) + 1) * sizeof(char));
        strcpy(tmp, cmd);

        char *tok = NULL;
        tok = strtok(tmp, " \n");
        while (tok != NULL)
        {
            switch (getChoice(tok))
            {
                case EXIT:
                    f_exit();
                    run = 0;
                    break;
                case INFO:
                case CREATE: 
                case CLOSE: 
                case RM:
                case OPEN:
                case WRITE: 
                case READ:
                case SIZE:
                case LS:
                case CD: 
                case MKDIR: 
                case RMDIR:
                case ERROR:
                    printf("Invalid menu option. Enter \"help\" or \"h\" to view available commands\n");
                    break;
                case -1:
                    printf("tok was null\n");
                    break;
            }
        }

        free(cmd);
        free(tmp);
        cmd = NULL;
        tmp = NULL;

    } while (run);

    return 0;
}
