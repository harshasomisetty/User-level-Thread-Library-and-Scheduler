/*
 *  Copyright (C) 2019 CS416 Spring 2019
 *	
 *	Tiny File System
 *
 *	File:	tfs.h
 *  Author: Yujie REN
 *	Date:	April 2019
 *
 */

#include <linux/limits.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef _TFS_H
#define _TFS_H

#define MAGIC_NUM 0x5C3A
#define MAX_INUM 1024
#define MAX_DNUM 16384


#define INODE_BITMAP_SIZE 1
#define DATA_BITMAP_SIZE 1
#define INODE_BLOCK_RESERVE 482
#define INODE_BLOCK_RESERVE_INDEX 3
#define INODE_SIZE 256
#define DATA_BLOCK_RESERVE 7707
#define DATA_BLOCK_RESERVE_INDEX 485
#define SUPERBLOCK_INDEX 0
#define INODE_MAP_INDEX 1
#define DATA_MAP_INDEX 2
#define DIRECT_PTR_ARR_SIZE 16
#define VALID 1
#define INVALID 0

#include "global.h"

struct superblock {
	uint32_t	magic_num;			/* magic number */
	uint16_t	max_inum;			/* maximum inode number */
	uint16_t	max_dnum;			/* maximum data block number */
	uint32_t	i_bitmap_blk;		/* start address of inode bitmap */
	uint32_t	d_bitmap_blk;		/* start address of data block bitmap */
	uint32_t	i_start_blk;		/* start address of inode region */
	uint32_t	d_start_blk;		/* start address of data block region */
};

struct inode {
	uint16_t	ino;				/* inode number, abstract */
	uint16_t	valid;				/* validity of the inode */
	uint32_t	size;				/* size of the file */
	uint32_t	type;				/* type of the file */
	uint32_t	link;				/* link count */
	int			direct_ptr[16];		/* direct pointer to data block, abstract indexes */
	int			indirect_ptr[8];	/* indirect pointer to data block */
	struct stat	vstat;				/* inode stat */
};

struct dirent {
	uint16_t ino;					/* inode number of the directory entry */
	uint16_t valid;					/* validity of the directory entry */
	char name[252];					/* name of the directory entry */
};

int abstractIndex(int realIndex) {
	return realIndex + 1;
}

int realIndex(int abstractIndex){
	return abstractIndex - 1;
}




/*
 * bitmap operations
 */

void set_bitmap(bitmap_t b, int i) {
    b[i / 8] |= 1 << (i & 7);
}

void unset_bitmap(bitmap_t b, int i) {
    b[i / 8] &= ~(1 << (i & 7));
}

uint8_t get_bitmap(bitmap_t b, int i) {
    return b[i / 8] & (1 << (i & 7)) ? 1 : 0;
}

#endif
