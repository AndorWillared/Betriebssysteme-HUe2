//
// Created by Andor on 25.06.2019.
//

#ifndef HÜ2_HU2_H
#define HÜ2_HU2_H

/*EXIT-CODES*/
/*Todo: Fehlerfälle als kommentare einfügen!!!*/
#define ERR_WRONG_CALL      1   /**/
#define ERR_IMG_NOT_FOUND   2   /**/
#define ERR_FILE_IO         3   /**/
#define ERR_ILL_PART_NUM    4   /**/
#define ERR_PART_NO_EOS     5   /**/
#define ERR_MALLOC          6   /**/
#define ERR_OTHER           9   /**/

#define ERR_NOT_FILE_LIST   10  /**/
#define ERR_IN_FILE_LIST    11  /**/
#define ERR_MULTI_LIST      12  /**/
#define ERR_MULTI_FILE      13  /**/
#define ERR_FILE_SIZE       14  /**/

#define ERR_IN0_IN_DIR      15  /**/
#define ERR_IN0_NOT_FREE    16  /**/
#define ERR_INN_DIR_COUNT   17  /**/
#define ERR_INN_ILL_TYPE    18  /**/
#define ERR_INF_IN_DIR      19  /**/
#define ERR_ROOT_NO_DIR     20  /**/

#define ERR_DIR_UNRBL       21  /**/
#define ERR_OTHER_FS        99  /**/

/*CONSTANTS*/
#define SECTOR_SIZE 512
#define NPE (SECTOR_SIZE / sizeof(PartEntry))
#define DESCR_SIZE;


/*TYPES*/
typedef struct {
    unsigned int filerefs;
    unsigned int listrefs;
} Block;

typedef struct {
    unsigned int refcount;
} Inode;

typedef struct {
    unsigned int type;
    unsigned int start;
    unsigned int size;
    char descr[DESCR_SIZE];
} ParEntry;

ParEntry ptr[NPE];

#endif //HÜ2_HU2_H
