#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>

#define NU_DESC 64
#define BUFFER_SIZE 1518

// Tx Desc structure
struct tx_desc
{
	uint64_t buf_addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t css;
	uint8_t status;
	uint16_t special;
};

struct rx_desc
{
	uint64_t buf_addr;
	uint16_t length;
	uint16_t checksum;
	uint8_t status;
	uint8_t errors;
	uint16_t special;
};


volatile uint32_t *phy_mmio_addr;
// Tx Desc Registers
#define E1000_TCTL 0x00400
#define E1000_TIPG 0x00410
#define E1000_TDBAL 0x03800
#define E1000_TDBAH 0x03804
#define E1000_TDLEN 0x03808
#define E1000_TDH 0x03810
#define E1000_TDT 0x03818

// Transmit control
#define E1000_TCTL_EN 0x00000002
#define E1000_TCTL_PSP 0x00000008
#define E1000_TCTL_CT 0x00000ff0
#define E1000_TCTL_COLD 0x1000000

#define E1000_TXD_CMD_RS 0x00000008
#define E1000_TXD_CMD_EOP 0x00000001

//Rx Desc Registers
#define E1000_RCTL 0x00100
#define E1000_RDBAL 0x02800
#define E1000_RDBAH 0x02804
#define E1000_RDLEN 0x02808
#define E1000_RDH 0x02810
#define E1000_RDT 0x02818
#define E1000_MTA 0x5200


// Receive Control
#define E1000_RCTL_EN 0x00000002
#define E1000_RCTL_BAM 0x00008000
#define E1000_RCTL_CRC 0x04000000

#define E1000_RX_RAL 0x05400
#define E1000_RX_RAH 0x05404

struct tx_desc tx_desc_table[NU_DESC];
struct rx_desc rx_desc_table[NU_DESC];
char tx_buf[NU_DESC][BUFFER_SIZE];
char rx_buf[NU_DESC][BUFFER_SIZE];


int e1000_pci_attach(struct pci_func *);
int transmit_packet(char *, uint32_t);
int receive_packet(char *);
#endif	// JOS_KERN_E1000_H
