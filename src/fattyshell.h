/*
 *  FAT32 Shell Utility - header
 *  Authors: Keaun Moughari, Hayden Rogers, Marlan McInnes-Taylor
 *  Date:   November 14th, 2019
 * 
 */

#ifndef _FATTY_SHELL_H
#define _FATTY_SHELL_H

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
#define ATTR_LONG_NAME  (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)
#define EOC             0x0FFFFFF8

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
    uint32_t CountofClusters;
    uint32_t RootDirSectors;
    uint32_t FirstDataSec;
    uint32_t BytsPerSec;
    uint32_t RsvdSecCnt;
    uint32_t SecPerClus;
    uint32_t RootClus;
    uint32_t curClus;
    uint32_t DataSec;
    uint32_t TotSec;
    uint32_t FATSz;
    uint32_t MAX;

} __attribute__ ((packed)) fat;

typedef struct dir_t
{
    uint8_t DIR_Name[11];
    uint8_t DIR_Attr[1];
    uint8_t DIR_NTRes[1];
    uint8_t DIR_FstClusHI[2];
    uint8_t DIR_FstClusLO[2];
    uint8_t DIR_FileSize[4];

} __attribute__ ((packed)) dir;

typedef struct file_node
{
    int fstClus;
    short mode;
    struct file_node* next;
} node;




// init functions
void initBoot(FILE*, boot*);
void initFAT(boot*, fat*);
dir* initDir(FILE*, int);
void loadDir(FILE*, dir*);


// fat32 functions
void f_exit();
void f_info(boot*);
long int f_size(FILE *fp, fat *f_fat, dir *f_dir, cmd *instr);
int f_create(FILE *fp, fat *f_fat, dir *f_dir, cmd *instr);
int f_open(FILE *fp, fat *f_fat, dir *f_dir, cmd *instr, node* openFiles);
int f_read();
int f_close(FILE *fp, fat *f_fat, dir *f_dir, cmd *instr, node* openFiles);
int f_cd(FILE*, fat*, dir*, cmd*);
int f_ls(FILE*, fat*, dir*, cmd*);

// list functions
node* initList();
int add(node* openFiles, const int fstClus, const short mode);
int removeNode(node* openFiles, const int fstClus);
void clear(node* openFiles);


// helper functions
int calcClus(fat*, int);
int calcNext(fat*, int);
int calcFATSecAddr(fat*, int);
int calcFATSecNum(fat*, int);
int calcFATOff(fat*, int);
int getChoice(const char*);
int getEntVal(FILE*, int);
void dec2hex(uint8_t*);
void hex2dec(uint8_t*);
uint32_t arr2val(uint8_t *, int);
void cnvtEndian(uint8_t*, int);
void ascii2dec(uint8_t *);
void printMenu();
int isFile(uint8_t);
int isLast(dir*);
int isEmpty(dir*);
int isLong(dir*);
int isEndOfCluster(FILE*, const int);

// shell command functions
int parseCommand(FILE*, cmd*, boot*, fat*, dir*, node*);
void addToken(cmd*, char*);
void addNull(cmd*);
void clearCommand(cmd*);
void printTokens(cmd*);


/*
 * initialize boot
 *  >   FILE*
 *  >   boot*
 *  ::  void
 *
 *  * initializes boot
 * 
 */
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
        //cnvtEndian(f_boot->BS_OEMName, size);

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
        //cnvtEndian(f_boot->BS_VolLab, size);

        pos += fread(f_boot->BS_FilSysType, sizeof(uint8_t), 8, fp); // 82 - 89
        size = sizeof(f_boot->BS_FilSysType) / sizeof(uint8_t);        
        //cnvtEndian(f_boot->BS_FilSysType, size);

        fseek(fp, 510 - pos, SEEK_CUR);
        fread(f_boot->Signature_word, sizeof(uint8_t), 2, fp);    // 510 - 511
        size = sizeof(f_boot->Signature_word) / sizeof(uint8_t);        
        cnvtEndian(f_boot->Signature_word, size);
    }
}



/*
 * initialize FAT
 *  >   boot*
 *  >   fat*
 *  ::  void
 *
 *  * initializes FAT
 * 
 */
void initFAT(boot* f_boot, fat* f_fat)
{   
    uint32_t BPB_RootEntCnt = arr2val(f_boot->BPB_RootEntCnt, 2);
    uint32_t BPB_NumFATs = arr2val(f_boot->BPB_NumFATs, 1);
    uint32_t FATSz, TotSec;
    
    if (arr2val(f_boot->BPB_FATSz32, 4) != 0)
        FATSz = arr2val(f_boot->BPB_FATSz32, 4);
    else
        FATSz = arr2val(f_boot->BPB_FATSz16, 2);

    if (arr2val(f_boot->BPB_TotSec32, 4) != 0)
        TotSec = arr2val(f_boot->BPB_TotSec32, 4);
    else
        TotSec = arr2val(f_boot->BPB_TotSec16, 2);
    
    f_fat->BytsPerSec = arr2val(f_boot->BPB_BytsPerSec, 2);
    f_fat->RsvdSecCnt = arr2val(f_boot->BPB_RsvdSecCnt, 2);
    f_fat->SecPerClus = arr2val(f_boot->BPB_SecPerClus, 1);
    f_fat->RootDirSectors = ((BPB_RootEntCnt * 32) + (f_fat->BytsPerSec - 1)) / f_fat->BytsPerSec;
    f_fat->FirstDataSec = f_fat->RsvdSecCnt + (BPB_NumFATs * FATSz) + f_fat->RootDirSectors;
    f_fat->DataSec = TotSec - (f_fat->RsvdSecCnt + (BPB_NumFATs * FATSz) + f_fat->RootDirSectors);
    f_fat->CountofClusters = f_fat->DataSec / f_fat->SecPerClus;
    f_fat->MAX = f_fat->CountofClusters + 1;
    //f_fat->DataSec = f_fat->RsvdSecCnt + (BPB_NumFATs * FATSz) + f_fat->RootDirSectors;    // reset to data region size
    f_fat->curClus = f_fat->RootClus = arr2val(f_boot->BPB_RootClus, 4);
}


/*
 * initialize dir
 *  >   FILE*
 *  >   int
 *  ::  dir*
 *
 *  * initializes dir
 * 
 */
dir* initDir(FILE* fp, int n)
{
    if (!fp)
        return NULL;

    int start = ftell(fp);

    //printf("(%d) 0x%08x\n", n, n);
    fseek(fp, n, SEEK_SET);
    dir *f_dir = (dir*)malloc(sizeof(dir));
    loadDir(fp, f_dir);
    fseek(fp, start, SEEK_SET);
    
    /*
    int i;
    for (i = 0; i < 11; i++)
    {
        printf("%c", (char)f_dir->DIR_Name[i]);
    }
    printf("\n");
    printf("0x%02x\n", f_dir->DIR_Attr[0]);
    printf("0x%02x\n", f_dir->DIR_NTRes[0]);
    printf("0x%04x\n", arr2val(f_dir->DIR_FstClusHI, 2));
    printf("0x%04x\n", arr2val(f_dir->DIR_FstClusLO, 2));
    printf("0x%04x\n", arr2val(f_dir->DIR_FstClusHI, 2));
    printf("0x%08x\n", arr2val(f_dir->DIR_FileSize, 4));

    printf("\n");
    */
    return f_dir;
}


/*
 * load dir
 *  >   FILE*
 *  >   dir*
 *  ::  void
 *
 *  * initializes FAT
 * 
 */
void loadDir(FILE* fp, dir* f_dir)
{
    /*
    int i,j;
    char c;
    for (i = 0, j = 0; i < 11; i++, j++)
    {
        if ((c = fgetc(fp)) != 0x0)
            f_dir->DIR_Name[]

       
    }
    */
    fread(f_dir->DIR_Name, sizeof(uint8_t), 11, fp);
    //cnvtEndian((uint8_t*)f_dir->DIR_Name, 11);
    //int i;
    //for (i = 0; i < 11; i++)
    //{
    //    printf("%c", f_dir->DIR_Name[i]);
    //}
    fread(f_dir->DIR_Attr, sizeof(uint8_t), 1, fp);
    fread(f_dir->DIR_NTRes, sizeof(uint8_t), 1, fp);

    fseek(fp, 7, SEEK_CUR);

    fread(f_dir->DIR_FstClusHI, sizeof(uint8_t), 2, fp);
    cnvtEndian(f_dir->DIR_FstClusHI, 2);

    fseek(fp, 4, SEEK_CUR);

    fread(f_dir->DIR_FstClusLO, sizeof(uint8_t), 2, fp);
    cnvtEndian(f_dir->DIR_FstClusLO, 2);

    fread(f_dir->DIR_FileSize, sizeof(uint8_t), 4, fp);
    cnvtEndian(f_dir->DIR_FileSize, 4);
    //printf("(%d) 0x%08x\n", ftell(fp), ftell(fp));
}
//n: cluster; first sector of cluster n
int calcClus(fat* f_fat, int n)
{
    //printf("%d + ((%d - 2) * %d) = %d\n", f_fat->DataSec, n, f_fat->SecPerClus, f_fat->DataSec + ((n - 2) * f_fat->SecPerClus));
    return f_fat->FirstDataSec + ((n - 2) * f_fat->SecPerClus);
}

//n: sector number; addr (bytes) in fat img
int calcFATSecAddr(fat* f_fat, int n)
{  
    return n * (f_fat->BytsPerSec * f_fat->SecPerClus);
}

//n: cluster; sec in entry
int calcFATSecNum(fat* f_fat, int n)
{
    int FATOffset = n * 4;
    return f_fat->RsvdSecCnt + (FATOffset / f_fat->BytsPerSec);
}

//n: cluster
int calcFATOff(fat* f_fat, int n)
{
    int FATOffset = n * 4;
    return FATOffset % f_fat->BytsPerSec;
}

//n: cluster
int calcNext(fat* f_fat, int n)
{
    int thisFATEntSec = calcFATSecNum(f_fat, n);
    int thisFATEntOffset = calcFATOff(f_fat, n);

    //printf("thisFATEntSecNum: %d (0x%08x)\n", thisFATEntSec, thisFATEntSec);
    //printf("thisFATEntOffset: %d (0x%08x)\n", thisFATEntOffset, thisFATEntOffset);
    //printf("nextAddr: %d (0x%08x)\n", calcFATSecAddr(f_fat, thisFATEntSec) + thisFATEntOffset, calcFATSecAddr(f_fat, thisFATEntSec) + thisFATEntOffset);
    return calcFATSecAddr(f_fat, thisFATEntSec) + thisFATEntOffset;
}

/*
 * get ent value
 *  >   FILE*
 *  >   int
 *  ::  int
 *
 *  * gets ent value
 * 
 */
int getEntVal(FILE* fp, int n)
{
    uint8_t data[4];

    if (fp)
    {  
        fseek(fp, n, SEEK_SET);

        fread(data, sizeof(uint8_t), 4, fp);
        cnvtEndian(data, 4);
        printf("%d (0x%08x)\n", arr2val(data, 4), arr2val(data, 4));

        return arr2val(data, 4);
    }
    else
    {
        printf("Error with file pointer in getEntVal\n");
        return -1;
    }   
}


int catClusHILO(dir* f_dir)
{
    return ((f_dir->DIR_FstClusHI[0] << 24) | (f_dir->DIR_FstClusHI[1] << 16) | (f_dir->DIR_FstClusLO[0] << 8) | f_dir->DIR_FstClusLO[1]);
}

/*
 * print menu
 *  ::  void
 *
 *  * prints menu
 * 
 */
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

/*
 * get choice
 *  >   const char*
 *  ::  int
 *
 *  * gets choice
 * 
 */
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


/*
 * convert endian
 *  >   uint8_t*
 *  >   int
 *  ::  void
 *
 *  * converts endian
 * 
 */
void cnvtEndian(uint8_t *x, int size)
{
    uint8_t tmp[size];
    int i, j;
    for (i = 0, j = size - 1; i < size; i++, j--)
        tmp[i] = x[j];
    for (i = 0; i < size; i++)
        x[i] = tmp[i];
}


/*
 * array to value
 *  >   uint8_t*
 *  >   int
 *  ::  void
 *
 *  * array to value
 * 
 */
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

/*
 * is end of cluster ?
 *  >   FILE*
 *  >   const int
 *  ::  int
 *
 *  * returns whether is end of cluster
 * 
 */
int isEndOfCluster(FILE *img, const int nextCluster)
{
    int i;
    
    unsigned char eocMarker1[] = {248, 255, 255, 15};
    unsigned char eocMarker2[] = {240, 255, 255, 15};
    unsigned char clusterInfo[4];

    fseek(img, nextCluster, SEEK_SET);

    for (i = 0; i < 4; i++)
        clusterInfo[i] = fgetc(img);

    for (i = 0; i < 4; i++)
        if (clusterInfo[i] != eocMarker1[i])
            return 0;
    return 1;
}

/***************COMMAND FUNCTIONS***************/

void f_exit() 
{
    run = 0;
}


/*
 * info
 *  >   boot*
 *  ::  void
 *
 *  * prints info
 * 
 */
void f_info(boot* f_boot)
{
    int i;
    printf("BS_jmpBoot: 0x%08x\n", arr2val(f_boot->BS_jmpBoot, 3));
    printf("BS_OEMName: ");
    for (i = 0; i < 8; i++)
        printf("%c", f_boot->BS_OEMName[i]);
    printf("\nBytsPerSec: %d\n", arr2val(f_boot->BPB_BytsPerSec, 2));
    printf("SecPerClus: %d\n", arr2val(f_boot->BPB_SecPerClus, 1));
    printf("RsvdSecCnt: %d\n", arr2val(f_boot->BPB_RsvdSecCnt, 2));
    printf("NumFATs: %d\n", arr2val(f_boot->BPB_NumFATs, 1));
    printf("RootEntCnt: %d\n", arr2val(f_boot->BPB_RootEntCnt, 2));
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
    printf("VolLab: %s\n", (char*)f_boot->BS_VolLab);
    printf("FilSysType: %s\n", (char*)f_boot->BS_FilSysType);
    printf("Sig_word: 0x%08x\n", arr2val(f_boot->Signature_word, 2));
    printf("%c\n", '\0');
}


/*
 * size
 *  >   boot*
 *  >   fat*
 *  >   dir*
 *  >   cmd*
 *  ::  long int
 *
 *  * required size function
 * 
 */
long int f_size(FILE *fp, fat *f_fat, dir *f_dir, cmd *instr)
{
    if (!fp)
        return -2;

    long int size = -1;
    int start = ftell(fp);
    int next = calcFATSecAddr(f_fat, calcClus(f_fat, f_fat->curClus));
    dir *tmp = NULL;
    while ((tmp = initDir(fp, next)) != NULL && isFile(tmp->DIR_Attr[0]))
    {
        if (tmp->DIR_Attr[0] != ATTR_LONG_NAME && strncmp(tmp->DIR_Name, instr->tokens[1], strlen(instr->tokens[1])) == 0)
        {
            size = arr2val(tmp->DIR_FileSize, 4);
            free(tmp);
            return size;
        }
        else
        {
            next += 32;
        }

    }

    if (tmp != NULL)
        free(tmp);
    fseek(fp, start, SEEK_SET);
    return size;
}

/*
 * cd
 *  >   FILE*
 *  >   fat*
 *  >   dir*
 *  >   cmd*
 *  ::  int
 *
 *  * required cd function
 * 
 */
int f_cd(FILE* fp, fat* f_fat, dir* f_dir, cmd* instr)
{
    if (!fp)
        return -2;

    int start = ftell(fp);
    int next = calcFATSecAddr(f_fat, calcClus(f_fat, f_fat->curClus));
    dir* tmp = NULL;

    while ((tmp = initDir(fp, next)) != NULL)
    {   
        if (tmp->DIR_Attr[0] != ATTR_LONG_NAME && strncmp(tmp->DIR_Name, instr->tokens[1], strlen(instr->tokens[1])) == 0)
        {   
            if (tmp->DIR_Attr[0] == ATTR_DIRECTORY)
            {
                f_fat->curClus = next;
                fseek(fp, start, SEEK_SET);
                memcpy((void*)f_dir->DIR_Name, (void*)tmp->DIR_Name, sizeof(uint8_t) * 11);
                memcpy((void*)f_dir->DIR_Attr, (void*)tmp->DIR_Attr, sizeof(uint8_t) * 1);
                memcpy((void*)f_dir->DIR_NTRes, (void*)tmp->DIR_NTRes, sizeof(uint8_t) * 1);
                memcpy((void*)f_dir->DIR_FstClusHI, (void*)tmp->DIR_FstClusHI, sizeof(uint8_t) * 2);
                memcpy((void*)f_dir->DIR_FstClusLO, (void*)tmp->DIR_FstClusLO, sizeof(uint8_t) * 2);
                memcpy((void*)f_dir->DIR_FileSize, (void*)tmp->DIR_FileSize, sizeof(uint8_t) * 4);
                
                free(tmp);
                tmp = NULL;
                return 0;
            }
            else
            {
                free(tmp);
                tmp = NULL; 
                return -1;
            }
        }
        else
        {
            next+=32;
        }   
    }

    fseek(fp, start, SEEK_SET);
    return -1;
}


int f_ls(FILE* fp, fat* f_fat, dir* f_dir, cmd* instr)
{
    if (!fp)
        return -2;

    int start = ftell(fp);
    int next = calcFATSecAddr(f_fat, calcClus(f_fat, f_fat->curClus));
    dir* tmp = NULL;
    
    if (instr->size < 3 || strcmp(instr->tokens[1], ".") == 0)
    {   
        if (f_fat->curClus != f_fat->RootClus)
            printf(".\n..\n");

        while ((tmp = initDir(fp, next)) != NULL && isFile(tmp->DIR_Attr[0]))
        {   
            if (tmp->DIR_Attr[0] != ATTR_LONG_NAME)
                printf("%s\n", (char*)tmp->DIR_Name);

            next+=32;
            free(tmp); 
        }

        return 0;
    }
    else if (strcmp(instr->tokens[1], "..") == 0)
    {
        if (f_fat->curClus == f_fat->RootClus)
            return -3;
        else
        {
            return 0;
        }
        
    }
    else
    {
        while ((tmp = initDir(fp, next)) != NULL && isFile(tmp->DIR_Attr[0]))
        {   
            if (tmp->DIR_Attr[0] != ATTR_LONG_NAME && strncmp(instr->tokens[1], (char*)tmp->DIR_Name, strlen(instr->tokens[1])))
            {    
                
                //printf("%s\n", (char*)tmp->DIR_Name);
            }
            next+=32;
            free(tmp);
        }

        return 0;
    }

    if (tmp != NULL)
        free(tmp);
    
    fseek(fp, start, SEEK_SET);
    return -1;
}


/*
 * open
 *  >   FILE*
 *  >   fat*
 *  >   dir*
 *  >   cmd*
 *  ::  int
 *
 *  * required open function
 * 
 */
int f_open(FILE *fp, fat *f_fat, dir *f_dir, cmd *instr, node* openFiles)
{
    if (!fp)
        return -2;

    int result = -4; //-4 on fail 1 on success
    int fstClus = 0;
    int start = ftell(fp);
    int next = calcFATSecAddr(f_fat, calcClus(f_fat, f_fat->curClus));
    dir *tmp = NULL;
    while ((tmp = initDir(fp, next)) != NULL && isFile(tmp->DIR_Attr[0]))
    {   
        if (instr->size < 4)
            return -3;
        if (tmp->DIR_Attr[0] != ATTR_LONG_NAME && strncmp((char *)tmp->DIR_Name, instr->tokens[1], strlen(instr->tokens[1])) == 0)
        {            
            //not a directory and read only
            if(tmp->DIR_Attr[0] & 0x10 == 0x00 && (tmp->DIR_Attr[0] & 0x01 == 0x01)){
                fstClus = catClusHILO(tmp);
                //selected more is not r
                if(strcmp(instr->tokens[2], "r") != 0)
                    return -1;
                else{
                    result = add(openFiles, fstClus, 1);
                    break;
                }
            }
            //not a directory and not read only
            else if(!(tmp->DIR_Attr[0] & 0x10 == 0x00)){
                fstClus = catClusHILO(tmp);
                //read
                if(strcmp(instr->tokens[2], "r") == 0) {
                    result = add(openFiles, fstClus, 1);
                    break;
                }
                //write
                else if(strcmp(instr->tokens[2], "w") == 0) {
                    result = add(openFiles, fstClus, 2);
                    break;
                }
                //read and write
                else if(strcmp(instr->tokens[2], "wr") == 0 || strcmp(instr->tokens[2], "rw") == 0) {
                    result = add(openFiles, fstClus, 3);
                    break;
                }
            }
            else
            {
               return -3;
            }
                       
        }
        else
        {
            next += 32;
            free(tmp);
        }
    }


    if (tmp != NULL)
        free(tmp);

    fseek(fp, start, SEEK_SET);
    return result;
}

int f_close(FILE *fp, fat *f_fat, dir *f_dir, cmd *instr, node* openFiles)
{
    if (!fp)
        return -2;

    int result = -1; //-1 on file dne 0 not open 1 on success
    int fstClus = 0;
    int start = ftell(fp);
    int next = calcFATSecAddr(f_fat, calcClus(f_fat, f_fat->curClus));
    dir *tmp = NULL;
    while ((tmp = initDir(fp, next)) != NULL && isFile(tmp->DIR_Attr[0]))
    {
        //if (instr->size < 4)
        //    return -3;
        if (tmp->DIR_Attr[0] != ATTR_LONG_NAME && strncmp(tmp->DIR_Name, instr->tokens[1], strlen(instr->tokens[1])) == 0)
        {
            if (strcmp(tmp->DIR_Name, instr->tokens[1]) == 0)
            {
                //not a directory
                if(tmp->DIR_Attr[0] & 0x10 != 0x00){
                    fstClus = catClusHILO(tmp);
                    result = removeNode(openFiles, fstClus);
                }                
            }            
        }
        else
        {
            next += 32;
        }
        free(tmp);
    }

    if (tmp != NULL)
        free(tmp);

    fseek(fp, start, SEEK_SET);
    return result;
}
/***************LIST FUNCTIONS***************/

node * initList()
{
    node* head = (node*)malloc(sizeof(node));
    node* tail = (node*)malloc(sizeof(node));
    tail->fstClus = 0;
    tail->mode = 0;
    tail->next = NULL;

    head->fstClus = 0;
    head->mode = 0;
    head->next = tail;

    return head;
}

int add(node* listHead, const int clusNum, const short fMode)
{
    node* temp = listHead->next;

    //find end of list
    while(temp->next->next != NULL) {
        if(temp->fstClus == clusNum) //file already open
            return 0;
        temp = temp->next;
    }

    node* newFile = (node*)malloc(sizeof(node));
    newFile->fstClus = clusNum;
    newFile->mode = fMode;

    newFile->next = temp->next;
    temp->next = newFile;

    return 1; //successful add
}

int removeNode(node* listHead, const int clusNum)
{
    node* current = listHead->next;
    node* prev = listHead;

    //find end of list
    while(current->next != NULL){
        //found open file in list
        if(current->fstClus == clusNum){
            prev->next = current->next;
            free(current);
            return 1;
        }

        prev = current;
        current = current->next;
    }

    return 0;
}

void clear(node* listHead)
{
    node* temp = NULL;

    while(listHead->next != NULL){
        temp = listHead->next;
        free(listHead);
        listHead = temp;
    }

    free(listHead);
}

/***************HELPER FUNCTIONS***************/


/*
 * parse command
 *  >   FILE*
 *  >   fat*
 *  >   dir*
 *  >   cmd*
 *  >   node*
 *  ::  long int
 *
 *  * parses command
 * 
 */
int parseCommand(FILE* fp, cmd* instr, boot* f_boot, fat* f_fat, dir* f_dir, node* openFiles)
{
    if (instr->size == 0)
        return -1;
    
    long int n;
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
            break;
        case CLOSE: 
            n = f_close(fp, f_fat, f_dir, instr, openFiles);
            if (n == -2)
                printf("Error with file pointer.\n");
            else if (n == -1)
                printf("Error: %s does not exist or is a directory.\n", instr->tokens[1]);
            else if (n == 0)
                printf("Error: %s is not open.\n", instr->tokens[1]);
            else
                printf("%s is closed.\n", instr->tokens[1]);                
            break;
        case RM:
            break;
        case OPEN:
            n = f_open(fp, f_fat, f_dir, instr, openFiles);
            if (n == -2)
                printf("Error with file pointer.\n");
            if (n == -1)
                printf("Error: %s is read only and mode is %s.\n", instr->tokens[1], instr->tokens[2]);
            else if (n == 0)
                printf("%s: Already open.\n", instr->tokens[1]);
            else if (n == -3)
                printf("Invalid usage.\n");
            else if (n == -4)
                printf("%s: Does not exist.\n", instr->tokens[1]);
            else
                printf("%s: is open.\n", instr->tokens[1]);
            break;
        case WRITE: 
            break;
        case READ:
            break;
        case SIZE:
            n = f_size(fp, f_fat, f_dir, instr);
            if (n == -1)
                printf("%s: Does not exist.\n", instr->tokens[1]);
            else if (n == -2)
                printf("Error with file pointer.\n");
            else
                printf("Size of %s is: %d\n", instr->tokens[1], n);
            break;
        case LS:
            n = f_ls(fp, f_fat, f_dir, instr);
            if (n == -1)
                printf("%s: Doesn't exist\n", instr->tokens[1]);
            else if (n == -2)
                printf("Problem with file pointer\n");
            else if (n == -3)
                printf("..: Root doesn't have parent.\n");
            break;
        case CD:
            n = f_cd(fp, f_fat, f_dir, instr);
            if (n == -1)
                printf("%s: Does not exist or is not a directory.\n", instr->tokens[1]); 
            else if (n == -2)
                printf("Error with file pointer.\n");
            else
                printf("CWD is now: %s\n", instr->tokens[1]);
            break;
        case MKDIR:
            break;
        case RMDIR:
            break;
        case ERROR:
            printf("Invalid menu option. Enter \"help\" or \"h\" to view available commands\n");
            break;
        case -1:
            printf("tok was null\n");
            break;
    }
}


/*
 * add token
 *  >   cmd*
 *  >   char*
 *  ::  long int
 *
 *  * adds a tok
 * 
 */
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


int isFile(uint8_t x)
{
    if (x == ATTR_DIRECTORY)        return 1;
    else if (x == ATTR_HIDDEN)      return 1;
    else if (x == ATTR_ARCHIVE)     return 1;
    else if (x == ATTR_LONG_NAME)   return 1;
    else if (x == ATTR_READ_ONLY)   return 1;
    else if (x == ATTR_SYSTEM)      return 1;
    else if (x == ATTR_VOLUME_ID)   return 1;
    else                            return 0;
}



int isLast(dir* dir)
{
    // if (littleEndian(dir->DIR_Name, 1) == 0)
    //     return 1;
    // else
    //     return 0;
    return 0;
}


int isEmpty(dir* dir)
{
    if( (int)dir->DIR_Name[0] == 229 )
        return 1;
    else
        return 0;
}


int isLong(dir* dir)
{
    unsigned char dirAttrib = dir->DIR_Attr[0];
    unsigned char mask = 15;

    if((dirAttrib & mask) == 15)
        return 1;
    else 
        return 0;
}


#endif