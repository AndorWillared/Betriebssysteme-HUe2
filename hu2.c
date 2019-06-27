#include <stdio.h>
#include <stdlib.h>
#include "hu2.h"

int main(int argc, char *argv[]) {
    char* diskName;
    char* partitionName;
    FILE *disk;

    /*Exits if number of arguments doesn't equal 3 (progName, fileName, partitionName)*/
    if(argc!=3) {
        error("wrong number of arguments");
    }

    diskName = argv[1];
    partitionName = argv[2];

    disk = fopen(diskName, "rb");
    if(disk==NULL) {
        error("cannot open disk image file '%s'", diskName);
    }
    fseek(disk, 1 * SECTOR_SIZE, SEEK_SET);
    if(fread(ptr, 1 SECTOR_SIZE, disk) != SECTOR_SIZE){
        error("cannot read partition table from disk image '%s'", diskName);
    }









    fclose(disk);
    return 0;
}