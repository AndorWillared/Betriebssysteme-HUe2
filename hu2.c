#include <stdio.h>
#include <stdlib.h>
#include "hu2.h"

/*
 Debug outputs can be deleted: printf("#DEBUG| OUTPUT",VARS)/*Todo: DEBUG*\/*/



int main(int argc, char *argv[]) {
    char* diskName;
    char* partitionNum;
    FILE *disk;

    Block blocks[];
    Inode inodes[];

    /*Exits if number of arguments doesn't equal 3 (progName, fileName, partitionNum)*/
    if(argc!=3) {
        exit(ERR_WRONG_CALL); /*Fehler 2a)*/
    }

    diskName = argv[1];
    char* rest;
    long arg2 = strtol(argv[1], &rest, 10);
    /*Exits if partNum is to big or argv[1] is not an int*/
    if(arg2 >= MAX_INT || rest != '\0') {
        exit(ERR_WRONG_CALL); /*Fehler 2a)*/
    } else {
        partitionNum = arg2;
    }
    printf("#DEBUG| partitionNum: %d",ar)/*Todo: DEBUG*/

    disk = fopen(diskName, "rb");
    if(disk==NULL) {
        exit(ERR_IMG_NOT_FOUND) /*Fehler 2b)*/
    }
    fseek(disk, 1 * SECTOR_SIZE, SEEK_SET);
    if(fread(ptr, 1 SECTOR_SIZE, disk) != SECTOR_SIZE){
        exit(ERR_FILE_IO) /*Fehler 2c)*/
    }
    /*Get count of ParEntries to check if partNum is valid*/
    int i;
    for(i= 0; ptr[i]!=NULL; i++);
    if(partitionNum > i || partitionNum < 0){
        exit(ERR_ILL_PART_NUM) /*Fehler 2d)*/
    }





    fclose(disk);
    return 0;
}

void checkDir(DIR, Data *data){
    /*Checks*/


    /*Recursion*/
    for(SUBDIR : DIR) {
        checkDir(SUBDIR, data);
    }
}