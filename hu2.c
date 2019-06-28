#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include "hu2.h"

/*
 Debug outputs can be deleted: printf("#DEBUG| OUTPUT\n",VARS);/*Todo: DEBUG*\/*/

unsigned int fsStart; /* file system start sector *//*Todo: Why global, maybe later?*/

Block blocks[];
Inode inodes[];

unsigned int freeblks;	/*num of free blocks, set in superBlock check -> verify after check over*/
unsigned int freeinos; 		/*num of free inodes, set: superBlock -> verify: later*/
unsigned int numberOfEntriesinFreeinos;		/*num of entries in free inode list, set: superBlock -> verfy: later*/

void readBlock(FILE *disk,
               unsigned int blockNum,
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

/*
void checkDir(DIR){
    for(SUBDIR : DIR) {
        checkDir(SUBDIR, data);
    }
}
*/
checkFreeBlock(unsigned char *p) {

    unsigned int free;
    unsigned int nfree;

    if(p == NULL) return;
    for (int i = 0; i < NICFREE; i++) {
        free = get4Bytes(p);
        p += 4;
        if (i < nfree) {
            blocks[free].listrefs +=1;
            printf("  %s block[%3d] = %u (0x%X)\n",
                   i == 0 ? "link" : "free", i, free, free);
            if(i == 0) {
                checkFreeBlock(*((char**)free));
            }

        }
    }
}

void checkSuperBlock(FILE *disk){
    unsigned char blockBuffer[BLOCK_SIZE];
    readBlock(disk, 1, blockBuffer); /*writes superblock to blockBuffer*/

    /*geklaut*/
    unsigned char *p = blockBuffer;
    unsigned int free;
    unsigned int nfree;

    unsigned int fsize;
    unsigned int isize;
    unsigned int freeblks;
    unsigned int freeinos;
    unsigned int ninode;
    unsigned int ino;

    int tim;
    char *dat;
    unsigned char flag;

    /*magicNum = get4Bytes(p);
    /*Todo: kann man die magic number überprüfen?*/
    p += 4; /*skip magic number */
    fsize = get4Bytes(p);
    printf("file system size = %u (0x%X) blocks\n", fsize, fsize);

    p += 4;
    isize = get4Bytes(p);
    printf("inode list size  = %u (0x%X) blocks\n", isize, isize);

    p += 4;
    freeblks = get4Bytes(p);
    printf("free blocks = %u (0x%X)\n", freeblks, freeblks);

    p += 4;
    freeinos = get4Bytes(p);
    printf("free inodes = %u (0x%X)\n", freeinos, freeinos);

    p += 4;
    ninode = get4Bytes(p);
    printf("number of entries in free inode list = %u (0x%X)\n", ninode, ninode);

    p += 4;
    for (int i = 0; i < NICINOD; i++) {
        ino = get4Bytes(p);
        p += 4;
        if (i < ninode) {
            printf("  free inode[%3d] = %u (0x%X)\n", i, ino, ino);
        }
    }

    nfree = get4Bytes(p);
    printf("number of entries in free block list = %u (0x%X)\n", nfree, nfree);

    p += 4;
    checkFreeBlock(p);






    tim = get4Bytes(p);
    p += 4;
    dat = ctime((time_t *) &tim);
    dat[strlen(dat) - 1] = '\0';
    printf("last super block update = %d (%s)\n", tim, dat);

    flag = *p++;
    printf("free list locked flag = %d\n", (int) flag);

    flag = *p++;
    printf("inode list locked flag = %d\n", (int) flag);

    flag = *p++;
    printf("super block modified flag = %d\n", (int) flag);

    flag = *p++;
    printf("fs mounted read-only flag = %d\n", (int) flag);
    /*geklaut ende*/
}

int main(int argc, char *argv[]) {
    char* diskName;
    unsigned int partitionNum;
    FILE *disk;

    unsigned char *ptptr;
    unsigned int fsSize;
    unsigned int partType;
    unsigned int numBlocks;
    unsigned int currBlock;
    unsigned char blockBuffer[BLOCK_SIZE];



    /*Exits if number of arguments doesn't equal 3 (progName, fileName, partitionNum)*/
    if(argc!=3) {
        printf("idiot1\n");/*Todo*/
        exit(ERR_WRONG_CALL); /*Fehler 2a)*/
    }

    diskName = argv[1];
    disk = fopen(diskName, "rb");
    if(disk == NULL) {
        printf("idiot2\n");/*Todo*/
        exit(ERR_IMG_NOT_FOUND); /*Fehler 2b)*/
    }

    printf("hallo ich lebe");
    fseek(disk, 1 * SECTOR_SIZE, SEEK_SET);
    if(fread(ptr, 1, SECTOR_SIZE, disk) != SECTOR_SIZE){
        printf("idiot3\n");/*Todo*/
        exit(ERR_FILE_IO); /*Fehler 2c)*/
    }
    /*Exits if partNum is to big or argv[1] is not an int*/
    char *rest;
    long arg2 = strtoul(argv[1], &rest, 10);
    if(partitionNum > 15 || partitionNum < 0 || *rest != '\0'){
        printf("idiot4\n");/*Todo*/
        exit(ERR_ILL_PART_NUM); /*Fehler 2d)*/
    } else {
        partitionNum = arg2;
    }
    printf("#DEBUG| partitionNum: %d\n",arg2);/*Todo: DEBUG*/

    ptptr = ptr + (partitionNum * 32);
    partType = get4Bytes(ptptr + 0);
    if ((partType & 0x7FFFFFFF) != 0x00000058) {
        printf("idiot5\n");/*Todo*/
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


    /*Super block check*/
    checkSuperBlock(disk);





    fclose(disk);
    return 0;
}