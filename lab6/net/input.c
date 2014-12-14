#include "ns.h"
#include<inc/lib.h>
#include<inc/x86.h>

extern union Nsipc nsipcbuf;
static struct jif_pkt *pkt = (struct jif_pkt *)&nsipcbuf;
void sleep_msec(int msec)
{
        unsigned cur_time = sys_time_msec();
        unsigned alarm = cur_time + msec;
        while (sys_time_msec() < alarm)
                sys_yield();
}

void
input(envid_t ns_envid)
{
	binaryname = "ns_input";
	// initialize
	*pkt->jp_data = 0;
	// LAB 6: Your code here:
	// 	- read a packet from the device driver
	//	- send it to the network server
	// Hint: When you IPC a page to the network server, it will be
	// reading from it for a while, so don't immediately receive
	// another packet in to the same physical page.
	int len;
	
	while(1)
	{					
	  while((len = sys_e1000_receive(pkt->jp_data)) < 0)
	  {
	     sys_yield();
	  }	
	
	 pkt->jp_len = len;
	 ipc_send(ns_envid, NSREQ_INPUT, pkt, PTE_P | PTE_U | PTE_W);
	
	 sleep_msec(100);
	}
	
}

