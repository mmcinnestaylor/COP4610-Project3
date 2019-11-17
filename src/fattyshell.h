/*
 *  FAT32 Shell Utility - header
 *  Authors: Keaun Moughari, Hayden Rogers, Marlan McInnes-Taylor
 *  Date:   November 14th, 2019
 * 
 */

#ifndef __FATTY_SHELL_H
#define __FATTY_SHELL_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

static int run = 1;

typedef enum op_t 
{ 
    EXIT,
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

typedef struct cmd_t
{
    char** tokens;
    int size;

} cmd;

typedef struct boot_t
{
    uint8_t BS_jmpBoot[3];
    uint8_t BS_OEMName[8];
    uint8_t BPB_BytsPerSec[2];
    uint8_t BPB_SecPerClus;
    uint8_t BPB_RsvdSecCnt[2];
    uint8_t BPB_NumFATs;
    uint8_t BPB_RootEntCnt[2];
    uint8_t BPB_TotSec16[2];
    uint8_t BPB_Media;
    uint8_t BPB_FATSz16[2];
    uint8_t BPB_SecPerTrk[2];
    uint8_t BPB_NumHeads[2];
    uint8_t BPB_HiddSec[4];
    uint8_t BPB_TotSec32[4];
    uint8_t BS_DrvNum;
    uint8_t BS_Reserved1;
    uint8_t BS_BootSig;
    uint8_t BS_VolID;
    uint8_t BS_VolLab[11];
    uint8_t BS_FilSysType[8];
    uint8_t Signature_word[2];
    uint8_t BPB_FATSz32[4];
    uint8_t BPB_ExtFlags[2];
    uint8_t BPB_FSVer[2];
    uint8_t BPB_RootClus[4];
    uint8_t BPB_FSInfo[2];
    uint8_t BPB_BkBootSec[2];
    uint8_t BPB_Reserved[12];
} __attribute__ ((packed)) boot;

typedef struct fat_t 
{
    uint32_t RootDirSectors;
    uint32_t FATSz;
    uint32_t TotSec;
    uint32_t DataSec;
    uint32_t CountofClusters;

} __attribute__ ((packed)) fat;

typedef struct data_t
{
    char DIR_Name[11];
    uint8_t DIR_Attr[1];
    uint8_t DIR_NTRes[1];
    uint8_t DIR_CrtTimeTenth[1];
    uint8_t DIR_CrtTime[2];
    uint8_t DIR_CrtDate[2];
    uint8_t DIR_LstAccDate[2];
    uint8_t DIR_FstClusHI[2];
    uint8_t DIR_WrtTime[2];
    uint8_t DIR_WrtDate[2];
    uint8_t DIR_FstClusLO[2];
    uint8_t DIR_FileSize[4];

} __attribute__ ((packed)) data;


// init functions
void initFAT(FILE*, boot*);

// fat32 functions
void f_exit();
void f_info(fat* data);
int f_size();
int f_open();
int f_read();
int f_close();


// helper functions
int getChoice(const char*);
void dec2hex(uint8_t*);
void hex2dec(uint8_t*);
void cnvtEndian(uint8_t*);
void printMenu();

// shell command functions
int parseCommand(cmd*, boot*);
void addToken(cmd*, char*);
void addNull(cmd*);
void clearCommand(cmd*);
void printTokens(cmd*);



void initFAT(FILE* fp, boot* f_boot)
{
    if (fp)
    {
        int pos = 0;
        pos = fread(f_boot->BS_jmpBoot, sizeof(uint8_t), 3, fp);

        cnvtEndian(f_boot->BS_jmpBoot);
        hex2dec(f_boot->BS_jmpBoot);
    }

}

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

void dec2hex(uint8_t* dec)
{

}

void hex2dec(uint8_t* hex)
{
    int size = sizeof(hex) / sizeof(uint8_t);

}

void cnvtEndian(uint8_t* x)
{
    int size = sizeof(x) / sizeof(uint8_t);
    uint8_t tmp[size];
    
    int i, j;
    for (i = 0, j = size - 1; i < size; i++, j--)
        tmp[i] = x[j];
    for (i = 0; i < size; i++)
        x[i] = tmp[i];
}

int parseCommand(cmd* instr, boot* f_boot)
{
    if (instr->size == 0)
        return -1;
    switch (getChoice(instr->tokens[0]))
    {
        case EXIT:
            f_exit();
            run = 0;
            break;
        case INFO:
            f_info(f_boot);
            break;
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


    int i;
    for (i = 0; i < instr->size; i++)
    {

    }
}

void addToken(cmd* instr, char* tok)
{
	//extend token array to accomodate an additional token
	if (instr->size == 0)
		instr->tokens = (char**) malloc(sizeof(char*));
	else
		instr->tokens = (char**) realloc(instr->tokens, (instr->size+1) * sizeof(char*));	

	//allocate char array for new token in new slot
	instr->tokens[instr->size] = (char *)malloc((strlen(tok)+1) * sizeof(char));
	strcpy(instr->tokens[instr->size], tok);

	instr->size++;
}

void addNull(cmd* instr)
{
	//extend token array to accomodate an additional token
	if (instr->size == 0)
		instr->tokens = (char**)malloc(sizeof(char*));
	else
		instr->tokens = (char**)realloc(instr->tokens, (instr->size+1) * sizeof(char*));

	instr->tokens[instr->size] = (char*) NULL;
	instr->size++;
}

void printTokens(cmd* instr)
{
	int i;
	printf("Tokens:\n");
	for (i = 0; i < instr->size; i++) {
		if ((instr->tokens)[i] != NULL)
			printf("%s\n", (instr->tokens)[i]);
	}
}

void clearCommand(cmd* instr)
{
	int i;
	for (i = 0; i < instr->size; i++)
		free(instr->tokens[i]);

	free(instr->tokens);

	instr->tokens = NULL;
	instr->size = 0;
}

#endif