#include <stdio.h>
#include <stdlib.h>
#include "hu2.h"

int main(int argc, char *argv[]) {
    char* diskName;
    char* partitionName;
    FILE *disk;

    Block blocks[];
    Inode inodes[];

    /*Exits if number of arguments doesn't equal 3 (progName, fileName, partitionName)*/
    if(argc!=3) {
        exit(ERR_WRONG_CALL); /*Fehler a)*/
    }

    diskName = argv[1];
    partitionName = argv[2];

    disk = fopen(diskName, "rb");
    if(disk==NULL) {
        exit(ERR_IMG_NOT_FOUND) /*Fehler b)*/
    }
    fseek(disk, 1 * SECTOR_SIZE, SEEK_SET);
    if(fread(ptr, 1 SECTOR_SIZE, disk) != SECTOR_SIZE){
        exit(ERR_FILE_IO) /*Fehler c)*/
    }










    fclose(disk);
    return 0;
}