//
// Created by Andor on 25.06.2019.
//

#ifndef HÜ2_HU2_H
#define HÜ2_HU2_H

#define SECTOR_SIZE 512
#define NPE (SECTOR_SIZE / sizeof(PartEntry))
#define DESCR_SIZE;

typedef struct {
    unsigned int type;
    unsigned int start;
    unsigned int size;
    char descr[DESCR_SIZE];
} ParEntry;

ParEntry ptr[NPE];

#endif //HÜ2_HU2_H
