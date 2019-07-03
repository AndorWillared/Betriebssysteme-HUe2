//
// Created by Andor on 25.06.2019.
//

#ifndef HU2_H
#define HU2_H

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


#define NICFREE		500	/* number of free blocks in superblock */
#define NICINOD		500	/* number of free inodes in superblock */

#define FALSE		0
#define TRUE 		!FALSE


/*TYPES*/
typedef struct {
    unsigned int filerefs;
    unsigned int listrefs;
} Block;

typedef struct {
    unsigned int refcount;
} Inode;

#endif //HU2_H
