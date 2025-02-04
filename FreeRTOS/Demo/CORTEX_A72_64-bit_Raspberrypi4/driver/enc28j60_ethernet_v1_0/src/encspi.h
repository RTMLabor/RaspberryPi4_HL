#ifndef __ENCSPI_H
#define __ENCSPI_H

#include "spi0.h"
#include "enc28j60.h"
#include "printf.h"
#include "bparameters.h"
#include "bcm_mem.h"

// Structure for Ethernet header
typedef struct {
   uint8_t DestAddrs[6];
   uint8_t SrcAddrs[6];
   uint16_t type;
} EtherNetII;

// Ethernet packet types

#define ARPPACKET 0x0608
#define IPPACKET  0x0008

#define ETH_PRIORITY (0xA1)

// Structure for an ARP Packet

typedef struct {
   EtherNetII eth;
   uint16_t hardware;
   uint16_t protocol;
   uint8_t hardwareSize;
   uint8_t protocolSize;
   uint16_t opCode;
   uint8_t senderMAC[6];
   uint8_t senderIP[4];
   uint8_t targetMAC[6];
   uint8_t targetIP[4];
} ARP;

// ARP OpCodes

#define ARPREPLY   0x0200
#define ARPREQUEST 0x0100

// ARP hardware types

#define ETHERNET   0x0100

void ENC_SPI_Select(bool select);
void ENC_SPI_SendBuf(u8 *master2slave, u8 *slave2master, u16 bufferSize);
void ENC_SPI_Send(u8 command);
void ENC_SPI_SendWithoutSelection(u8 command);

void enc28j60_isr(void);

void init_network(void);

ENC_HandleTypeDef *encspi_getHandle(void);

// TESTING FUNCTIONS

void SendArpPacket(uint8_t *targetIP, uint8_t *deviceMAC);
void arp_test(void);


#endif