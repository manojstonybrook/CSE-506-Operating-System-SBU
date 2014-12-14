#ifndef COMPRESSION_H
#define COMPRESSION_H_


#include<stdio.h>
#include<stdint.h>
#include<string.h>

uint8_t characterConversion(char a);
char* compress(char *buffer,uint64_t size);
#endif /* COMPRESSION_H*/
