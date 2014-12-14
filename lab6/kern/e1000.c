#include <kern/e1000.h>
#include <kern/pci.h>
#include <kern/pmap.h>
#include <inc/types.h>
#include <inc/string.h>
#include <inc/error.h>

// LAB 6: Your driver code here

int e1000_pci_attach(struct pci_func *pciFunction)
{
	uint32_t dev_stat_reg;
	
	pci_func_enable(pciFunction);
	phy_mmio_addr = mmio_map_region(pciFunction->reg_base[0], pciFunction->reg_size[0]);
	
	char * base_addr = (char *)phy_mmio_addr;

	//Set the register values

	//base adress LOW	
        uint32_t* tdbal = (uint32_t*) (base_addr+ E1000_TDBAL);
	*tdbal = (uint64_t)PADDR(tx_desc_table);		
	
	//base address HIGH
        uint32_t* tdbah = (uint32_t*) (base_addr+ E1000_TDBAH);
	*tdbah = 0;	
	
	//LENGTH
	uint32_t *tdlen = (uint32_t*) (base_addr+ E1000_TDLEN);
	*tdlen = sizeof(tx_desc_table);	
	
	// head
	uint32_t *tdh = (uint32_t*) (base_addr+ E1000_TDH);
	*tdh = 0;	
	
	// tail
	uint32_t *tdt = (uint32_t*) (base_addr+ E1000_TDT);
	*tdt = 0;	

	// TCTL (tranmit control) E1000_TCTL_en | E1000_TCTL_psp | E1000_TCTL_ct | E1000_TCTL_cold
	uint32_t *tctl = (uint32_t*) (base_addr+ E1000_TCTL);
	*tctl = E1000_TCTL_EN | E1000_TCTL_PSP | E1000_TCTL_CT | E1000_TCTL_COLD;	
	
	uint32_t *tipg = (uint32_t*) (base_addr+ E1000_TIPG);
	*tipg = 0x60200a;	

	int i;
	for(i = 0; i < NU_DESC; i++)
	{	
	  tx_desc_table[i].status |= 1;
	  tx_desc_table[i].buf_addr = PADDR(tx_buf[i]);
	}


	//Setting up Reciever
	//LOW	
        uint32_t* rxral = (uint32_t*) (base_addr+ E1000_RX_RAL);
	*rxral = 0x12005452;		
	
	//HIGH	
        uint32_t* rxrah = (uint32_t*) (base_addr+ E1000_RX_RAH);
	*rxrah = 0x80005634;		
		
	//MTA	
        uint32_t* mta = (uint32_t*) (base_addr+ E1000_MTA);
	*mta = 0;		
	
	uint32_t* rdbal = (uint32_t*) (base_addr+ E1000_RDBAL);
	*rdbal = PADDR(rx_desc_table);		
	
	//Length
	uint32_t* rdlen = (uint32_t*) (base_addr+ E1000_RDLEN);
	*rdlen = sizeof(rx_desc_table);		
	
	//head
	uint32_t* rdh = (uint32_t*) (base_addr+ E1000_RDH);
	*rdh = 0;		
	
	//tail
	uint32_t* rdt = (uint32_t*) (base_addr+ E1000_RDT);
	*rdt = 0;		
	
	//RCTL E1000_RCTL_en | E1000_RCTL_bam | E1000_RCTL_crc
	uint32_t* rctl = (uint32_t*) (base_addr+ E1000_RCTL);
	*rctl = E1000_RCTL_EN | E1000_RCTL_BAM | E1000_RCTL_CRC;		
	
	for(i = 0;i < NU_DESC; i++)
	 {
	   rx_desc_table[i].buf_addr = PADDR(rx_buf[i]);
	 }

	return 1;
}


int transmit_packet(char *buffer, uint32_t length)
{
	// free head
	static int head = 0;
	uint32_t* free_desc_addr = (uint32_t*) ((char *)phy_mmio_addr + E1000_TDT);
	//status to transmit
	if(!(tx_desc_table[head].status & 0x1))
	{
	  cprintf("descriptor is full not able to transmit ");
	  return -1;
	}

	int i;
	//data
	memmove(tx_buf[head], buffer, length);
	//length	
	tx_desc_table[head].length = length;

	// report status and end of packet	
	tx_desc_table[head].cmd |= (E1000_TXD_CMD_RS| E1000_TXD_CMD_EOP);
	//next free
	head = (head +1)%NU_DESC;
	*free_desc_addr = head;

	return 0;
}

int receive_packet(char *buffer)
{
	//free head
	static int head = 0;
	uint32_t* free_desc_addr = (uint32_t*) ((char *)phy_mmio_addr + E1000_RDT);
	//status to recieve
	if(!(rx_desc_table[head].status & 0x1))
	{
	  cprintf("no data received");
	  return -1;
	}

	int len = rx_desc_table[head].length;

	//data
	memmove(buffer,rx_buf[head],len);
	rx_desc_table[head].status = 0;
	head = (head+1)%NU_DESC;
	*free_desc_addr = head;

	return len; 
}

