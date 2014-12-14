#include "compression.h"

uint8_t characterConversion(char a)
{
			if(a == 'A'){
				return 0;
			}
			else if(a == 'C'){
				return 1;
			}
			else if(a == 'G'){
				return 2;
			}
			else if(a == 'T'){
				return 3;
			}
			return 5;
}

// compression code. returns the number of bytes written.
// returns 0 if size is not divisible by 4.
char* compress(char *buffer,uint64_t size)
{
	if(size%4!=0)
		return 0;
	char ch;
	uint8_t val = 0;
	uint64_t count = 0;
	uint64_t n=0;
	char *bufR=(char *)calloc(1+(size/4),sizeof(char));
	while(n<size)
	{
			ch=buffer[count];
			val = val*4 + characterConversion(ch);
			count++;
			if(count>0 && count%4==0)
			{
				bufR[n]=val;
				n++;
				val=0;
			}

	}
	return bufR;

}
