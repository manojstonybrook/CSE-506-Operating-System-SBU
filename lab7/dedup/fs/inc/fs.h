/*
 * fs.h
 *
 *  Created on: Nov 27, 2014
 *      Author: manoj
 */

#ifndef FS_H_
#define FS_H_

#include <stdio.h>

//typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
//const char * fs_name = "FS";
#define BLKSIZE		512
#define ENTRY		(BLKSIZE/4)
// Maximum size of a filename (a single path component), including null
// Must be a multiple of 4
#define MAXNAMELEN	16

// Number of block pointers in a File descriptor
#define NDIRECT	 84 // 84*128*512 ~5.25MB
#define SuperBlockAddr_start 0x10

//Assuming my file system can store 500 files so we have to store
// store 128*500 file structures so total size 128*8*500 ~ 512000 (0x7D000)
#define SuperBlockAddr_end   0x7D000
// First 128 bytes in super block are for book keeping of pointer available for write
// in data and super block
#define superblock_tail_addr        0x0
#define datablock_tail_addr         0x4
#define superblock_total_blocks     0x8
#define superblock_written_blocks   0xC


#define HashBlock_Start 0x7D000
//32 MB first hash block
#define HashBlock_end 0x207D000

#define HashLowBits  0x01FFFFFF

#define HashHighBits 0xFE000000


#define DataBlock_Start 0x207D000
// 4GB File System
#define DataBlock_end   0x100000000

// total file structure size 128 bytes
struct File {
	char key[MAXNAMELEN];	    // filename act as a key
	uint32_t f_size;			// file size in bytes

	// Direct blocks each block is of 512 bytes and 512/4 ~= 128 entries
	//  total 20 of these blocks so total file size 20*128*512 ~ 1.25 MB file
	uint32_t f_direct[NDIRECT];   // 32+32+20
	uint32_t f_written;
	uint32_t n_blocks;
	uint32_t pad[5];
};

struct SuperBlock {
    //tail pointer in super block
	uint32_t tail_sb;
	//tail pointer in data block
	uint32_t tail_db;
	uint32_t total_blocks;
	uint32_t written_blocks;
	// Extra space can be used for maintaining information at FileSystem level
	//uint32_t pad[28];
};

#endif /* FS_H_ */
