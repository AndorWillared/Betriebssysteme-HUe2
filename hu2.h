//
// Created by Andor on 25.06.2019.
//

#ifndef HU2_H
#define HU2_H

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>


/*EXIT-CODES*/
#define ERR_WRONG_CALL      1   /*1a) Falscher Aufruf des Programms*/
#define ERR_IMG_NOT_FOUND   2   /*1b) Image-Datei nicht gefunden*/
#define ERR_FILE_IO         3   /*1c) Datei Ein/Ausgabefehler*/
#define ERR_ILL_PART_NUM    4   /*1d) Illegale Partitionsnummer*/
#define ERR_PART_NO_EOS     5   /*1e) Partition enthaelt kein EOS32-FS*/
#define ERR_MALLOC          6   /*1f) Erfolgloser Aufruf von malloc()*/
#define ERR_OTHER           9   /*1g ANDERER FEHLER*/

#define ERR_NOT_FILE_LIST   10  /*2a) Ein Block ist weder in einer Datei noch auf der Freiliste*/
#define ERR_IN_FILE_LIST    11  /*2b) Ein Block ist sowohl in einer Datei als auch auf der Freiliste*/
#define ERR_MULTI_LIST      12  /*2c) Ein Block ist  mehr als einmal in der Freiliste*/
#define ERR_MULTI_FILE      13  /*2d) Ein Block ist mehr als einmal in einer Datei oder in mehr als einer Datei*/
#define ERR_FILE_SIZE       14  /*2e) Die Groesse einer Datei ist nicht konsistent mit den im inode vermerkten Bloecken*/

#define ERR_IN0_IN_DIR      15  /*2f) Ein Inode mit Linkcount 0 erscheint in einem Verzeichnis*/
#define ERR_IN0_NOT_FREE    16  /*2g) Ein Inode mit Linkcount 0 ist nicht frei*/
#define ERR_INN_DIR_COUNT   17  /*2h) Ein Inode mit Linkcount n!=0 erscheint nicht in exakt n Verzeichnissen*/
#define ERR_INN_ILL_TYPE    18  /*2i) Ein Inode hat ein Typfeld mit illegalem Wert*/
#define ERR_INF_IN_DIR      19  /*2j) Ein inode erscheint in einem Verzeichnis, ist aber frei*/
#define ERR_ROOT_NO_DIR     20  /*2k) Der Root-Inode ist kein Verzeichnis*/

#define ERR_DIR_UNRBL       21  /*2l) Ein Verzeichnis kann von der Wurzel aus nicht erreicht werden*/
#define ERR_OTHER_FS        99  /*2m) ANDERER Dateisystem-Fehler*/

/*CONSTANTS*/
#define MAX_INT 32.767
#define SECTOR_SIZE 512
#define NPE (SECTOR_SIZE / sizeof(ParEntry))
#define DESCR_SIZE 20
#define SECTOR_SIZE	512	/* disk sector size in bytes */
#define BLOCK_SIZE	4096	/* disk block size in bytes */
#define SPB		(BLOCK_SIZE / SECTOR_SIZE)

// constants by Geisse

#define SECTOR_SIZE	512	/* disk sector size in bytes */
#define BLOCK_SIZE	4096	/* disk block size in bytes */
#define SPB		(BLOCK_SIZE / SECTOR_SIZE)
#define LINE_SIZE	100	/* input line buffer size in bytes */
#define LINES_PER_BATCH	32	/* number of lines output in one batch */

#define NICINOD		500	/* number of free inodes in superblock */
#define NICFREE		500	/* number of free blocks in superblock */
#define INOPB		64	/* number of inodes per block */
#define DIRPB		64	/* number of directory entries per block */
#define DIRSIZ		60	/* max length of path name component */

#define IFMT		070000	/* type of file */
#define IFREG		040000	/* regular file */
#define IFDIR		030000	/* directory */
#define IFCHR		020000	/* character special */
#define IFBLK		010000	/* block special */
#define IFFREE		000000	/* reserved (indicates free inode) */
#define ISUID		004000	/* set user id on execution */
#define ISGID		002000	/* set group id on execution */
#define ISVTX		001000	/* save swapped text even after use */
#define IUREAD		000400	/* user's read permission */
#define IUWRITE		000200	/* user's write permission */
#define IUEXEC		000100	/* user's execute permission */
#define IGREAD		000040	/* group's read permission */
#define IGWRITE		000020	/* group's write permission */
#define IGEXEC		000010	/* group's execute permission */
#define IOREAD		000004	/* other's read permission */
#define IOWRITE		000002	/* other's write permission */
#define IOEXEC		000001	/* other's execute permission */

// our constants

#define FALSE		0
#define TRUE 		!FALSE

#define REG        1
#define DIR        2
#define SPE        3        //stands for CHR and BLK special files

#define DIRECT    11
#define SINGLE    12
#define DOUBLE    13
#define SUBDIRECT 14

#define B_LIST      21
#define B_FILE      22


/*TYPES*/
typedef struct {
    unsigned int filerefs;
    unsigned int listrefs;
} Block;

typedef struct {
    unsigned int refcount;
} Inode;

#endif //HU2_H
