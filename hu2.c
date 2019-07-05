#include "hu2.h"


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

unsigned int fsStart; /* file system start sector *//*Todo: Why global, maybe later?*/

FILE *disk;	           /*the disk to check*/

Block * blocks;
unsigned int numBlocks;
Inode * inodes;
unsigned int numInodes;

unsigned int freeblks;	/*num of free blocks, set in superBlock check -> verify after check over*/
unsigned int freeinos; 	/*num of free inodes, set: superBlock -> verify: later*/
unsigned int numberOfEntriesinFreeinos;	/*num of entries in free inode list, set: superBlock -> verfy: later*/

unsigned int specialBlockEnd;        /*indicates where the "special" bocks end -> start block check from there*/

unsigned int maxSize;               // the max possible filesize for a given inode, manipulated in checkBlock, read in checkInode

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

void debugPrintInodes(void){
    printf("#DEBUG| Prints all inodes in inode-array %d\n",numInodes);/*Todo: DEBUG*/
    for(int i = 0; i < numInodes*INOPB; i++){
        Inode *x = &inodes[i]; //Todo: Block *x = &blocks[i]; OR Block x = blocks[i]; ?
        if(x->refcount>0){
            printf("#DEBUG| Inode[%d] -> (%d) \n", i, x->refcount);/*Todo: DEBUG*/
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
    unsigned int ninode;

    p += 4; /*skip magic number */
    fsize = get4Bytes(p); //printf("#DEBUG| file system size = %u (0x%X) blocks\n", fsize, fsize);/*Todo: DEBUG*/
    
    /*Initialisation of blocks*/
    if(fsize!=numBlocks){printf("#DEBUG| wie ist das denn möglich??? fsize sollte numBlocks entsprechen!!"); /*Todo: DEBUG*/}
    
  	numBlocks = fsize;
	blocks = (Block *) malloc(numBlocks * sizeof(Block));
    fatalErrorIf(!blocks,ERR_MALLOC,"Insufficient memory at block allocation");
    
    p += 4;
    isize = get4Bytes(p);
    //printf("#DEBUG| inode list size  = %u (0x%X) blocks\n", isize, isize); /*Todo: DEBUG*/

  	/*Initialisation of inodes*/
    numInodes = isize;
  	inodes = (Inode *) malloc(INOPB * numInodes * sizeof(Inode));//Todo: We should read numInodes from the inode-table later!
  	fatalErrorIf(!inodes,ERR_MALLOC,"Insufficient memory at inode allocation");

  	specialBlockEnd = isize + 1 + 1;        // skip ALL inodeBlocks + the bootblock + superblock later
    
    p += 12;
    ninode = get4Bytes(p);
    p += 4;     //ninode

    p+= 4* NICINOD;     //ino (s)

  	/*Todo kann das nich auch in die checkFreeBlock methode? V*/
    nfree = get4Bytes(p);
    p += 4;       // nfree
    for (int i = 0; i < NICFREE; i++) {
        free = get4Bytes(p);
        p += 4;
        if (i < nfree) {
          	if(i == 0) {
                checkFreeBlock(free);    // block 0 is freeBlock
          	}
      	}
    }
 	/*Bis hier?^*/
}

void checkBlock(unsigned int addr, int type, int flag){
  	if(addr == 0) return;
    unsigned char blockBuffer[BLOCK_SIZE];
    unsigned int ino;
    unsigned char * p;
    unsigned char * ino_p;

    maxSize += BLOCK_SIZE;              //increment maximum file size by one block

  	incrementBlock(addr,B_FILE);		//increment file-counter of "old" address

  	readBlock(disk, addr, blockBuffer);
  
	p = blockBuffer;
  
    if(type==DOUBLE) {
      	for (int i = 0; i < BLOCK_SIZE/4; i++) {
          	checkBlock(get4Bytes(p), SINGLE, flag); 
          	p+=4;
        }
    } else if(type==SINGLE) {
      	for (int i = 0; i < BLOCK_SIZE/4; i++) {
          	checkBlock(get4Bytes(p), DIRECT, flag); 
          	p+=4;
        }
    } else if(type==DIRECT) {
        if(flag==DIR) {
          
          	for (int i = 0; i < DIRPB; i++) {
                unsigned char ino_blockBuffer[BLOCK_SIZE];
              	ino = get4Bytes(p);

              	printf("Die Inode: %u \n",ino);
              	
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
  	unsigned int size;
  	int type;

    mode = get4Bytes(p);

    p+= 28; // skip until size: mode//nlink//uid//gid//tim//tim//tim//

    size = get4Bytes(p);

    maxSize = 0;        // reset maxSize for current inode

    p+=4; //size
	
  	if((mode & IFMT) == IFREG) {
      	printf("#DEBUG| inode is REG\n");//Todo: DEBUG
      	type = REG;
      	Inode *x = &inodes[inID];
        x->refcount +=1;

        printf("Die Inode: %u, hat den refcount %d \n",inID, x->refcount);
    } else if((mode & IFMT) == IFDIR) {
      	printf("#DEBUG| inode is DIR\n");//Todo: DEBUG
      	type = DIR;
      	Inode *x = &inodes[inID];
        x->refcount +=1;

        printf("Die Inode: %u, hat den refcount %d \n",inID, x->refcount);
    } else if(( (mode & IFMT) == IFCHR) || ((mode & IFMT) == IFBLK)){
        printf("#DEBUG| inode is BLOCK or CHAR\n");//Todo: DEBUG
        type = SPE; // BLK or CHR
        Inode *x = &inodes[inID];
        x->refcount +=1;

        printf("Die Inode: %u, hat den refcount %d \n",inID, x->refcount);
  	}
  	
  	if(doRecursion) {
      	//Direct blocks
        for (int j = 0; j < 6; j++) {
          addr = get4Bytes(p);
          p += 4;
          if(type == DIR || type == REG) {
              printf("checking direct \n");
              checkBlock(addr, DIRECT, type);
          }
        }
      
      	//Single indirect block
        addr = get4Bytes(p);
        p += 4;
        if(type == DIR || type == REG){
            printf("checking single \n");
            checkBlock(addr, SINGLE, type);
        }

      	//Double indirect block
        addr = get4Bytes(p);
        p += 4;
        if(type == DIR || type == REG) {
            printf("checking double \n");
            checkBlock(addr, DOUBLE, type);
        }
  	}else{

        for (int j = 0; j < 6; j++) {
            addr = get4Bytes(p);
            p += 4;
            if(type == DIR || type == REG) printf("Adresse: %d\n",addr);
        }

        //Single indirect block
        addr = get4Bytes(p);
        p += 4;
        if(type == DIR || type == REG) printf("Adresse: %d\n",addr);

        //Double indirect block
        addr = get4Bytes(p);
        p += 4;
        if(type == DIR || type == REG) printf("Adresse: %d\n",addr);
  	}

  	//check size

  	printf("The size is: %d, while max size is: %d \n", size, maxSize);

  //	if(type!= SPE) fatalErrorIf(size > maxSize, ERR_FILE_SIZE, "File is larger than indicated by block size");

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


void checkBlockList(){

        printf("#DEBUG| Prints all used Blocks in blocks-array %d\n",numBlocks);/*Todo: DEBUG*/
        for(int i = specialBlockEnd; i < numBlocks; i++){
            Block *x = &blocks[i];

            fatalErrorIf(x->filerefs == 0 && x->listrefs == 0,ERR_NOT_FILE_LIST,"Block is neither free nor in file !");

            fatalErrorIf(x->filerefs == 1 && x->listrefs == 1,ERR_IN_FILE_LIST,"Block is both in free list and a file !");

            fatalErrorIf(x->listrefs > 1,ERR_MULTI_FILE,"Block has multiple occurences in free list !");

            fatalErrorIf(x->filerefs > 1,ERR_MULTI_LIST,"Block has multiple occurences in file(s) !");

            //prints non-erronuos blocks
            printf("#DEBUG| Block[%d] -> (%d , %d) \n", i, blocks[i].filerefs, blocks[i].listrefs);/*Todo: DEBUG*/
        }

}

void checkInodeList(){

    //iterativer Durchgang durch die inodes

    unsigned char blockBuffer[BLOCK_SIZE];
    unsigned char * p;
    //unsigned int maxFileSize;
    unsigned int size;
    unsigned int nlink;
    unsigned int mode;
    unsigned int addr;


    for(int bi=2; bi < numInodes + 2 ; bi++ ){     // start at block 2 -> start of inode - table

        readBlock(disk,bi,blockBuffer);
        p = blockBuffer;

        printf("block %d,", bi);
        printf("________________________\n");

        for (int ii = 0; ii < INOPB; ii++) {

            if(bi==2){     //skip 1st inode of block 2
                p+=16*4;
                continue;
            }

            printf("inode [%d]: \n", ii + INOPB * (bi -2));

            mode = get4Bytes(p);
            p += 4; //mode

            nlink = get4Bytes(p);       //nlink
            p += 4;

            p += 4;             //uid

            p += 4;             //gid

            p += 4;             //tim

            p += 4;             //tim2

            p += 4;             //tim3

            size = get4Bytes(p);
            p += 4;             //size

            char * flag;

            if( ((mode & IFMT) == IFDIR) ){
                flag = "DIR";
            } else if( ((mode & IFMT) == IFREG) ){
                flag = "REG";
            } else if( ((mode & IFMT) == IFBLK) ){
                flag = "BLK";
            } else if( ((mode & IFMT) == IFCHR) ){
                flag = "BLK";
            }
            if( ((mode & IFMT) == IFFREE) ){
                flag = "FREE";
            }

            printf("is %s, has %u links and size = %u \n", flag, nlink, size);

            for (int j = 0; j < 6; j++) {
                addr = get4Bytes(p);
                p += 4;
                if (addr != 0) {
                    //maxFileSize +=BLOCK_SIZE;
                    printf("  direct block[%1d] = %u (0x%X)\n", j, addr, addr);
                }
            }
            addr = get4Bytes(p);
            p += 4;
            if (addr != 0) {
                //maxFileSize += 1024 * BLOCK_SIZE;
                printf("  single indirect = %u (0x%X)\n", addr, addr);
            }
            addr = get4Bytes(p);
            p += 4;
            if (addr != 0) {
                // maxFileSize += 1024 * 1024 * BLOCK_SIZE;            //ggf überprüfen ob die Blöcke SELBST alle !=0 sind
                printf("  double indirect = %u (0x%X)\n", addr, addr);
            }

            Inode * reference = &inodes[(bi-2) * INOPB + ii];     // get the inode to compare with from Inodes - list

            //error-checking

            fatalErrorIf( ((mode & IFMT) != IFDIR) && reference->refcount<=0, ERR_DIR_UNRBL, "Directory is unreachable from root");

            // fatalErrorIf(size>maxFileSize, ERR_FILE_SIZE,"Filesize does not correspond to blocks");  //maybe check here to but I dont know how
            fatalErrorIf(  (((mode & IFMT) != IFREG) && ((mode & IFMT) != IFDIR) && ((mode & IFMT) != IFCHR) && ((mode & IFMT) != IFBLK)), ERR_INN_ILL_TYPE, "Illegal Inode type");

            fatalErrorIf( nlink==0 && reference->refcount < 0, ERR_IN0_IN_DIR,"Inode with linkcount 0 appears in directory");
            fatalErrorIf( nlink==0 && ((mode & IFMT) == IFFREE), ERR_IN0_NOT_FREE,"Inode with linkcount 0 is not free");
            fatalErrorIf( nlink!=0 && nlink != reference->refcount, ERR_INN_DIR_COUNT, "Inode does not appear in correct number of directories");
            fatalErrorIf( reference->refcount && ((mode & IFMT) == IFFREE), ERR_INF_IN_DIR, "Inode appears in directory, despite being free ");

        }
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
  	fatalErrorIf(argc!=3,ERR_WRONG_CALL, "Not enough arguments ! \n"
                                        "usage: ./hu2 <disk> <partition> \n"
                                        "<disk> = name of EOS32 image file \n"
                                        "<partition> = partition to inspect \n");

    diskName = argv[1];
    disk = fopen(diskName, "rb");
    
  	fatalErrorIf(disk==NULL,ERR_IMG_NOT_FOUND, "Image file could not be found");
  	
    fseek(disk, 1 * SECTOR_SIZE, SEEK_SET);
    
  	fatalErrorIf(fread(partTable, 1, SECTOR_SIZE, disk) != SECTOR_SIZE,ERR_FILE_IO, "No Partition table found \n");
      
    /*Exits if partNum is to big or argv[1] is not an int*/
    char *rest;
    long arg2 = strtoul(argv[2], &rest, 10);
    if(arg2 > 15 || arg2 < 0 || *rest != '\0'){
        fatalError(ERR_ILL_PART_NUM, "Illegal partition number \n");
    } else {
        partitionNum = arg2;
    }

    ptptr = partTable + partitionNum * 32;
    partType = get4Bytes(ptptr + 0);
    /*todo: check if partitionEntry is empty*/
		
  	fatalErrorIf(((partType & 0x7FFFFFFF) != 0x00000058), ERR_PART_NO_EOS, "Partition does not contain an EOS-Filesystem");
  
    fsStart = get4Bytes(ptptr + 4);
    
  	fsSize = get4Bytes(ptptr + 8);
	fatalErrorIf(fsSize % SPB != 0,ERR_OTHER_FS, "File system size is not a multiple of block size.");
    
  	numBlocks = fsSize / SPB;
  	fatalErrorIf(numBlocks <2, ERR_OTHER_FS, "File system has less than 2 blocks.");
  
    /*Super block check*/
    printf("Starting super block check \n");

    readBlock(disk, 1, blockBuffer); /*writes superblock to blockBuffer*/
    checkSuperBlock(blockBuffer);

    /*Super block check done*/


  	/*Start iteration through directories */
  	printf("Starting recursive run trough directories \n");

    readBlock(disk, 2, blockBuffer);  /*writes inode-table-block to blockBuffer*/
    p = blockBuffer;
    p+=64; //skip inode 0
  
  	mode = get4Bytes(p);
  	fatalErrorIf( (mode & IFMT) != IFDIR, ERR_ROOT_NO_DIR, "Das Root-Verzeichnis ist keine Directory !");
	
  	checkInode(p, 1 + (2 * INOPB), TRUE); //start checking the directory-tree with the root-inode (inode 1 in block 2);
    /*Inode iteration end*/

  	debugPrintBlocks();//Todo: DEBUG!
    debugPrintInodes();  //Todo: debugn

    printf("Starting check for block - inconsistencies \n");
    //checkBlockList();

    printf("Starting check for inode - inconsistencies\n");
    //checkInodeList();

    fclose(disk);

    printf("File system check done: No errors detected. \n");
    return 0; 
}
