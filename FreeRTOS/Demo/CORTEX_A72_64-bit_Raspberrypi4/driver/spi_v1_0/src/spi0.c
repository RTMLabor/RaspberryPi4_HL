#include "spi0.h"
#include "printf.h"

//Dev purpose
#include "mini_uart.h"


void spi0_init() {
    gpio_pin_set_func(ENC_INT_PIN, GFInput);      // Interrupt pin for enc28j60
    gpio_pin_set_func(7, GFAlt0);       //CS1
    //gpio_pin_set_func(8, GFOutput);
    gpio_initOutputPinWithPullNone(8);  //CS0
    //gpio_pin_set_func(8, GFAlt0);     //CS0  -> CS
    gpio_pin_set_func(ENC_MISO_PIN, GFAlt0);       //MISO -> SO
    gpio_pin_set_func(ENC_MOSI_PIN, GFAlt0);      //MOSI -> SI
    gpio_pin_set_func(ENC_SCLK_PIN, GFAlt0);      //SCLK -> SCK

    // set falling edge detection on gpio5
    gpio_pin_set_ev_detection(ENC_INT_PIN, Ev_falling);

    gpio_pin_enable(ENC_INT_PIN);
    gpio_pin_enable(ENC_CS_PIN);
    gpio_pin_enable(8);
    gpio_pin_enable(ENC_MISO_PIN);
    gpio_pin_enable(ENC_MOSI_PIN);
    gpio_pin_enable(ENC_SCLK_PIN);
}

void spi0_chip_select (u8 chip_select) {
    gpio_setPinOutputBool(8, chip_select);
    /*REGS_SPI0->cs = (REGS_SPI0->cs & ~CS_CS) | (chip_select << CS_CS__SHIFT) |
        CS_CLEAR_RX | CS_CLEAR_TX | CS_TA;
     */   //printf ("chip is selected\n");
}
void spi0_chip_deselect (u8 chip_select) {
    gpio_setPinOutputBool(8, chip_select);
    //REGS_SPI0->cs = (REGS_SPI0->cs & ~CS_TA);
        //printf ("chip is deselected\n");
}

void spi0_send_recv(u8 chip_select, u8 *sbuffer, u8 *rbuffer, u32 size) {
    REGS_SPI0->data_length = size;
    REGS_SPI0->cs = (REGS_SPI0->cs & ~CS_CS) | (chip_select << CS_CS__SHIFT) |
        CS_CLEAR_RX | CS_CLEAR_TX | CS_TA;
    // Abfrage, ob CS_TA-bit nicht gesetzt ist (spi_chip_select noch nicht durchlaufen wurde)
    /* if (REGS_SPI0->cs & ~CS_TA)
    {
        //printf("Chip select line wurde schon aufgerufen \n");
        REGS_SPI0->cs = REGS_SPI0->cs | CS_CLEAR_RX | CS_CLEAR_TX | CS_TA;
    }else
    {
        //printf("Chip select line wurde noch nicht aufgerufen \n");
    } */
    
        
    u32 read_count = 0;
    u32 write_count = 0;

    while(read_count < size || write_count < size) {
        while(write_count < size && REGS_SPI0->cs & CS_TXD) {
            if (sbuffer) {
                REGS_SPI0->fifo = *sbuffer++;
            } else {
                REGS_SPI0->fifo = 0;
            }

            write_count++;
        }

        while(read_count < size && REGS_SPI0->cs & CS_RXD) {
            u32 data = REGS_SPI0->fifo;

            if (rbuffer) {
                *rbuffer++ = data;
            }

            read_count++;
        }
    }

    while(!(REGS_SPI0->cs & CS_DONE)) {
        while(REGS_SPI0->cs & CS_RXD) {
            u32 r = REGS_SPI0->fifo;
            printf("Left Over: %d\n", r);
        }
    }
    // De-select chip select line
    //REGS_SPI0->cs = (REGS_SPI0->cs & ~CS_CS);
    REGS_SPI0->cs = (REGS_SPI0->cs & ~CS_TA);
}

void spi0_send(u8 chip_select, u8 *data, u32 size) {
    spi0_send_recv(chip_select, data, 0, size);
}

void spi0_recv(u8 chip_select, u8 *data, u32 size) {
    spi0_send_recv(chip_select, 0, data, size);
}
