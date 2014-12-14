/*
 * content_aware_chunking.h
 *
 *  Created on: 01-Dec-2014
 *      Author: rishoo
 */

#ifndef CONTENT_AWARE_CHUNKING_H_
#define CONTENT_AWARE_CHUNKING_H_


#include<stdio.h>
#include<stdint.h>
#include<string.h>
#define B 4
#define READ_SIZE 16000
#define MIN_CHUNK 2000

uint8_t getInteger(char);
long long int int_mod(long long int,long long int);//finds proper mod even if a is negative!! :D
long long int exponent(long long int,int,long long int);
int main_cac_fn(char *);

#endif /* CONTENT_AWARE_CHUNKING_H_ */
