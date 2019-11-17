/*
 *  FAT32 Shell Utility - source
 *  Authors: Keaun Moughari, Hayden Rogers, Marlan McInnes-Taylor
 *  Date:   November 14th, 2019
 * 
 */

#include "fattyshell.h"

int main(int argc, char** argv)
{   
    // check for valid argc
    if (argc != 2)
    {
        printf("Correct usage: %s <path>\n", argv[0]);
        return -1;
    }

    // check for valid file ext (.img)
    if (strstr(argv[1], ".img") == NULL)
    {
        printf("Incompatible filetype. Expecting type of .img\n");
        return -1;
    }
    
    // declare structs for fat regions
    boot f_boot;
    fat f_fat;
    data f_data;

    // open filename passed in in read/binary mode, init f_boot info
    FILE *fp = NULL;
    fp = fopen(argv[1], "rb");
    if (fp) 
    {  
        initBoot(fp, &f_boot);
        initFAT(&f_boot, &f_fat);
    }
    else
    {
        printf("Error opening %s\n", argv[1]);
        return -1;
    }

    // beginning of shell
    printf("Fatty Shell\nEnter \"help\" or \"h\" to view available commands\n");
    char *tok = NULL;
    char *tmp = NULL;
    
    // cmd struct to hold each token of instruction
    cmd instr;
    instr.tokens = NULL;
    instr.size = 0;
    
    do
    {
        printf("\\] ");
        
        do
        {
            scanf("%ms", tok);
            tmp = (char*) malloc((strlen(tok) + 1) * sizeof(char));

            int start = 0;
            int i;
            for (i = 0; i < strlen(tok); i++)
            {
                if (tok[i] == ' ')
                {
                    if (i - start > 0)
                    {
                        memcpy(tmp, tok + start, i - start);
                        tmp[i-start] = '\0';

                        addToken(&instr, tmp);
                    }
    
                    start = i + 1;
                }
            }

            if (start < strlen(tok))
            {
                memcpy(tmp, tok + start, strlen(tok) - start);
                tmp[i - start] = '\0';
                
                addToken(&instr, tmp);
            }
            
            addNull(&instr);
            parseCommand(&instr, &f_boot);
            clearCommand(&instr);
            
            free(tok);
            free(tmp);
            tok = NULL;
            tmp = NULL;

        } while (getchar() != '\n');
    } while (run);
    
    return 0;
}
