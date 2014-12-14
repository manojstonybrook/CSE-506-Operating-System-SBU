/*
 * file_system.c
 *
 *  Created on: Nov 27, 2014
 *      Author: manoj
 */
#include <stdio.h>
#include <file_sys.h>
#include <file.h>

#include <content_aware_chunking.h>

//char* file_name = "ecoli2_w.fasta";


int main(int argc, char *argv[])
{
//#define STORE 1

int n;
char buf[512];
fs_init();
if((strcmp("sync",argv[1]))==0)
	fs_sync();

if ((strcmp("store",argv[1]))==0)
		{char *fl_name = argv[2];write_file(fl_name);}

if ((strcmp("cac",argv[1]))==0)
	{char *fl_name = argv[2];write_chunks(fl_name);}

if ((strcmp("query",argv[1]))==0)
{
	//printf("\nargs %d",argc);
	char *dump;
	if(argc<=3)
		dump = "dump.fasta";
	else
		dump = argv[3];
	char *fl_name1 = argv[2];
	struct File* file = (struct File*)malloc(sizeof(struct File));
	int err = file_stat(fl_name1, file);
	if(err < 0)
	{
		printf("\n File not Found");
		return 0;
	}
	FILE *dp = fopen(dump, "w");

	int b_no = 0;

	//while((n = file_data_read(file, buf, b_no)) > 0)
	while(b_no < file->n_blocks)
	{
	  n = file_data_read(file, buf, b_no);
	  //printf("\n%d", n);
	  fwrite(buf, sizeof(char), n, dp);
	  b_no++;
	}
	printf("\nNo of blocks %d", file->n_blocks);
	printf("\n File size %d", file->f_size);

	fclose(dp);
	printf(" \n Data written \n finished");
}

return 0;

}



int write_file_chunks(char *name,struct File *new_file)
{

	int n,j;
	char buf[512];
	FILE *readfile;
	//printf("%s\n",name);
	readfile = fopen(name, "r");
	if(!readfile)
	{
		printf("\nNo file found!\n");
		return -1;
	}
	//else printf("\nFile found\n");


	int block_no;
	if(new_file->f_size%BLKSIZE == 0)
		block_no=new_file->f_size/BLKSIZE;
	else
		block_no = (new_file->f_size/BLKSIZE)+1;

	int dup_count = 0;
	while((n = fread(&buf, sizeof(char), BLKSIZE ,readfile)) > 0)
	{
		dup_count+=file_data_write(new_file, buf, n, block_no);
		block_no++;
	}
	fclose(readfile);
	//printf("\n%d", new_file->f_size);
	return dup_count;

}

void write_chunks(char *file_name)
{
	int nchunks = main_cac_fn(file_name);
	//printf("\nNo of chunks: %d",nchunks);
	int i;
	struct File* new_file = (struct File*)malloc(sizeof(struct File));
	strcpy(new_file->key,file_name);
	int dup_count=0;
	for(i =0; i< nchunks;i++)
	{
		char file_name[sizeof "ofl10000"];
		sprintf(file_name, "ofl%d",i);
		dup_count+=write_file_chunks(file_name,new_file);
		//printf("\n\ni is %d",i);
	}
	file_meta_data_write(new_file);
	printf("\nDuplicate count %d",dup_count);
		//printf("\nNo. of blocks %d",block_no);
		float dedup_ratio;
		dedup_ratio = deduplication_ratio();

		printf("\n system_dedup_ratio %f", dedup_ratio);
		printf("\n finished");

}

void write_file(char *name)
{

	FILE *readfile;
	int n;
	//char* name = "ecoli2_parsed.txt";
	char buf[512];
	struct File* new_file = (struct File*)malloc(sizeof(struct File));
	readfile = fopen(name, "r");
	if(!readfile) {printf("File not found!");return;}
	strcpy(new_file->key,name);
	int block_no=0,dedup_count = 0;
	while((n = fread(&buf, sizeof(char), BLKSIZE ,readfile)) > 0)
	{
		dedup_count+=file_data_write(new_file, buf, n, block_no);
		block_no++;
	}
	printf("\nNo of blocks duplicated: %d\n",dedup_count);
	printf("No of blocks written: %d\n",block_no);
	//printf("File size: %d\n",new_file->f_size);
		float dedup_ratio;
		dedup_ratio = deduplication_ratio();
		printf("\n system_dedup_ratio %f", dedup_ratio);
		file_meta_data_write(new_file);
		printf("\n finished");
}
