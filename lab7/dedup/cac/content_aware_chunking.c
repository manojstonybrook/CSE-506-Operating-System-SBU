// implementing content aware chunking.
// this algorithm divides a given file into large chunks which are then stored blockwise!!
// takes a file as an input and divides it into multiple files(chunks)
// the input file MUST ALWAYS HAVE DOT EXTENSION!! 
#include "content_aware_chunking.h"

uint64_t M=2147483647;
uint32_t w=48;
uint32_t t=12;

uint8_t getInteger(char ch)
{
	if(ch=='A')
		return 0;
	if(ch=='C')
		return 1;
	if(ch=='T')
		return 2;
	if(ch=='G')
		return 3;
	return 3;
}
long long int int_mod(long long int a,long long int b)//finds proper mod even if a is negative!! :D
{
	return ((a%b+b)%b);
}
long long int exponent(long long int a,int x,long long int mod)
{
	long long int exp=1,i;
	for(i=1;i<=x;i++)
		exp=(exp*a)%mod;
	return exp;
}

int main_cac_fn(char *f)
{
	//char *f="2_dump.fasta";
	FILE *read,*write;
	read=fopen(f, "r+");
	if(!read) printf("\nFile not found!");
	else printf("\nFile found");
	int count=0,nc=0,fc=0;
	uint32_t i,j;
	uint64_t file_counter=0,curr_fc,l;
	while(1)
	{
		//char file_name[sizeof "file10000.txt"];
		//sprintf(file_name, "of%d.txt", count++);
		//write=fopen(file_name,"w+");
		
		char chunk[READ_SIZE+1];
		fseek(read,file_counter,SEEK_SET);
		l=fread(chunk,1,READ_SIZE,read);
		if(l==0) break;
		else if(l<=MIN_CHUNK+w+5)//end of file reached
		{
			//do nothing
			char file_name[sizeof "ofl10000"];
			sprintf(file_name, "ofl%d", count);
			count++;
			write=fopen(file_name,"w+");
			for(j=0;j<l;j++)
				fputc(chunk[j],write);
			fclose(write);
			fc++;
			printf("Forceful chunk2 %d\n",fc);
			break;
		}
		else
		{
			//find chunk
			uint8_t flag=0;
			int start_window,end_window;
			curr_fc=file_counter;
			long long int hash_value=0;
			file_counter+=MIN_CHUNK;
			for(i=MIN_CHUNK;i<w+MIN_CHUNK;i++)
			{
				hash_value=int_mod(hash_value*B+getInteger(chunk[i]),M);
				file_counter++;
			}
			//calculated hash_value for window
			while(i<l)
			{
				if(hash_value%(1<<t)==0)//this is the chunk!!
				{
					flag=1;
					//write from the beginning of the chunk to position i!!
					char file_name[sizeof "ofl10000"];
					sprintf(file_name, "ofl%d", count);
					count++;
					write=fopen(file_name,"w+");
					for(j=0;j<i;j++)
						fputc(chunk[j],write);
					fclose(write);
					break;
				}
				else
				{
					//if(i>=READ_SIZE)break;
					hash_value=int_mod(hash_value-int_mod(getInteger(chunk[i-w])*exponent(B,w-1,M),M),M);
					hash_value=int_mod(hash_value*B,M);
					hash_value=int_mod(hash_value+getInteger(chunk[i]),M);
					i++;
					file_counter++;
				}
			}
			if(flag==1)//chunked
			{
				nc++;
				//printf("Normal chunk %d\n",nc);
			}
			else //not chunked
			{
				char file_name[sizeof "ofl10000"];
				sprintf(file_name, "ofl%d", count);
				count++;
				write=fopen(file_name,"w+");
				for(j=0;j<i;j++)
					fputc(chunk[j],write);
				fclose(write);
				fc++;
				//printf("Forceful chunk1 %d\n",fc);
			}
		}
		
		/*for(i=0;i<strlen(s);i++)
			if(s[i]=='A' || s[i]=='C' || s[i]=='T' || s[i]=='G')
				printf("%c",s[i]);
		*/
		
	}
	//printf("%lld\n",file_counter);
	fclose(read);
	printf("\nNormal chunks: %d Forced chunks: %d Total chunks: %d",nc,fc,(nc+fc));
	return (fc+nc);
}


