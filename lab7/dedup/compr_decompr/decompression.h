#ifndef DECOMPRESSION_H
#define DECOMPRESSION_H_


#include<stdio.h>
#include<stdint.h>
#include<string.h>

char reverseConvert(uint8_t i);
char * decompression(char *buff,uint64_t length);

#endif /* DECOMPRESSION_H*/
