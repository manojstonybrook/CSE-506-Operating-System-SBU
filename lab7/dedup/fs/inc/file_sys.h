/*
 * file_sys.h
 *
 *  Created on: Nov 27, 2014
 *      Author: manoj
 */

#ifndef FILE_SYS_H_
#define FILE_SYS_H_

#include <fs.h>

static const char * FS_NAME = "../FS4";

/* fs.c */
int alloc_block(int *addr);
void flush_block(int addr);
void superblock_update(void);
void fs_init(void);
float file_deduplication_ratio(struct File *f);
float deduplication_ratio();
void initialize_superblock(void);
int file_get_block(struct File *f, uint32_t file_blockno, int *pblk);
//int	file_create(const char *path, struct File **f);
//int	file_open(const char *path, struct File **f);
int	file_data_read(struct File *f, char *buf, int block_no);

int	file_data_write(struct File *f, char *buf, size_t count, int block_no);
int	file_meta_data_write(struct File *f);

int	file_set_size(struct File *f, off_t newsize);
int file_stat(char * key, struct File *f);
uint32_t generate_hash(const char *buf, size_t count);
//void	file_flush(struct File *f);
//int	file_remove(const char *path);
void	fs_sync(void);
int SuperBlockTail_Update(int offset);
int DataBlockTail_Update(int offset);
int write_deduplicate_block_addr(struct File *f, uint32_t file_blockno, int pblk);

#endif /* FILE_SYS_H_ */
