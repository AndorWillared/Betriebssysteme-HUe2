//special files auch noch überprüfen, nur erster block ist KEIN "echter" Block

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>
#include "hu2.h"

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

#define REG        1
#define DIR        2
#define CHR        3
#define BLK        4

#define DIRECT    11
#define SINGLE    12
#define DOUBLE    13
#define SUBDIRECT 14

#define B_LIST      21
#define B_FILE      22

/*ERROR Functions*/
void fatalError(int err_code, char * err_msg);
void fatalErrorIf(int err_condition, int err_code, char *err_msg);

/*DEBUG Functions*/
void debugPrintBlocks(void);

/*HELPER Functions*/
void readBlock(FILE *disk, unsigned int blockNum, unsigned char *blockBuffer);
unsigned int get4Bytes(unsigned char *ptr);

/*Rest*/
void checkFreeBlock(unsigned int addr);
void checkSuperBlock(unsigned char *p);
void checkBlock(unsigned int addr, int type, int flag);
void checkInode(unsigned char *p, unsigned int addr, int doRecursion);
void incrementBlock(unsigned int addr, int type);
  
/* Debug outputs can be deleted: printf("#DEBUG| OUTPUT\n",VARS);/*Todo: DEBUG*\/*/

unsigned int fsStart; /* file system start sector *//*Todo: Why global, maybe later?*/

Block * blocks;
unsigned int numBlocks;
Inode * inodes;
unsigned int numInodes;

unsigned int freeblks;	/*num of free blocks, set in superBlock check -> verify after check over*/
unsigned int freeinos; 	/*num of free inodes, set: superBlock -> verify: later*/
unsigned int numberOfEntriesinFreeinos;	/*num of entries in free inode list, set: superBlock -> verfy: later*/

FILE *disk;	           /*the disk to check*/

void fatalError(int err_code, char * err_msg) {
  	printf("#ERROR| %d  -> \"%s\" \n", err_code, err_msg);
    exit(err_code);
}

void fatalErrorIf(int err_condition, int err_code, char *err_msg){
    if(err_condition){
        fatalError(err_code, err_msg);
    }
}

/*nur zum testen*/
void debugPrintBlocks(void){
  printf("#DEBUG| Prints all used Blocks in blocks-array %d\n",numBlocks);/*Todo: DEBUG*/
  for(int i = 0; i < numBlocks; i++){
    Block *x = &blocks[i]; //Todo: Block *x = &blocks[i]; OR Block x = blocks[i]; ?   	
    if(x->filerefs > 0 || x->listrefs > 0){
      printf("#DEBUG| Block[%d] -> (%d , %d) \n", i, blocks[i].filerefs, blocks[i].listrefs);/*Todo: DEBUG*/
    }
  }
}

void readBlock(FILE *disk, unsigned int blockNum, unsigned char *blockBuffer) {
    fseek(disk, fsStart * SECTOR_SIZE + blockNum * BLOCK_SIZE, SEEK_SET);
  	fatalErrorIf(fread(blockBuffer, BLOCK_SIZE, 1, disk) != 1,ERR_OTHER_FS,"Cannot read block!");  
}

unsigned int get4Bytes(unsigned char *ptr) { 
    return (unsigned int) ptr[0] << 24 |
           (unsigned int) ptr[1] << 16 |
           (unsigned int) ptr[2] <<  8 |
           (unsigned int) ptr[3] <<  0;
}


void checkFreeBlock(unsigned int addr) {
    unsigned int nfree;
    unsigned char blockBuffer[BLOCK_SIZE];
    int i;
  	unsigned char *p;
  
  	readBlock(disk, addr, blockBuffer);
  	p = blockBuffer;
  
    nfree = get4Bytes(p);
    p += 4;
    
    printf("#DEBUG| nfree = %u (0x%X)\n", nfree, nfree);/*Todo: DEBUG*/
		
      for (i = 0; i < NICFREE; i++) { 
        addr = get4Bytes(p);
        p += 4;
        if (i < nfree && addr != 0) {      
            incrementBlock(addr,B_LIST);
            Block *x = &blocks[addr];
            printf("#DEBUG| 2. x.listrefs = %d \n", x->listrefs);/*Todo: DEBUG*/
            printf("#DEBUG| %s block[%3d] = %u (0x%X)\n", i == 0 ? "link" : "free", i, addr, addr);/*Todo: DEBUG*/
           	if(i == 0) {
                checkFreeBlock(addr);    //TODO: errrrr
            }
        }
    }
  		/*hier kann doch einfach i < nfree stehen und dann ...
   		for (i = 0; i < nfree; i++) { 
        	addr = get4Bytes(p);
        	p += 4;
        	printf("#DEBUG| %s block[%3d] = %u (0x%X)\n",
            	i == 0 ? "link" : "free", i, addr, addr);
      		if(i == 0 && addr != 0) {
          		checkFreeBlock(addr, disk);    //TODO: errrrr
        	}
      }*/
}

void checkSuperBlock(unsigned char *p){  
    unsigned int nfree;
    unsigned int free;

    unsigned int fsize;
    unsigned int isize;
    unsigned int freeblks;
    unsigned int freeinos;
    unsigned int ninode;
    unsigned int ino;

    /*magicNum = get4Bytes(p);
    /*Todo: kann man die magic number überprüfen?*/
    p += 4; /*skip magic number */
    fsize = get4Bytes(p); //printf("#DEBUG| file system size = %u (0x%X) blocks\n", fsize, fsize);/*Todo: DEBUG*/
    
    /*Initialisation of blocks*/
    if(fsize!=numBlocks){printf("#DEBUG| wie ist das denn möglich??? fsize sollte numBlocks entsprechen!!"); /*Todo: DEBUG*/}
    
  	numBlocks = fsize;
	blocks = (Block *) malloc(numBlocks * sizeof(Block));
    fatalErrorIf(!blocks,ERR_MALLOC,"Insufficient memory at block allocation");
    
    p += 4;
    isize = get4Bytes(p); //printf("#DEBUG| inode list size  = %u (0x%X) blocks\n", isize, isize); /*Todo: DEBUG*/

  	/*Initialisation of inodes*/
    numInodes = isize;
  	inodes = (Inode *) malloc(numInodes * sizeof(Inode));//Todo: We should read numInodes from the inode-table later!
  	fatalErrorIf(!inodes,ERR_MALLOC,"Insufficient memory at inode allocation");
    
    
    p += 12;
    ninode = get4Bytes(p); //printf("#DEBUG|entries in free inode list = %u (0x%X)\n", ninode, ninode);/*Todo: DEBUG*/
    p += 4;
    for (int i = 0; i < NICINOD; i++) {
        ino = get4Bytes(p);
        p += 4;
        if (i < ninode) { //printf("#DEBUG| free inode[%3d] = %u (0x%X)\n", i, ino, ino);/*Todo: DEBUG*/
        }
    }

  	/*Todo kann das nich auch in die checkFreeBlock methode? V*/
    nfree = get4Bytes(p); //printf("#DEBUG| number of entries in free block list = %u (0x%X)\n", nfree, nfree);/*Todo: DEBUG*/
    p += 4;
    for (int i = 0; i < NICFREE; i++) {
        free = get4Bytes(p);
        p += 4;
        if (i < nfree) {
            printf("#DEBUG| %s block[%3d] = %u (0x%X)\n",
                   i == 0 ? "link" : "free", i, free, free);/*Todo: DEBUG*/
          	if(i == 0) {
                checkFreeBlock(free);    //TODO: errrrr
          	}
      	}
    }
 	/*Bis hier?^*/
}

void checkBlock(unsigned int addr, int type, int flag){
  	if(addr == 0){ printf("     - is not used!\n"); return;}
    unsigned char blockBuffer[BLOCK_SIZE];
    unsigned int ino;
    unsigned char * p;
    unsigned char * ino_p;
	
  	incrementBlock(addr,B_FILE);		//increment file-counter of "old" address

  	readBlock(disk, addr, blockBuffer);
  
	p = blockBuffer; 
  
    if(type==DOUBLE) {
      	printf("Checking double indirect blocks \n");
      	for (int i = 0; i < BLOCK_SIZE/4; i++) {
          	checkBlock(get4Bytes(p), SINGLE, flag); 
          	p+=4;
        }
    } else if(type==SINGLE) {
        printf("Checking single indirect blocks \n");      
      	for (int i = 0; i < BLOCK_SIZE/4; i++) {
          	checkBlock(get4Bytes(p), DIRECT, flag); 
          	p+=4;
        }
    } else if(type==DIRECT) {
        printf("Checking direct blocks \n");
        
        if(flag==DIR) {
          
          	for (int i = 0; i < DIRPB; i++) {
                unsigned char ino_blockBuffer[BLOCK_SIZE];
              	ino = get4Bytes(p);
              	
              	readBlock(disk, 2+ino/INOPB, ino_blockBuffer);
              	ino_p = ino_blockBuffer;
              	ino_p += (ino%INOPB)*64;
              	p += 4;
                checkInode(ino_p, ino, i>2);
              	//todo: müssen wir p hier auch nochmal erhöhen oder reicht das in der checkinode methode?
              	p += DIRSIZ;
            }
        }
    }
}
                     
void checkInode(unsigned char *p, unsigned int inID, int doRecursion){
    unsigned int mode;
  	unsigned int addr;
  	int type;
    mode = get4Bytes(p);
 
  	
  	p += 32; //skip mode//nlink//uid//gid//tim//tim//tim//size
	
  	if((mode & IFMT) == IFREG) {
      	printf("#DEBUG| inode is REG\n");//Todo: DEBUG
      	type = REG;
      	Inode *x = &inodes[inID];
        x->refcount +=1;
    } else if((mode & IFMT) == IFDIR) {
      	printf("#DEBUG| inode is DIR\n");//Todo: DEBUG
      	type = DIR;
      	Inode *x = &inodes[inID];
        x->refcount +=1;
    } else {
      	printf("#DEBUG| inode is neither REG nor DIR\n");//Todo: DEBUG
    }
  	
  	if(doRecursion) {
      	//Direct blocks
        for (int j = 0; j < 6; j++) {
          addr = get4Bytes(p);
          p += 4;
          if (mode != 0) {
              if((mode & IFMT) == IFDIR || (mode & IFMT) == IFREG) checkBlock(addr, DIRECT, type);
            }
        }
      
      	//Single indirect block
        addr = get4Bytes(p);
        p += 4;
        if (mode != 0) {
          if((mode & IFMT) == IFDIR || (mode & IFMT) == IFREG){
              printf("Checking single indirect !!!!! \n");
              checkBlock(addr, SINGLE, type);
              
          } 
        }
      
      	//Double indirect block
        addr = get4Bytes(p);
        p += 4;
        if (mode != 0) {
          if((mode & IFMT) == IFDIR || (mode & IFMT) == IFREG){
              printf("Checking double indirect !!!!! \n");
              checkBlock(addr, DOUBLE, type);
          } 
        }
  	}
}

//increments the File or List field of specified block

void incrementBlock(unsigned int addr, int type){
	Block *x =  &blocks[addr]; 

   	switch(type) {
       case B_FILE: 
            x->filerefs +=1;
            break;
       case B_LIST: 
            x->listrefs +=1;
            break;
       default: printf("what da fuck is dis?\n"); 
                exit(999);  // FEHLER
   	}
}



/*geklaut ende*/

int main(int argc, char *argv[]) {
    char* diskName;
    unsigned int partitionNum;

    unsigned char partTable[SECTOR_SIZE];
    unsigned char *ptptr;
    unsigned int fsSize;
    unsigned int partType;
    unsigned int numBlocks;
  
    unsigned char blockBuffer[BLOCK_SIZE];
  	unsigned int mode;
    unsigned char * p;
  
    /*Exits if number of arguments doesn't equal 3 (progName, fileName, partitionNum)*/
  	fatalErrorIf(argc!=3,ERR_WRONG_CALL, "Fehler 2a");

    diskName = argv[1];
    disk = fopen(diskName, "rb");
    
  	fatalErrorIf(disk==NULL,ERR_IMG_NOT_FOUND, "Fehler 2b");
  	
    fseek(disk, 1 * SECTOR_SIZE, SEEK_SET);
    
  	fatalErrorIf(fread(partTable, 1, SECTOR_SIZE, disk) != SECTOR_SIZE,ERR_FILE_IO, "Fehler 2c");
      
    /*Exits if partNum is to big or argv[1] is not an int*/
    char *rest;
    long arg2 = strtoul(argv[2], &rest, 10);
    if(arg2 > 15 || arg2 < 0 || *rest != '\0'){
        fatalError(ERR_ILL_PART_NUM, "Fehler 2d");
    } else {
        partitionNum = arg2;
    }
    printf("#DEBUG| partitionNum: %ld\n",arg2);/*Todo: DEBUG*/

    ptptr = partTable + partitionNum * 32;
    partType = get4Bytes(ptptr + 0);
    /*todo: check if partitionEntry is empty*/
		
  	fatalErrorIf(((partType & 0x7FFFFFFF) != 0x00000058), ERR_PART_NO_EOS, "Fehler 2e"); 
  
    fsStart = get4Bytes(ptptr + 4);
    
  	fsSize = get4Bytes(ptptr + 8);
	fatalErrorIf(fsSize % SPB != 0,ERR_OTHER_FS, "Fehler 1m - File system size is not a multiple of block size.");   
    
  	numBlocks = fsSize / SPB;
  	fatalErrorIf(numBlocks <2, ERR_OTHER_FS, "Fehler 1m - File system has less than 2 blocks.");
  
    /*Super block check*/
    readBlock(disk, 1, blockBuffer); /*writes superblock to blockBuffer*/
    checkSuperBlock(blockBuffer);
  	/*Super block check done*/
  
	printf("__________________________________\n");
  	
    readBlock(disk, 2, blockBuffer);  /*writes inode-table-block to blockBuffer*/
    p = blockBuffer;
    p+=64; //skip inode 0
  
  	mode = get4Bytes(p);
  	fatalErrorIf( (mode & IFMT) != IFDIR, ERR_ROOT_NO_DIR, "Das Root-Verzeichnis ist keine Directory !");
	
  	checkInode(p, 1 + (2 * INOPB), TRUE); //start checking the directory-tree with the root-inode (inode 1 in block 2);
  
    debugPrintBlocks();//Todo: DEBUG!
  
    fclose(disk);
    return 0; 
}
