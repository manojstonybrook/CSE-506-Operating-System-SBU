#include "decompression.h"

char reverseConvert(uint8_t i)
{
	if(i==0)
		return 'A';
	else if(i==1)
		return 'C';
	else if(i==2)
		return 'G';
	else if(i==3) return 'T';
	else return 'I';
}


char * decompression(char *buff,uint64_t length)
{
	char ch;
	char *retBuff=(char *)calloc(4*length,sizeof(char));
	uint8_t c;
	uint64_t n=0,retBuffCounter=0;
	while(n<length)
	{
		ch=buff[n];
		n++;
		c=(uint8_t)ch;
		uint8_t r;
		char *x=(char *)malloc(4*sizeof(char));
		int it=3,j;
		for(j=0;j<4;j++)
		{
			r=c%4;
			x[it]=reverseConvert(r);
			it--;
			c=c/4;
		}
		for(j=0;j<4;j++)
			retBuff[retBuffCounter++]=x[j];

	}
	return retBuff;
}
