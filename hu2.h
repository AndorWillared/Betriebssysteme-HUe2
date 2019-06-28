//
// Created by Andor on 25.06.2019.
//

#ifndef HU2_H
#define HU2_H

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
#define MAX_INT 32.767
#define SECTOR_SIZE 512
#define NPE (SECTOR_SIZE / sizeof(ParEntry))
#define DESCR_SIZE 20
#define SECTOR_SIZE	512	/* disk sector size in bytes */
#define BLOCK_SIZE	4096	/* disk block size in bytes */
#define SPB		(BLOCK_SIZE / SECTOR_SIZE)


#define NICFREE		500	/* number of free blocks in superblock */
#define NICINOD		500	/* number of free inodes in superblock */

/*TYPES*/
typedef struct {
    unsigned int filerefs;
    unsigned int listrefs;
} Block;

typedef struct {
    unsigned int refcount;
} Inode;

#endif //HU2_H
