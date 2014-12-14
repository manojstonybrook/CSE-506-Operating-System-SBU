#include "ns.h"
#include <inc/lib.h>

#include <inc/x86.h>
extern union Nsipc nsipcbuf;
static struct jif_pkt *pkt = (struct jif_pkt *)&nsipcbuf;

// Taken from test input to dump output 
static void
hexdump_output_test(const char *prefix, const void *data, int len);
void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	int len;
	char buf[80];
	char *end = buf + sizeof(buf);	
	while(1)
	{
	 if((len = ipc_recv(NULL, pkt, NULL)) < 0)
	 {
	    return;
	 }
	 hexdump_output_test("output.c: ", pkt->jp_data, pkt->jp_len);
	  //snprintf(buf, end - buf,"%s%04x   ", "output.c: ", 0);
	  while((len = sys_e1000_transmit(pkt->jp_data, pkt->jp_len)) < 0)
		sys_yield();
	}
	return;
}

static void
hexdump_output_test(const char *prefix, const void *data, int len)
{
        int i;
        char buf[80];
        char *end = buf + sizeof(buf);
        char *out = NULL;
        for (i = 0; i < len; i++)
	{
		if (i % 16 == 0)
		   out = buf + snprintf(buf, end - buf,"%s%04x   ", prefix, i);
		out += snprintf(out, end - out, "%02x", ((uint8_t*)data)[i]);
		if (i % 16 == 15 || i == len - 1)
		        cprintf("%.*s\n", out - buf, buf);
		if (i % 2 == 1)
		        *(out++) = ' ';
		if (i % 16 == 7)
		        *(out++) = ' ';
        }
}
