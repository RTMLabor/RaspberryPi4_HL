/*
 * FreeRTOS+TCP V2.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

/* RPi4 library files. */
#include "enc28j60.h"
#include "encspi.h"
#include "bstatus.h"
#include "timer.h"

/*
#include <xemacps.h>
#include "RPi4/x_topology.h"
#include "RPi4/x_emacpsif.h"
#include "RPi4/x_emacpsif_hw.h"
*/
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef volatile u32 reg32;

/* Xilinx library files. */
/*
#include <xemacps.h>
#include "Zynq/x_topology.h"
#include "Zynq/x_emacpsif.h"
#include "Zynq/x_emacpsif_hw.h"
*/

/* Provided memory configured as uncached. */
// #include "uncached_memory.h"

/*
#ifndef	BMSR_LINK_STATUS
	#define BMSR_LINK_STATUS            0x0004UL
#endif
*/
#ifndef	PHY_LS_HIGH_CHECK_TIME_MS
	// Check if the LinkSStatus in the PHY is still high after 15 seconds of not receiving packets.
	#define PHY_LS_HIGH_CHECK_TIME_MS	15000
#endif

#ifndef	PHY_LS_LOW_CHECK_TIME_MS
	// Check if the LinkSStatus in the PHY is still low every second.
	#define PHY_LS_LOW_CHECK_TIME_MS	1000
#endif

// The size of each buffer when BufferAllocation_1 is used: http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Buffer_Management.html
#define niBUFFER_1_PACKET_SIZE		1536
/*
// Naming and numbering of PHY registers.
#define PHY_REG_01_BMSR			0x01	// Basic mode status register
*/
#ifndef iptraceEMAC_TASK_STARTING
	#define iptraceEMAC_TASK_STARTING()	do { } while( 0 )
#endif

/* Default the size of the stack used by the enc28j60 deferred handler task to twice
the size of the stack used by the idle task - but allow this to be overridden in
FreeRTOSConfig.h as configMINIMAL_STACK_SIZE is a user definable constant. */
#ifndef configEMAC_TASK_STACK_SIZE
	#define configEMAC_TASK_STACK_SIZE ( 2 * configMINIMAL_STACK_SIZE )
#endif


/*-----------------------------------------------------------*/

/*
 * Look for the link to be up every few milliseconds until either xMaxTime time
 * has passed or a link is found.
 */
static BaseType_t prvGMACWaitLS( TickType_t xMaxTime );

/*
 * A deferred interrupt handler for all MAC/DMA interrupt sources.
 */
static void prvEMACHandlerTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* EMAC data/descriptions. */
// see enc28j60.h
ENC_HandleTypeDef *networkHandle =  NULL;


// old zynq code:

extern int phy_detected;

/* A copy of PHY register 1: 'PHY_REG_01_BMSR' */
static uint32_t ulPHYLinkStatus = 0;

#if( ipconfigUSE_LLMNR == 1 )
	static const int xLLMNR_MACAddress[] = { 0x01, 0x00, 0x5E, 0x00, 0x00, 0xFC };
#endif

/* ucMACAddress as it appears in main.c */
extern int ucMACAddress[ 6 ];

/* Holds the handle of the task used as a deferred interrupt processor.  The
handle is used so direct notifications can be sent to the task for all EMAC/DMA
related interrupts. */
TaskHandle_t enc28j60TaskHandle = NULL;


/*-----------------------------------------------------------*/

BaseType_t bNetworkInterfaceInitialise( void )
{
 BaseType_t bStatus, xLinkStatus;
const TickType_t xWaitLinkDelay = pdMS_TO_TICKS( 7000UL ), xWaitRelinkDelay = pdMS_TO_TICKS( 1000UL );
	FreeRTOS_debug_printf(("bNetworkInterfaceInitialise started"));
	/* Guard against the init function being called more than once. */
	if( enc28j60TaskHandle == NULL )
	{
		init_network();
		networkHandle = encspi_getHandle(); // Get pointer to the shared instance	
		/* The deferred interrupt networkHandler task is created at the highest
		possible priority to ensure the interrupt networkHandler can return directly
		to it.  The task's networkHandle is stored in enc28j60TaskHandle so interrupts can
		notify the task when there is something to process. */
		xTaskCreate( prvEMACHandlerTask, "enc28j60", configEMAC_TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &enc28j60TaskHandle );
	}
	else
	{
		/* Initialisation was already performed, just wait for the link. */
		prvGMACWaitLS( xWaitRelinkDelay );
	}

	/* Only return pdTRUE when the Link Status of the PHY is high, otherwise the
	DHCP process and all other communication will fail. */
	xLinkStatus = bGetPhyLinkStatus();

	return ( xLinkStatus != BST_FAILURE );
}
/*-----------------------------------------------------------*/

BaseType_t xNetworkInterfaceInput()
{
	while (!ENC_GetReceivedFrame(encspi_getHandle()));
	return 1;
}
/*-----------------------------------------------------------*/

BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxBuffer, BaseType_t bReleaseAfterSend )
{
	if( ( ulPHYLinkStatus & PHSTAT2_LSTAT ) != 0 )
	{
		iptraceNETWORK_INTERFACE_TRANSMIT();
		// Fill enc28j60-struct
		#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
		{
			/* This driver wants to own all network buffers which are to be transmitted. */
			configASSERT( iReleaseAfterSend != pdFALSE );
		}
		#endif

		//handle-> = pxBuffer->ulIPAddress;

		// Send the packet
		/*
		if (ENC_RestoreTXBuffer(&handle, sizeof(ARP)) == 0) {
			FreeRTOS_printf("Sending Ethernet-package.\n");
			
		*/	
			ENC_WriteBuffer(pxBuffer, sizeof(NetworkBufferDescriptor_t));
			networkHandle->transmitLength = sizeof(NetworkBufferDescriptor_t);

			ENC_Transmit(&networkHandle);
		// }


		//emacps_send_message( &xEMACpsif, pxBuffer, bReleaseAfterSend );
	}
	else if( bReleaseAfterSend != pdFALSE )
	{
		/* No link. */
		vReleaseNetworkBufferAndDescriptor( pxBuffer );
	}

	return pdTRUE;
}
/*-----------------------------------------------------------*/
/*
static inline unsigned long ulReadMDIO( unsigned ulRegister )
{
uint16_t usValue;

	XEmacPs_PhyRead( &( xEMACpsif.emacps ), phy_detected, ulRegister, &usValue );
	return usValue;
}
*/
/*-----------------------------------------------------------*/

static BaseType_t prvGMACWaitLS( TickType_t bMaxTime )
{
TickType_t bStartTime, bEndTime;
const TickType_t bShortDelay = pdMS_TO_TICKS( 20UL );
BaseType_t bReturn;

	bStartTime = xTaskGetTickCount();

	for( ;; )
	{
		bEndTime = xTaskGetTickCount();

		if( bEndTime - bStartTime > bMaxTime )
		{
			bReturn = BST_FAILURE;
			break;
		}
		ulPHYLinkStatus = networkHandle->LinkStatus;

		if( ( ulPHYLinkStatus & PHSTAT2_LSTAT ) != 0 )
		{
			bReturn = BST_SUCCESS;
			break;
		}

		vTaskDelay( bShortDelay );
	}

	return bReturn;
}
/*-----------------------------------------------------------*/

void vNetworkInterfaceAllocateRAMToBuffers( NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] )
{
static uint8_t ucNetworkPackets[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS * niBUFFER_1_PACKET_SIZE ] __attribute__ ( ( aligned( 32 ) ) );
uint8_t *ucRAMBuffer = ucNetworkPackets;
uint32_t ul;

	for( ul = 0; ul < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; ul++ )
	{
		pxNetworkBuffers[ ul ].pucEthernetBuffer = ucRAMBuffer + ipBUFFER_PADDING;
		*( ( uintptr_t * ) ucRAMBuffer ) = ( uintptr_t ) ( &( pxNetworkBuffers[ ul ] ) );
		ucRAMBuffer += niBUFFER_1_PACKET_SIZE;
	}
}
/*-----------------------------------------------------------*/

BaseType_t bGetPhyLinkStatus( void )
{
BaseType_t bStatus;
	ulPHYLinkStatus = networkHandle->LinkStatus;
	if( (ulPHYLinkStatus & PHSTAT2_LSTAT) == 0 )
	{
		bStatus = BST_FAILURE;
	}
	else
	{
		bStatus = BST_SUCCESS;
	}

	return bStatus;
}
/*-----------------------------------------------------------*/

static void prvEMACHandlerTask( void *pvParameters )
{
TimeOut_t xPhyTime;
TickType_t xPhyRemTime;
UBaseType_t uxLastMinBufferCount = 0;
UBaseType_t uxCurrentCount;
BaseType_t xResult = 0;
BaseType_t bStatus;
const TickType_t ulMaxBlockTime = pdMS_TO_TICKS( 100UL );

	/* Remove compiler warnings about unused parameters. */
	( void ) pvParameters;

	/* A possibility to set some additional task properties like calling
	portTASK_USES_FLOATING_POINT() */
	iptraceEMAC_TASK_STARTING();

	vTaskSetTimeOutState( &xPhyTime );
	xPhyRemTime = pdMS_TO_TICKS( PHY_LS_LOW_CHECK_TIME_MS );

	for( ;; )
	{
		uxCurrentCount = uxGetMinimumFreeNetworkBuffers();
		if( uxLastMinBufferCount != uxCurrentCount )
		{
			/* The logging produced below may be helpful
			while tuning +TCP: see how many buffers are in use. */
			uxLastMinBufferCount = uxCurrentCount;
			FreeRTOS_debug_printf( ( "Network buffers: %d lowest %d\n",
				uxGetNumberOfFreeNetworkBuffers(), uxCurrentCount ) );
		}

		#if( ipconfigCHECK_IP_QUEUE_SPACE != 0 )
		{
		static UBaseType_t uxLastMinQueueSpace = 0;

			uxCurrentCount = uxGetMinimumIPQueueSpace();
			if( uxLastMinQueueSpace != uxCurrentCount )
			{
				/* The logging produced below may be helpful
				while tuning +TCP: see how many buffers are in use. */
				uxLastMinQueueSpace = uxCurrentCount;
				FreeRTOS_debug_printf( ( "Queue space: lowest %lu\n", uxCurrentCount ) );
			}
		}
		#endif /* ipconfigCHECK_IP_QUEUE_SPACE */
		
		// Polling for packet
		xResult = xNetworkInterfaceInput();
		FreeRTOS_debug_printf(("ENC-Packet received"));

		/*
		if( ( xEMACpsif.isr_events & EMAC_IF_ALL_EVENT ) == 0 )
		{
			// No events to process now, wait for the next.
			ulTaskNotifyTake( pdFALSE, ulMaxBlockTime );
		}

		// RPi4 port: check if interrupt gpio from enc28j60 is high?!
		if( ( xEMACpsif.isr_events & EMAC_IF_RX_EVENT ) != 0 )
		{
			xEMACpsif.isr_events &= ~EMAC_IF_RX_EVENT;
			xResult = emacps_check_rx( &xEMACpsif );
		}

		if( ( xEMACpsif.isr_events & EMAC_IF_TX_EVENT ) != 0 )
		{
			xEMACpsif.isr_events &= ~EMAC_IF_TX_EVENT;
			emacps_check_tx( &xEMACpsif );
		}

		if( ( xEMACpsif.isr_events & EMAC_IF_ERR_EVENT ) != 0 )
		{
			xEMACpsif.isr_events &= ~EMAC_IF_ERR_EVENT;
			emacps_check_errors( &xEMACpsif );
		}
		*/
		if( xResult > 0 )
		{
			/* A packet was received. No need to check for the PHY status now,
			but set a timer to check it later on. */
			vTaskSetTimeOutState( &xPhyTime );
			xPhyRemTime = pdMS_TO_TICKS( PHY_LS_HIGH_CHECK_TIME_MS );
			xResult = 0;
		}
		else
        {
            /* Delay to avoid excessive CPU usage (adjust as needed) */
            vTaskDelay(ulMaxBlockTime);
        }

		/* --- PHY LINK STATUS CHECK --- */
		if( xTaskCheckForTimeOut( &xPhyTime, &xPhyRemTime ) != pdFALSE )
		{
			bStatus = bGetPhyLinkStatus();
			// Link status changed, when:
			// ( ulPHYLinkStatus & PHSTAT2_LSTAT) != 0) true => Link is up
			// ( bStatus )								true => Link was down

			// ( ulPHYLinkStatus & PHSTAT2_LSTAT) != 0) false => Link is down
			// ( bStatus )								false => Link was up
			if( ( ( ulPHYLinkStatus & PHSTAT2_LSTAT) != 0) == ( bStatus ) )
			{
				ulPHYLinkStatus = bStatus;
				FreeRTOS_debug_printf( ( "prvEMACHandlerTask: PHY LS now %d\n", ( ulPHYLinkStatus & PHSTAT2_LSTAT ) != 0 ) );
			}

			vTaskSetTimeOutState( &xPhyTime );
			if( ( ulPHYLinkStatus & PHSTAT2_LSTAT ) != 0 )
			{
				xPhyRemTime = pdMS_TO_TICKS( PHY_LS_HIGH_CHECK_TIME_MS );
			}
			else
			{
				xPhyRemTime = pdMS_TO_TICKS( PHY_LS_LOW_CHECK_TIME_MS );
			}
		}
	}
}
/*-----------------------------------------------------------*/
