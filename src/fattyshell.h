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

#define SCNx8 "hhx"

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
    ERROR,
    HELP

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
    uint8_t BPB_SecPerClus[1];
    uint8_t BPB_RsvdSecCnt[2];
    uint8_t BPB_NumFATs[1];
    uint8_t BPB_RootEntCnt[2];
    uint8_t BPB_TotSec16[2];
    uint8_t BPB_Media[1];
    uint8_t BPB_FATSz16[2];
    uint8_t BPB_SecPerTrk[2];
    uint8_t BPB_NumHeads[2];
    uint8_t BPB_HiddSec[4];
    uint8_t BPB_TotSec32[4];
    uint8_t BPB_FATSz32[4];
    uint8_t BPB_ExtFlags[2];
    uint8_t BPB_FSVer[2];
    uint8_t BPB_RootClus[4];
    uint8_t BPB_FSInfo[2];
    uint8_t BPB_BkBootSec[2];
    uint8_t BPB_Reserved[12];
    uint8_t BS_DrvNum[1];
    uint8_t BS_Reserved1[1];
    uint8_t BS_BootSig[1];
    uint8_t BS_VolID[4];
    uint8_t BS_VolLab[11];
    uint8_t BS_FilSysType[8];
    uint8_t Signature_word[2];

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
void initBoot(FILE*, boot*);
void initFAT(boot*, fat*);
void initData();

// fat32 functions
void f_exit();
void f_info(boot*);
int f_size();
int f_open();
int f_read();
int f_close();


// helper functions
int getChoice(const char*);
void dec2hex(uint8_t*);
void hex2dec(uint8_t*);
uint32_t arr2val(uint8_t *, int);
void cnvtEndian(uint8_t*, int);
void ascii2dec(uint8_t *);
void printMenu();
int isLast(data_t*);
int isEmpty(data_t*);
int isLong(data_t*);

// shell command functions
int parseCommand(cmd*, boot*);
void addToken(cmd*, char*);
void addNull(cmd*);
void clearCommand(cmd*);
void printTokens(cmd*);



void initBoot(FILE* fp, boot* f_boot)
{
    if (fp)
    {
        int size = 0;
        size_t pos = 0;
        
        pos = fread(f_boot->BS_jmpBoot, sizeof(uint8_t), 3, fp);    // 0 - 2
        size = sizeof(f_boot->BS_jmpBoot) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BS_jmpBoot, size);

        pos += fread(f_boot->BS_OEMName, sizeof(uint8_t), 8, fp);    // 3 - 10
        size = sizeof(f_boot->BS_OEMName) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BS_OEMName, size);

        pos += fread(f_boot->BPB_BytsPerSec, sizeof(uint8_t), 2, fp);    // 11 - 12
        size = sizeof(f_boot->BPB_BytsPerSec) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_BytsPerSec, size);

        pos += fread(f_boot->BPB_SecPerClus, sizeof(uint8_t), 1, fp);    // 13
        size = sizeof(f_boot->BPB_SecPerClus) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_SecPerClus, size);

        pos += fread(f_boot->BPB_RsvdSecCnt, sizeof(uint8_t), 2, fp);    // 14 - 15
        size = sizeof(f_boot->BPB_RsvdSecCnt) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_RsvdSecCnt, size);

        pos += fread(f_boot->BPB_NumFATs, sizeof(uint8_t), 1, fp);   // 16 
        size = sizeof(f_boot->BPB_NumFATs) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_NumFATs, size);

        pos += fread(f_boot->BPB_RootEntCnt, sizeof(uint8_t), 2, fp);    // 17 - 18
        size = sizeof(f_boot->BPB_RootEntCnt) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_RootEntCnt, size);

        pos += fread(f_boot->BPB_TotSec16, sizeof(uint8_t), 2, fp);  // 19 - 20
        size = sizeof(f_boot->BPB_TotSec16) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_TotSec16, size);

        pos += fread(f_boot->BPB_Media, sizeof(uint8_t), 1, fp); // 21
        size = sizeof(f_boot->BPB_Media) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_Media, size);

        pos += fread(f_boot->BPB_FATSz16, sizeof(uint8_t), 2, fp);   // 22 - 23
        size = sizeof(f_boot->BPB_FATSz16) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_FATSz16, size);

        pos += fread(f_boot->BPB_SecPerTrk, sizeof(uint8_t), 2, fp); // 24 - 25
        size = sizeof(f_boot->BPB_SecPerTrk) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_SecPerTrk, size);

        pos += fread(f_boot->BPB_NumHeads, sizeof(uint8_t), 2, fp);  // 26 - 27
        size = sizeof(f_boot->BPB_NumHeads) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_NumHeads, size);

        pos += fread(f_boot->BPB_HiddSec, sizeof(uint8_t), 4, fp);   // 28 - 31
        size = sizeof(f_boot->BPB_HiddSec) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_HiddSec, size);

        pos += fread(f_boot->BPB_TotSec32, sizeof(uint8_t), 4, fp);  // 32 - 35
        size = sizeof(f_boot->BPB_TotSec32) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_TotSec32, size);

        pos += fread(f_boot->BPB_FATSz32, sizeof(uint8_t), 4, fp);   // 36 - 39
        size = sizeof(f_boot->BPB_FATSz32) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_FATSz32, size);

        pos += fread(f_boot->BPB_ExtFlags, sizeof(uint8_t), 2, fp);  // 40 - 41 
        size = sizeof(f_boot->BPB_ExtFlags) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_ExtFlags, size);

        pos += fread(f_boot->BPB_FSVer, sizeof(uint8_t), 2, fp); // 42 - 43
        size = sizeof(f_boot->BPB_FSVer) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_FSVer, size);

        pos += fread(f_boot->BPB_RootClus, sizeof(uint8_t), 4, fp);  // 44 - 47
        size = sizeof(f_boot->BPB_RootClus) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_RootClus, size);

        pos += fread(f_boot->BPB_FSInfo, sizeof(uint8_t), 2, fp);    // 48 - 49
        size = sizeof(f_boot->BPB_FSInfo) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_FSInfo, size);

        pos += fread(f_boot->BPB_BkBootSec, sizeof(uint8_t), 2, fp); // 50 - 51
        size = sizeof(f_boot->BPB_BkBootSec) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_BkBootSec, size);

        pos += fread(f_boot->BPB_Reserved, sizeof(uint8_t), 12, fp); // 52 - 63
        size = sizeof(f_boot->BPB_Reserved) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BPB_Reserved, size);

        pos += fread(f_boot->BS_DrvNum, sizeof(uint8_t), 1, fp); // 64
        size = sizeof(f_boot->BS_DrvNum) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BS_DrvNum, size);

        pos += fread(f_boot->BS_Reserved1, sizeof(uint8_t), 1, fp);  // 65
        size = sizeof(f_boot->BS_Reserved1) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BS_Reserved1, size);

        pos += fread(f_boot->BS_BootSig, sizeof(uint8_t), 1, fp);    // 66
        size = sizeof(f_boot->BS_BootSig) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BS_BootSig, size);

        pos += fread(f_boot->BS_VolID, sizeof(uint8_t), 4, fp);  // 67 - 70
        size = sizeof(f_boot->BS_VolID) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BS_VolID, size);

        pos += fread(f_boot->BS_VolLab, sizeof(uint8_t), 11, fp);    // 71 - 81
        size = sizeof(f_boot->BS_VolLab) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BS_VolLab, size);

        pos += fread(f_boot->BS_FilSysType, sizeof(uint8_t), 8, fp); // 82 - 89
        size = sizeof(f_boot->BS_FilSysType) / sizeof(uint8_t);        
        cnvtEndian(f_boot->BS_FilSysType, size);

        fseek(fp, 510 - pos, SEEK_CUR);
        fread(f_boot->Signature_word, sizeof(uint8_t), 2, fp);    // 510 - 511
        size = sizeof(f_boot->Signature_word) / sizeof(uint8_t);        
        cnvtEndian(f_boot->Signature_word, size);
        
        /*
        int i;
        printf("BS_jmpBoot: 0x%08x\n", arr2val(f_boot->BS_jmpBoot, 3));
        for (i = 0; i < 8; i++)
            printf("BS_OEMName: 0x%02x\n", f_boot->BS_OEMName[i]);
        printf("BytsPerSec: 0x%08x\n", arr2val(f_boot->BPB_BytsPerSec, 2));
        printf("SecPerClus: 0x%08x\n", arr2val(f_boot->BPB_SecPerClus, 1));
        printf("RsvdSecCnt: 0x%08x\n", arr2val(f_boot->BPB_RsvdSecCnt, 2));
        printf("NumFATs: 0x%08x\n", arr2val(f_boot->BPB_NumFATs, 1));
        printf("RootEntCnt: 0x%08x\n", arr2val(f_boot->BPB_RootEntCnt, 2));
        printf("TotSec16: 0x%08x\n", arr2val(f_boot->BPB_TotSec16, 2));
        printf("Media: 0x%08x\n", arr2val(f_boot->BPB_Media, 1));
        printf("FATSz16: 0x%08x\n", arr2val(f_boot->BPB_FATSz16, 2));
        printf("SecPerTrk: 0x%08x\n", arr2val(f_boot->BPB_SecPerTrk, 2));
        printf("NumHeads: 0x%08x\n", arr2val(f_boot->BPB_NumHeads, 2));
        printf("HiddSec: 0x%08x\n", arr2val(f_boot->BPB_HiddSec, 4));
        printf("TotSec32: 0x%08x\n", arr2val(f_boot->BPB_TotSec32, 4));
        printf("FATSz32: 0x%08x\n", arr2val(f_boot->BPB_FATSz32, 4));
        printf("ExtFlags: 0x%08x\n", arr2val(f_boot->BPB_ExtFlags, 2));
        printf("FSVer: 0x%08x\n", arr2val(f_boot->BPB_FSVer, 2));
        printf("RootClus: 0x%08x\n", arr2val(f_boot->BPB_RootClus, 4));
        printf("FSInfo: 0x%08x\n", arr2val(f_boot->BPB_FSInfo, 2));
        printf("BkBootSec: 0x%08x\n", arr2val(f_boot->BPB_BkBootSec, 2));
        for (i = 0; i < 12; i++)
            printf("Reserved: 0x%02x\n", f_boot->BPB_Reserved[i]);
        printf("DrvNum: 0x%08x\n", arr2val(f_boot->BS_DrvNum, 1));
        printf("Reserved1: 0x%08x\n", arr2val(f_boot->BS_Reserved1, 1));
        printf("BootSig: 0x%08x\n", arr2val(f_boot->BS_BootSig, 1));
        printf("VolID: 0x%08x\n", arr2val(f_boot->BS_VolID, 4));
        for (i = 0; i < 11; i++)
            printf("VolLab: 0x%02x\n", f_boot->BS_VolLab[i]);
        for (i = 0; i < 8; i++)
            printf("FilSysType: 0x%02x\n", f_boot->BS_FilSysType[i]);
        printf("Sig_word: 0x%08x\n", arr2val(f_boot->Signature_word, 2));
        */
    }

}

void initFAT(boot* f_boot, fat* f_fat)
{   
    uint32_t BPB_RootEntCnt = arr2val(f_boot->BPB_RootEntCnt, 2);
    uint32_t BPB_BytsPerSec = arr2val(f_boot->BPB_BytsPerSec, 2);
    uint32_t BPB_ResvdSecCnt = arr2val(f_boot->BPB_RsvdSecCnt, 2);
    uint32_t BPB_NumFATs = arr2val(f_boot->BPB_NumFATs, 1);
    uint32_t BPB_SecPerClus = arr2val(f_boot->BPB_SecPerClus, 1);
    uint32_t FATSz, TotSec;
    
    if (arr2val(f_boot->BPB_FATSz32, 4) != 0)
        FATSz = arr2val(f_boot->BPB_FATSz32, 4);
    else
        FATSz = arr2val(f_boot->BPB_FATSz16, 2);

    if (arr2val(f_boot->BPB_TotSec32, 4) != 0)
        TotSec = arr2val(f_boot->BPB_TotSec32, 4);
    else
        TotSec = arr2val(f_boot->BPB_TotSec16, 2);

    f_fat->RootDirSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec - 1)) / BPB_BytsPerSec;
    f_fat->DataSec = TotSec - (BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + f_fat->RootDirSectors);
    f_fat->CountofClusters = f_fat->DataSec / BPB_SecPerClus;
}

void printMenu()
{   
    printf("\nValid Options:\n");
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
    printf("exit\n\n");
}

int getChoice(const char* tok)
{
    if (tok == NULL)                        return -1;
    if (strcmp(tok, "exit") == 0)           return EXIT;
    else if (strcmp(tok, "h") == 0)         return HELP;
    else if (strcmp(tok, "help") == 0)      return HELP;
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

//converts ascii values to actual decimal values
void ascii2dec(uint8_t * arr)
{
    int i, size = sizeof(arr) / sizeof(uint8_t);

    for(i = 0; i < size; i++){
        if(arr[i] >= 48 && arr[i] <= 57)
            arr[i] -= 48;
        else if(arr[i] >= 65 && arr[i] <= 70)
            arr[i] -= 55;  
    }
}
void cnvtEndian(uint8_t *x, int size)
{
    uint8_t tmp[size];
    int i, j;
    for (i = 0, j = size - 1; i < size; i++, j--)
        tmp[i] = x[j];
    for (i = 0; i < size; i++)
        x[i] = tmp[i];
}

uint32_t arr2val(uint8_t *x, int size)
{
    uint32_t tmp = 0;
    if (size == 1)
        tmp = x[0];
    else if (size == 2)
        tmp = 0x0 | (x[0] << 8) | x[1];
    else if (size == 3)
        tmp = 0x0 | (x[0] << 16) | (x[1] << 8) | x[2];
    else if (size == 4)
        tmp = (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];

    return tmp;
}

void f_exit() 
{
    run = 0;
}

void f_info(boot* f_boot)
{
    int i;
    printf("BS_jmpBoot: 0x%08x\n", arr2val(f_boot->BS_jmpBoot, 3));
    for (i = 0; i < 8; i++)
        printf("BS_OEMName: 0x%02x\n", f_boot->BS_OEMName[i]);
    printf("BytsPerSec: 0x%08x\n", arr2val(f_boot->BPB_BytsPerSec, 2));
    printf("SecPerClus: 0x%08x\n", arr2val(f_boot->BPB_SecPerClus, 1));
    printf("RsvdSecCnt: 0x%08x\n", arr2val(f_boot->BPB_RsvdSecCnt, 2));
    printf("NumFATs: 0x%08x\n", arr2val(f_boot->BPB_NumFATs, 1));
    printf("RootEntCnt: 0x%08x\n", arr2val(f_boot->BPB_RootEntCnt, 2));
    printf("TotSec16: 0x%08x\n", arr2val(f_boot->BPB_TotSec16, 2));
    printf("Media: 0x%08x\n", arr2val(f_boot->BPB_Media, 1));
    printf("FATSz16: 0x%08x\n", arr2val(f_boot->BPB_FATSz16, 2));
    printf("SecPerTrk: 0x%08x\n", arr2val(f_boot->BPB_SecPerTrk, 2));
    printf("NumHeads: 0x%08x\n", arr2val(f_boot->BPB_NumHeads, 2));
    printf("HiddSec: 0x%08x\n", arr2val(f_boot->BPB_HiddSec, 4));
    printf("TotSec32: 0x%08x\n", arr2val(f_boot->BPB_TotSec32, 4));
    printf("FATSz32: 0x%08x\n", arr2val(f_boot->BPB_FATSz32, 4));
    printf("ExtFlags: 0x%08x\n", arr2val(f_boot->BPB_ExtFlags, 2));
    printf("FSVer: 0x%08x\n", arr2val(f_boot->BPB_FSVer, 2));
    printf("RootClus: 0x%08x\n", arr2val(f_boot->BPB_RootClus, 4));
    printf("FSInfo: 0x%08x\n", arr2val(f_boot->BPB_FSInfo, 2));
    printf("BkBootSec: 0x%08x\n", arr2val(f_boot->BPB_BkBootSec, 2));
    for (i = 0; i < 12; i++)
        printf("Reserved: 0x%02x\n", f_boot->BPB_Reserved[i]);
    printf("DrvNum: 0x%08x\n", arr2val(f_boot->BS_DrvNum, 1));
    printf("Reserved1: 0x%08x\n", arr2val(f_boot->BS_Reserved1, 1));
    printf("BootSig: 0x%08x\n", arr2val(f_boot->BS_BootSig, 1));
    printf("VolID: 0x%08x\n", arr2val(f_boot->BS_VolID, 4));
    for (i = 0; i < 11; i++)
        printf("VolLab: 0x%02x\n", f_boot->BS_VolLab[i]);
    for (i = 0; i < 8; i++)
        printf("FilSysType: 0x%02x\n", f_boot->BS_FilSysType[i]);
    printf("Sig_word: 0x%08x\n", arr2val(f_boot->Signature_word, 2));
}

int parseCommand(cmd* instr, boot* f_boot)
{
    if (instr->size == 0)
        return -1;
    switch (getChoice(instr->tokens[0]))
    {
        case HELP:
            printMenu();
            break;
        case EXIT:
            f_exit();
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



int isLast(data_t* dir)
{
    return 0;
}


int isEmpty(data_t* dir)
{
    return 0;
}


int isLong(data_t* dir)
{
    return 0;
}

#endif