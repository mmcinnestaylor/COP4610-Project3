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
    dir *f_dir = NULL;

    // open filename passed in in read/binary mode, init f_boot info
    FILE *fp = NULL;
    fp = fopen(argv[1], "rb");
    if (fp) 
    {  
        initBoot(fp, &f_boot);
        initFAT(&f_boot, &f_fat);
        printf("Clus N: %d\n", f_fat.curClus);
        int i = calcClus(&f_fat, f_fat.curClus);
        printf("Index: %d (0x%08x)\n", i, i);
        printf("Addr: %d (0x%08x)\n", calcFATSecAddr(&f_fat, i), calcFATSecAddr(&f_fat, i));

        f_dir = initDir(fp, calcFATSecAddr(&f_fat, f_fat.curClus));
        if (f_dir != NULL)
            printf("%d (0x%08x)\n", catClusHILO(f_dir), catClusHILO(f_dir));
        
    }
    else
    {
        printf("Error opening %s\n", argv[1]);
        return -1;
    }
    //create linked list of open files
    node* openFiles = initList();
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
            if (!scanf("%ms", &tok))
                continue;

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

            free(tok);
            free(tmp);
            tok = NULL;
            tmp = NULL;

        } while ('\n' != getchar());

        addNull(&instr);
        parseCommand(fp, &instr, &f_boot, &f_fat, f_dir);

        //printTokens(&instr);
        clearCommand(&instr);

    } while (run);

<<<<<<< HEAD
    free(f_dir);
=======
    clear(openFiles);
>>>>>>> 9d1f5399983d4abe0bc116acf17f0ca1aa2f930c
    fclose(fp);
    return 0;
}
