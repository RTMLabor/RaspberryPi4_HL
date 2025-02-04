#include "encspi.h"
#include "board.h"
#include "interrupt.h"

// MAC address to be assigned to the ENC28J60

uint8_t myMAC[6] = { 0xc0, 0xff, 0xee, 0xc0, 0xff, 0xee };

// Router MAC is not known to start with, and requires an ARP reply to find out

uint8_t routerMAC[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

// IP address to be assigned to the ENC28J60

uint8_t deviceIP[4] = { 192, 168, 178, 20 };

// IP Address of the router, whose hardware address we will find using the ARP request

uint8_t routerIP[4] = { 192, 168, 178, 1 };


ENC_HandleTypeDef networkhandle;

void ENC_SPI_Select(bool select) {
    /*if (true == select)
    {
        spi_chip_select(0);
    }else if(false == select) {
        spi_chip_deselect(0);
    } */   
    //spi_chip_select(!truefalse); // If it's true, select 0 (the ENC), if false, select 1 (i.e. deselect the ENC)
    spi0_chip_select(!select); // If it's true, select 0 (the ENC), if false, select 1 (i.e. deselect the ENC)
}

void ENC_SPI_SendBuf(u8 *master2slave, u8 *slave2master, u16 bufferSize) {
    spi0_chip_select(0);
    spi0_send_recv(0, master2slave, slave2master, bufferSize);
    spi0_chip_select(1);
}

void ENC_SPI_Send(u8 command) {
    spi0_chip_select(0);
    spi0_send(0, &command, 1);
    spi0_chip_select(1);
}

void ENC_SPI_SendWithoutSelection(u8 command) {
    spi0_send(0, &command, 1);
}

void enc28j60_isr(void)
{
   uart_puts("enc28j60 interupt \n");
   gpio_pin_clear_ev_detection(ENC_INT_PIN);
   ENC_IRQHandler(&networkhandle);
   return;

}

void init_network(void)
{
   networkhandle.Init.DuplexMode = ETH_MODE_HALFDUPLEX;
   networkhandle.Init.MACAddr = myMAC;
   networkhandle.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
   networkhandle.Init.InterruptEnableBits = EIE_LINKIE | EIE_PKTIE;

   #if( ipconfigUSE_LLMNR == 1 )
		{
			/* Also add LLMNR multicast MAC address. */
			networkHandle->Init.MACAddr = xLLMNR_MACAddress;
		}
		#endif	/* ipconfigUSE_LLMNR == 1 */

   printf("Starting network up.\n");
   
   if (!ENC_Start(&networkhandle)) {
      printf("Could not initialise network card.\n");
   } else {
      printf("Setting MAC address to C0:FF:EE:C0:FF:EE.\n");
      

      ENC_SetMacAddr(&networkhandle);

      printf("Network card successfully initialised.\n");
   }
   

   printf("Waiting for ifup...\n");
   while (!(networkhandle.LinkStatus & PHSTAT2_LSTAT)) ENC_IRQHandler(&networkhandle);
   printf("network is up and running.\n");
   
   //register interrupt detection of incoming packets
   isr_register(IRQ_SPI, ETH_PRIORITY, (0x1U << 0x3U), enc28j60_isr);

   // Re-enable global interrupts
   ENC_EnableInterrupts(EIE_INTIE);

   arp_test();

}

// Function to return a pointer to the networkhandle
ENC_HandleTypeDef *encspi_getHandle(void) {
    return &networkhandle;
}

//helping functions
void *arpmemset(void *dest, uint8_t val, uint16_t len)
{
    uint8_t *ptr = dest;
    while (len-- > 0)
       *ptr++ = val;
    return dest;
}

void *arpmemcpy(void *dest, const void *src, uint16_t len)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    while (len--)
       *d++ = *s++;
    return dest;
}

uint8_t arpmemcmp(void *str1, void *str2, unsigned count)
{
    uint8_t *s1 = str1;
    uint8_t *s2 = str2;

    while (count-- > 0)
    {
       if (*s1++ != *s2++)
          return s1[-1] < s2[-1] ? -1 : 1;
    }

    return 0;
}

// TESTING FUNCTIONS
void SendArpPacket(uint8_t *targetIP, uint8_t *deviceMAC)
{
   /* Parameters:
    *   targetIP - The target IP Address for the ARP request (the one whose hardware
    *              address we want)
    *   deviceMAC - The MAC address of the ENC28J60, i.e. the source MAC for the ARP
    *               request
   */
   //printf("Device-MAC is %x:%x:%x:%x:%x:%x \n", deviceMAC[0], deviceMAC[1], deviceMAC[2], deviceMAC[3], deviceMAC[4], deviceMAC[5]);
   ARP arpPacket;

   // The source of the packet will be the ENC28J60 MAC address
   arpmemcpy(arpPacket.eth.SrcAddrs, deviceMAC, 6);
   //printf("Src-MAC is %x:%x:%x:%x:%x:%x \n", arpPacket.eth.SrcAddrs[0], arpPacket.eth.SrcAddrs[1], arpPacket.eth.SrcAddrs[2], arpPacket.eth.SrcAddrs[3], arpPacket.eth.SrcAddrs[4], arpPacket.eth.SrcAddrs[5]);
   // The destination is broadcast - a MAC address of FF:FF:FF:FF:FF:FF
   arpmemset(arpPacket.eth.DestAddrs, 0xFF, 6);
   
   arpPacket.eth.type = ARPPACKET;
   arpPacket.hardware = ETHERNET;
    
   // We want an IP address resolved

   arpPacket.protocol = IPPACKET;
   arpPacket.hardwareSize = sizeof(deviceMAC);
   arpPacket.protocolSize = sizeof(deviceIP);
   arpPacket.opCode = ARPREQUEST;
  
   // Target MAC is set to 0 as it is unknown
   arpmemset(arpPacket.targetMAC, 0, 6);
   //printf("target-MAC is %x:%x:%x:%x:%x:%x \n", arpPacket.targetMAC[0], arpPacket.targetMAC[1], arpPacket.targetMAC[2], arpPacket.targetMAC[3], arpPacket.targetMAC[4], arpPacket.targetMAC[5]);
   // Sender MAC is the ENC28J60's MAC address
   arpmemcpy(arpPacket.senderMAC, deviceMAC, 6);
   //printf("sender-MAC is %x:%x:%x:%x:%x:%x \n", arpPacket.senderMAC[0], arpPacket.senderMAC[1], arpPacket.senderMAC[2], arpPacket.senderMAC[3], arpPacket.senderMAC[4], arpPacket.senderMAC[5]); 
   // The target IP is the IP address we want resolved
   arpmemcpy(arpPacket.targetIP, targetIP, 4);
   printf("target-IP is %d.%d.%d.%d \n", arpPacket.targetIP[0], arpPacket.targetIP[1], arpPacket.targetIP[2], arpPacket.targetIP[3]);
   // Check if the last reply has come from an IP address that we want i.e. someone else is already using it
   if (!arpmemcmp(targetIP, deviceIP, 4)) {
      // Yes, someone is using our IP so set the sender IP to 0.0.0.0
      arpmemset(arpPacket.senderIP, 0, 4);
   } else {
      // No, nobody is using our IP so we can use it confidently
      arpmemcpy(arpPacket.senderIP, deviceIP, 4);
   }

   // Send the packet

   if (ENC_RestoreTXBuffer(&networkhandle, sizeof(ARP)) == 0) {
      printf("Sending ARP request.\n");
      
      
      ENC_WriteBuffer((u8 *)&arpPacket, sizeof(ARP));
      networkhandle.transmitLength = sizeof(ARP);

      ENC_Transmit(&networkhandle);
   }
}

void arp_test(void)
{
   ARP *checkPacket;
   
   SendArpPacket(routerIP, myMAC);

   printf("Waiting for ARP response.\n");
   

   while (1) {
      while (!ENC_GetReceivedFrame(&networkhandle));
      printf("waiting! \n");
      uint16_t len    = networkhandle.RxFrameInfos.length;
      uint8_t *buffer = (uint8_t *)networkhandle.RxFrameInfos.buffer;
      checkPacket     = (ARP *)buffer;

      if (len > 0) {
         if (!memcmp(checkPacket->senderIP, routerIP, 4)) {
            // Success! We have found our router's MAC address

            memcpy(routerMAC, checkPacket->senderMAC, 6);
            printf("Router-MAC is %x:%x:%x:%x:%x:%x \n", routerMAC[0], routerMAC[1], routerMAC[2], routerMAC[3], routerMAC[4], routerMAC[5]);
            break;
         }
      }
   }
}