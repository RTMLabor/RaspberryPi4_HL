#ifndef __SPI_H
#define __SPI_H

#include "cpu_cortexa72.h"
#include "gpio.h"


#define ENC_INT_PIN     5
#define ENC_CS_PIN      7
#define ENC_MISO_PIN    9
#define ENC_MOSI_PIN    10
#define ENC_SCLK_PIN    11

struct Spi0Regs {
    reg32 cs;
    reg32 fifo;
    reg32 clock;
    reg32 data_length;
    reg32 ltoh;
    reg32 dc;
};

#define REGS_SPI0 ((struct Spi0Regs *)(BCM_RPI_PERIPHERAL_BASEADDR + 0x00204000))

// CS Register
#define CS_LEN_LONG	(1 << 25)
#define CS_DMA_LEN	(1 << 24)
#define CS_CSPOL2	(1 << 23)
#define CS_CSPOL1	(1 << 22)
#define CS_CSPOL0	(1 << 21)
#define CS_RXF		(1 << 20)
#define CS_RXR		(1 << 19)
#define CS_TXD		(1 << 18)
#define CS_RXD		(1 << 17)
#define CS_DONE		(1 << 16)
#define CS_LEN		(1 << 13)
#define CS_REN		(1 << 12)
#define CS_ADCS		(1 << 11)
#define CS_INTR		(1 << 10)
#define CS_INTD		(1 << 9)
#define CS_DMAEN	(1 << 8)
#define CS_TA		(1 << 7)
#define CS_CSPOL	(1 << 6)
#define CS_CLEAR_RX	(1 << 5)
#define CS_CLEAR_TX	(1 << 4)
#define CS_CPOL__SHIFT	3
#define CS_CPHA__SHIFT	2
#define CS_CS		(1 << 0)
#define CS_CS__SHIFT	0

void spi0_init();
void spi0_chip_select(u8 chip_select);
void spi0_chip_deselect (u8 chip_select);
void spi0_send_recv(u8 chip_select, u8 *sbuffer, u8 *rbuffer, u32 size);

void spi0_send(u8 chip_select, u8 *data, u32 size);
void spi0_recv(u8 chip_select, u8 *data, u32 size);

#endif