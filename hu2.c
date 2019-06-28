#include <stdio.h>
#include <stdlib.h>
#include "hu2.h"

/*
 Debug outputs can be deleted: printf("#DEBUG| OUTPUT\n",VARS)/*Todo: DEBUG*\/*/

unsigned int fsStart; /* file system start sector *//*Todo: Why global, maybe later?*/

int main(int argc, char *argv[]) {
    char* diskName;
    char* partitionNum;
    FILE *disk;

    unsigned int fsSize;
    unsigned int partType;
    EOS32_daddr_t numBlocks;
    EOS32_daddr_t currBlock;
    unsigned char blockBuffer[BLOCK_SIZE];

    Block blocks[];
    Inode inodes[];

    /*Exits if number of arguments doesn't equal 3 (progName, fileName, partitionNum)*/
    if(argc!=3) {
        exit(ERR_WRONG_CALL); /*Fehler 2a)*/
    }

    diskName = argv[1];
    disk = fopen(diskName, "rb");
    if(disk == NULL) {
        exit(ERR_IMG_NOT_FOUND) /*Fehler 2b)*/
    }

    fseek(disk, 1 * SECTOR_SIZE, SEEK_SET);
    if(fread(ptr, 1 SECTOR_SIZE, disk) != SECTOR_SIZE){
        exit(ERR_FILE_IO) /*Fehler 2c)*/
    }
    /*Get count of ParEntries to check if partNum is valid*/
    int i;
    for(i= 0; ptr[i]!=NULL; i++);

    /*Exits if partNum is to big or argv[1] is not an int*/
    char *rest;
    long arg2 = strtuol(argv[1], &rest, 10);
    if(partitionNum > i || partitionNum < 0 || *rest != '\0'){
        exit(ERR_ILL_PART_NUM) /*Fehler 2d)*/
    } else {
        partitionNum = arg2;
    }
    printf("#DEBUG| partitionNum: %d\n",ar)/*Todo: DEBUG*/

    ptptr = ptr + partitionNum * 32;
    partType = get4Bytes(ptptr + 0);
    if ((partType & 0x7FFFFFFF) != 0x00000058) {
        exit(ERR_PART_NO_EOS); /*Fehler 2e)*/
    }

    fsStart = get4Bytes(ptptr + 4);
    fsSize = get4Bytes(ptptr + 8);

    if (fsSize % SPB != 0) {
        printf("#DEBUG| File system size is not a multiple of block size.\n"); /*Todo: DEBUG*/
        exit(ERR_OTHER_FS); /*Fehler 1m)*/
    }
    numBlocks = fsSize / SPB;
    if (numBlocks < 2) {
        printf("#DEBUG| File system has less than 2 blocks.\n"); /*Todo: DEBUG*/
        exit(ERR_OTHER_FS); /*Fehler 1m)*/
    }

    currBlock = 1;
    readBlock(disk, currBlock, blockBuffer);






    fclose(disk);
    return 0;
}

void readBlock(FILE *disk,
               EOS32_daddr_t blockNum,
               unsigned char *blockBuffer) {
    fseek(disk, fsStart * SECTOR_SIZE + blockNum * BLOCK_SIZE, SEEK_SET);
    if (fread(blockBuffer, BLOCK_SIZE, 1, disk) != 1) {
        printf("#DEBUG| cannot read block %lu (0x%lX)\n", blockNum, blockNum); /*Todo: DEBUG*/
        exit(ERR_OTHER_FS); /*Fehler 1m)*/
    }
}

unsigned int get4Bytes(unsigned char *addr) {
    return (unsigned int) addr[0] << 24 |
           (unsigned int) addr[1] << 16 |
           (unsigned int) addr[2] <<  8 |
           (unsigned int) addr[3] <<  0;
}


void checkDir(DIR, Data *data){
    /*Checks*/


    /*Recursion*/
    for(SUBDIR : DIR) {
        checkDir(SUBDIR, data);
    }
}