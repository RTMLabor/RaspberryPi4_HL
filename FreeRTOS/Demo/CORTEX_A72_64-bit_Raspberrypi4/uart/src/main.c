#include <stddef.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_TCP_server.h"
#include "FreeRTOS_DHCP.h"

// driver includes
#include "uart.h"
#include "spi0.h"
#include "printf.h"
#include "enc28j60.h"
#include "bcm_string.h"
#include "bcm_io.h"

// From ErrorCodes.h (Enclustra)
typedef enum EN_RESULT
{
    EN_SUCCESS = 0x00000000,

    EN_ERROR_NULL_POINTER = 0x80000000,
    EN_ERROR_INVALID_ARGUMENT,
    EN_ERROR_FAILED_TO_SET_I2C_DEVICE_ADDRESS,
    EN_ERROR_FAILED_TO_INITIALISE_I2C_CONTROLLER,
    EN_ERROR_FAILED_TO_START_XIIC_DEVICE,
    EN_ERROR_FAILED_TO_STOP_XIIC_DEVICE,
    EN_ERROR_FAILED_TO_INITIALISE_INTERRUPT_CONTROLLER,
    EN_ERROR_FAILED_TO_START_INTERRUPT_CONTROLLER,
    EN_ERROR_XIIC_SEND_FAILED,
    EN_ERROR_XIIC_RECEIVE_FAILED,
    EN_ERROR_I2C_READ_FAILED,
    EN_ERROR_I2C_WRITE_FAILED,
    EN_ERROR_I2C_READ_TIMEOUT,
    EN_ERROR_I2C_WRITE_TIMEOUT,
    EN_ERROR_I2C_SLAVE_NACK,
    EN_ERROR_TIMER_INITIALISATION_FAILED,
    EN_ERROR_TIMER_SELF_TEST_FAILED,
    EN_ERROR_INVALID_MODULE_CONFIG_PROPERTY_INDEX,
    EN_ERROR_MODULE_CONFIG_PROPERTY_VALUE_OUT_OF_RANGE,
    EN_ERROR_MODULE_CONFIG_PROPERTIES_NOT_YET_READ,
    EN_ERROR_MODULE_CONFIG_PROPERTY_DOES_NOT_HAVE_VALUE_KEY,
    EN_ERROR_FAILED_TO_INITIALISE_EEPROM,
    EN_ERROR_FAILED_TO_WAKE_ATMEL_ATSHA204A,
    EN_ERROR_ATSHA204A_INVALID_ADDRESS_PARAMETER,
    EN_ERROR_ATSHA204A_INVALID_RESPONSE_CRC,
    EN_ERROR_ATSHA204A_INVALID_RESPONSE_SIZE,
    EN_ERROR_ATSHA204A_EXECUTION_ERROR,
    EN_ERROR_ATSHA204A_FIRST_COMMAND_AFTER_WAKE,
    EN_ERROR_ATSHA204A_INVALID_MAC,
    EN_ERROR_ATSHA204A_IO_ERROR,
    EN_ERROR_ATSHA204A_PARSE_ERROR,
    EN_ERROR_ATSHA204A_UNKNOWN_ERROR,
    EN_ERROR_ATSHA204A_FUNCTIONALITY_NOT_YET_IMPLEMENTED,
    EN_ERROR_ATSHA204A_WRITE_VERIFICATION_ERROR,
    EN_ERROR_ATSHA204A_INVALID_DATA_SLOT_INDEX,
    EN_ERROR_RTC_DEVICE_NOT_DETECTED,
    EN_ERROR_RTC_FEATURE_NOT_SUPPORTED,
    EN_ERROR_RTC_NOT_WORKING,
    EN_ERROR_IOTEST_FAILED,
    EN_ERROR_SUPPLY_OUT_OF_RANGE

} EN_RESULT;

#define STACK_SIZE			( configMINIMAL_STACK_SIZE * 3 )

/* Define names that will be used for SDN, LLMNR and NBNS searches. */
// defined in makefile DmainHOST
#ifndef mainHOST_NAME
#define mainHOST_NAME					"RaspberryPi4"
#define mainDEVICE_NICK_NAME			"RaspberryPi4"
#define mainDEVICE_NICK_NAME			mainHOST_NAME
#define TARGET_NAME						"RaspberryPi4"
#endif

#define INTC_DEVICE_INT_ID	0x0E

#define CPU1STARTADR 0xfffffff0
#define sev() __asm__("sev")

/* FTP and HTTP servers execute in the TCP server work task.  */
#define mainTCP_SERVER_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )
#define	mainTCP_SERVER_STACK_SIZE		1600 /* Not used in the Win32 simulator. */

/* The number and size of sectors that will make up the RAM disk. */
#define mainRAM_DISK_SECTOR_SIZE		512UL /* Currently fixed! */
#define mainRAM_DISK_SECTORS			( ( 5UL * 1024UL * 1024UL ) / mainRAM_DISK_SECTOR_SIZE ) /* 5M bytes. */
#define mainIO_MANAGER_CACHE_SIZE		( 15UL * mainRAM_DISK_SECTOR_SIZE )

/* Where the SD and RAM disks are mounted.  As set here the SD card disk is the
root directory, and the RAM disk appears as a directory off the root. */
#define mainSD_CARD_DISK_NAME			"/"
#define mainRAM_DISK_NAME				"/ram"


//addresses for OCM communication
#define BUFFER_SIZE 0x1000

#define BASE_ADDRESS 0xFFFF0000
#define READ_FLAG_ADDRESS BASE_ADDRESS
#define BYTES_TO_RECEIVE_ADDRESS READ_FLAG_ADDRESS+4
#define BYTES_RECEIVED_ADDRESS BYTES_TO_RECEIVE_ADDRESS+4
#define BUFFER1_BASE_ADDRESS BYTES_RECEIVED_ADDRESS +4
#define WRITE_FLAG_ADDRESS BUFFER1_BASE_ADDRESS+BUFFER_SIZE
#define BYTES_TO_SEND_ADDRESS WRITE_FLAG_ADDRESS+4
#define BYTES_SENT_ADDRESS BYTES_TO_SEND_ADDRESS+4
#define BUFFER2_BASE_ADDRESS BYTES_SENT_ADDRESS+4

#define RECVBUFSIZE 1460
#define SENDBUFSIZE 4

#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define INTC_DIST_BASE_ADDR	XPAR_SCUGIC_DIST_BASEADDR

#define TTC_INTR_ID		    XPAR_XTTCPS_0_INTR


#define DOWNLOAD_ACCEPT_TIMEOUT_MS	2000
#define CORE1_START_ADDR			0x00110000				// start address of the core1 program
#define RESERVERD_SPACE				0x10000					// reserved space for program size and other non-application data
#define CORE1_PRG_SIZE				CORE1_START_ADDR-RESERVERD_SPACE	// address where the size of the program or the FLASH_INVALID value is stored 
#define FLASH_CONFIGDATA			0xEC0000
#define FLASH_ADDRESS				0xF00000				// address where program size and program are stored in the flash
#define FLASH_INVALID				0xFFFFDEAD				// value which indicates that the flash contents are invalid
#define FLASH_UNINITIALIZED			0xFFFFFFFF				// value which indicates that the flash is not initialized with 1's
/*-----------------------------------------------------------*/

/* 
 * Prototypes for the standard FreeRTOS callback/hook functions implemented
 * within this file.
 */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );

static inline void io_halt(void)
{
    asm volatile ("wfi");
    return;
}
/*-----------------------------------------------------------*/

void TaskA(void *pvParameters)
{
    (void) pvParameters;
	char taskListBuffer[512]; // Adjust size as needed

    
    //NetworkBufferDescriptor_t nwBuffer;
    //nwBuffer.ulIPAddress

    for( ;; )
    {
        uart_puts("\r\n");
        uart_puthex(xTaskGetTickCount());
        uart_puts("\r\n");
		// Get task info and store it in the buffer
    	vTaskList(taskListBuffer);
		// Print the task list
		printf("Task Name\tState\tPrio-\tStack\tTask \n \t \t \trity \tLeft \tNumber \n");
		printf("-------------------------------------------------------------\n");
		printf("%s\n", taskListBuffer);
	
        //vARPGenerateRequestPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer );
        vTaskDelay(5000 / portTICK_RATE_MS);
    }

    return; /* Never reach this line */
}
/*-----------------------------------------------------------*/

void initTask(void *pvParameters)
{
    (void) pvParameters;
    

    vTaskDelete(NULL);
}

void TaskB(void *pvParameters)
{
    (void) pvParameters;

    for( ;; )
    {
        uart_puts("\r\n");
        uart_puts("GPIO-Interrupt-Task!");
        uart_puts("\r\n");
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    return; /* Never reach this line */
}
/*-----------------------------------------------------------*/

TimerHandle_t timer;
uint32_t count=0;
void interval_func(TimerHandle_t pxTimer)
{
    (void) pxTimer;
    uint8_t buf[2] = {0};
    uint32_t len = 0;

    len = uart_read_bytes(buf, sizeof(buf) - 1);
    if (len)
        uart_puts((char *)buf);

    return;
}
/*-----------------------------------------------------------*/
#define mfcp(reg, result) __asm__ __volatile__ ("mrs %0, " #reg : "=r" (result))

/*
 * The xApplicationDHCPHook determines the course of action during various phases of the DCHP.
 */
eDHCPCallbackAnswer_t xApplicationDHCPHook( eDHCPCallbackPhase_t eDHCPPhase,
                                            uint32_t ulIPAddress );

int download(void);
void ocmReadWriteTask (void * pvParameters);
void waitForUDPResetTask(void * pvParameters);
void initializeTCPSocketsTask(void * pvParameters);
unsigned int readNetworkAdressString(const char* start, char* targetAddressArray);
void rebootTask (void * pvParameters);
void vAssertCalled( const char *pcFile, uint32_t ulLine );
static void SetupInterruptSystem();
void communicationRequestISR(void *CallBackRef);
void initTask (void * pvParameters);
const char *getTargetName( void );
EN_RESULT ReadEeprom();
void initInterruptController();

/* The default IP and MAC address used by the demo.  The address configuration
defined here will be used if ipconfigUSE_DHCP is 0, or if ipconfigUSE_DHCP is
1 but a DHCP server could not be contacted.  See the online documentation for
more information. */
static char hostNameBuffer[256] = mainHOST_NAME;
static char configBuffer[261];
static uint8_t ucIPAddress[ 4 ] = { configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3 };
static uint8_t ucNetMask[ 4 ] = { configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3 };
static uint8_t ucGatewayAddress[ 4 ] = { configGATEWAY_ADDR0, configGATEWAY_ADDR1, configGATEWAY_ADDR2, configGATEWAY_ADDR3 };
static uint8_t ucDNSServerAddress[ 4 ] = { configDNS_SERVER_ADDR0, configDNS_SERVER_ADDR1, configDNS_SERVER_ADDR2, configDNS_SERVER_ADDR3 };
static BaseType_t xTasksAlreadyCreated = pdFALSE;
static char macAddrString[18];
static uint8_t useDHCP = 1;
/* Default MAC address configuration. */
uint8_t ucMACAddress[ 6 ] = { configMAC_ADDR0, configMAC_ADDR1, configMAC_ADDR2, configMAC_ADDR3, configMAC_ADDR4, configMAC_ADDR5 };

/* Use by the pseudo random number generator. */
static UBaseType_t ulNextRand;


/* The interrupt controller is initialised in this file, and made available to
other modules. */
//BcmGIC bInterruptController;    /* Interrupt controller instance */


static TaskHandle_t ocmReadWriteTaskHandle = NULL;
static TaskHandle_t initTaskHandle = NULL;
static TaskHandle_t waitForUDPResetTaskHandle = NULL;
static TaskHandle_t initializeTCPSocketsTaskHandle = NULL;


struct freertos_sockaddr xClient, xBindAddress;
Socket_t xListeningSocket_10310, xConnectedSocket_10310, xListeningSocket_10400, xConnectedSocket_10400;
socklen_t xSize = sizeof( xClient );
static const TickType_t xReceiveTimeOut = portMAX_DELAY;
static const TickType_t xDownloadReceiveTimeOut = DOWNLOAD_ACCEPT_TIMEOUT_MS/portTICK_PERIOD_MS;
const BaseType_t xBacklog = 20;


/* See http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCPIP_FAT_Examples_Xilinx_Zynq.html */
register char * stack_ptr asm ("sp");
SemaphoreHandle_t xSemaphore = NULL;
char LEFT_PADDING[] = "   ";

/* Handle of the task that runs the FTP and HTTP servers. */
static TaskHandle_t xServerWorkTaskHandle = NULL;

void main(void)
{
    TaskHandle_t initTask, task_a, task_b;

    uart_init();
    init_printf(0, putc);
    uart_puts("\r\n****************************\r\n");
    uart_puts("\r\n    FreeRTOS UART Sample\r\n");
    uart_puts("\r\n  (This sample uses UART2)\r\n");
    uart_puts("\r\n****************************\r\n");

    
    printf("Initializing SPI...\n");
    spi0_init();
    xTaskCreate(initTask, "initTask", STACK_SIZE, NULL, tskIDLE_PRIORITY, &initTask);

    xTaskCreate(TaskA, "Task A", 512, NULL, 0x10, &task_a);
    //xTaskCreate(TaskB, "Task B", 512, NULL, 0x10, &task_b);
    

    timer = xTimerCreate("print_every_10ms",(10 / portTICK_RATE_MS), pdTRUE, (void *)0, interval_func);
    if(timer != NULL)
    {
        xTimerStart(timer, 0);
    }

    /*
    uart_puts("Current_EL:");
    uart_puthex(get_el());
    uart_puts("\n");
    uart_puts("FreeRTOS_task created, timer created\n");
    uint64_t result;    
	mfcp(daif, result);
	uart_puts("daif: 0x");
    uart_puthex(result);
    uart_puts("\n");
	mfcp(cpacr_el1, result);
	uart_puts("cpacr_el1: 0x");
    uart_puthex(result);
    uart_puts("\n");
	mfcp(sctlr_el1, result);
	uart_puts("sctlr_el1: 0x");
    uart_puthex(result);
    uart_puts("\n");
	//mfcp(hcr_el2, result);
	//printf("hcr_el2: 0x%x\n", result);
	//mfcp(scr_el3, result);
	//printf("scr_el3: 0x%x\n", result);
	mfcp(TCR_EL1, result);
	uart_puts("TCR_EL1: 0x");
    uart_puthex(result);
    uart_puts("\n");
	mfcp(MAIR_EL1, result);
	uart_puts("MAIR_EL1: 0x");
    uart_puthex(result);
    uart_puts("\n");
	mfcp(ttbr0_el1, result);
	uart_puts("ttbr0_el1: 0x");
    uart_puthex(result);
    uart_puts("\n");
    uart_puts("start scheduler\n");
    */
   
    // initialize IP Stack
    printf("%s",FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress ));
    
    vTaskStartScheduler();
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
}

/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
}
/*-----------------------------------------------------------*/

const char *pcApplicationHostnameHook( void )
{
	/* Assign the specified hostname to this network node.  This function will be
	called during the DHCP: the machine will be registered with an IP address
	plus this name. */
	return hostNameBuffer;
}
/*-----------------------------------------------------------*/
/**
 * @brief Callback function for handling different phases of the DHCP process.
 * 
 * This function is called during various stages of the DHCP process, as identified by the 
 * `eDHCPPhase` parameter. It allows customization of the DHCP behavior based on whether DHCP 
 * should be used or a static IP address should be configured instead. The function decides 
 * whether the DHCP process should continue, use default IP configurations, or retain the 
 * current network settings.
 * 
 * @param eDHCPPhase The current phase of the DHCP process (e.g., eDHCPPhasePreDiscover, eDHCPPhasePreRequest).
 * @param ulIPAddress The IP address being processed in the current phase.
 * 
 * @return eDHCPCallbackAnswer_t The next action to take in the DHCP process:
 *         - eDHCPContinue: Continue with the DHCP process.
 *         - eDHCPUseDefaults: Stop DHCP process and use statically configured IP address.
 *         - eDHCPStopNoChanges: Stop DHCP process and retain the current network configuration.
 * 
 * @note This function is used to control DHCP behavior and customize the network configuration
 *       process, particularly when deciding whether to use DHCP or a static IP.
 */
eDHCPCallbackAnswer_t xApplicationDHCPHook( eDHCPCallbackPhase_t eDHCPPhase,
                                            uint32_t ulIPAddress )
{
eDHCPCallbackAnswer_t eReturn;
uint32_t ulStaticIPAddress, ulStaticNetMask;

  /* This hook is called in a couple of places during the DHCP process, as
  identified by the eDHCPPhase parameter. */
  switch( eDHCPPhase )
  {
    case eDHCPPhasePreDiscover  :
      /* A DHCP discovery is about to be sent out.  eDHCPContinue is
      returned to allow the discovery to go out.

      If eDHCPUseDefaults had been returned instead then the DHCP process
      would be stopped and the statically configured IP address would be
      used.

      If eDHCPStopNoChanges had been returned instead then the DHCP
      process would be stopped and whatever the current network
      configuration was would continue to be used.*/
	  if(useDHCP == 1)
	  {
		  eReturn = eDHCPContinue;
	  }
      else
	  {
		  eReturn = eDHCPUseDefaults;
	  }
      break;

    case eDHCPPhasePreRequest  :
  
 
	  eReturn = eDHCPContinue;
      

      break;

    default :
      // Cannot be reached, but set eReturn to prevent compiler warnings where compilers are disposed to generating one.
      eReturn = eDHCPContinue;
      break;
  }

  return eReturn;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	vAssertCalled( __FILE__, __LINE__ );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

/*-----------------------------------------------------------*/
/**
 * @brief Generates a pseudo-random number.
 * 
 * This function generates a pseudo-random number using a simple linear congruential generator (LCG) algorithm.
 * It takes the current value of `ulNextRand`, multiplies it by a constant multiplier, and adds a constant increment
 * to produce the next random number. The result is then shifted and masked to return a 15-bit value.
 * 
 * @return UBaseType_t A pseudo-random 15-bit number.
 * 
 * @note This function does not provide cryptographically secure random numbers, 
 *       and is suitable for general random number generation in embedded applications.
 */

UBaseType_t uxRand( void )
{
const uint32_t ulMultiplier = 0x015a4e35UL, ulIncrement = 1UL;

	/* Utility function to generate a pseudo random number. */

	ulNextRand = ( ulMultiplier * ulNextRand ) + ulIncrement;
	return( ( int ) ( ulNextRand >> 16UL ) & 0x7fffUL );
}
/*-----------------------------------------------------------*/
/**
 * @brief Hook function to handle DNS queries for the device.
 * 
 * This function is called during a DNS query to determine whether the queried
 * hostname matches the device's hostname. The function checks if the provided
 * `pcName` (the queried hostname) matches either the hostname returned by 
 * `pcApplicationHostnameHook()` or the name set by `mainDEVICE_NICK_NAME`.
 * If a match is found, the function returns `pdPASS` to indicate a successful match.
 * Otherwise, it returns `pdFAIL` to indicate the query did not match.
 * 
 * @param pcName The hostname being queried.
 * 
 * @return BaseType_t `pdPASS` if the queried hostname matches the device's
 *                   hostname, otherwise `pdFAIL`.
 * 
 * @note This hook can be used to control the behavior of DNS queries for
 *       the device and is useful for applications that need to handle custom
 *       DNS hostname resolution.
 */
BaseType_t xApplicationDNSQueryHook( const char *pcName )
{
BaseType_t xReturn;

	/* Determine if a name lookup is for this node.  Two names are given
	to this node: that returned by pcApplicationHostnameHook() and that set
	by mainDEVICE_NICK_NAME. */
	if( strcasecmp( pcName, pcApplicationHostnameHook() ) == 0 )
	{
		xReturn = pdPASS;
	}
	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

void vAssertCalled( const char *pcFile, uint32_t ulLine )
{
volatile uint32_t ulBlockVariable = 0UL;
volatile char *pcFileName = ( volatile char *  ) pcFile;
volatile uint32_t ulLineNumber = ulLine;

	( void ) pcFileName;
	( void ) ulLineNumber;

	printf("vAssertCalled( %s, %ld\r\n", pcFile, ulLine);

	/* Setting ulBlockVariable to a non-zero value in the debugger will allow
	this function to be exited. */
	taskDISABLE_INTERRUPTS();
	{
		while( ulBlockVariable == 0UL )
		{
			__asm volatile( "NOP" );
		}
	}
	taskENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

/* Called by FreeRTOS+TCP when the network connects or disconnects.  Disconnect
events are only received if implemented in the MAC driver. */

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
char cBuffer[ 16 ];
	// If the network has just come up...
	if( eNetworkEvent == eNetworkUp )
	{
		// Create the tasks that use the IP stack if they have not already been created. 
		FreeRTOS_GetAddressConfiguration( &ulIPAddress, &ulNetMask, &ulGatewayAddress, &ulDNSServerAddress );
		FreeRTOS_inet_ntoa( ulIPAddress, cBuffer );
		printf( "IP Address: %s\r\n", cBuffer);

		FreeRTOS_inet_ntoa( ulNetMask, cBuffer );
		printf("Subnet Mask: %s\r\n", cBuffer);

		FreeRTOS_inet_ntoa( ulGatewayAddress, cBuffer );
		printf("Gateway Address: %s\r\n", cBuffer);

		FreeRTOS_inet_ntoa( ulDNSServerAddress, cBuffer );
		printf("DNS Server Address: %s\r\n\r\n\r\n", cBuffer);
		if( xTasksAlreadyCreated == pdFALSE )
		{

			xTaskCreate( initializeTCPSocketsTask, "initializeTCPSocketsTask", STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &initializeTCPSocketsTaskHandle );

			if( xServerWorkTaskHandle != NULL )
			{
				xTaskNotifyGive( xServerWorkTaskHandle );
			}

			// Register example commands with the FreeRTOS+CLI command interpreter via the UDP port specified by the
			// mainUDP_CLI_PORT_NUMBER constant. 
			#if( mainCREATE_UDP_CLI_TASKS == 1 )
			{
				vRegisterSampleCLICommands();
				vRegisterFileSystemCLICommands();
				vRegisterTCPCLICommands();
				vStartUDPCommandInterpreterTask( mainUDP_CLI_TASK_STACK_SIZE, mainUDP_CLI_PORT_NUMBER, mainUDP_CLI_TASK_PRIORITY );
			}
			#endif

			#if( mainCREATE_TCP_ECHO_CLIENT_TASKS_SINGLE == 1 )
			{
				vStartTCPEchoClientTasks_SingleTasks( mainECHO_CLIENT_TASK_STACK_SIZE, mainECHO_CLIENT_TASK_PRIORITY );
			}
			#endif

			#if( mainCREATE_SIMPLE_TCP_ECHO_SERVER == 1 )
			{
				vStartSimpleTCPServerTasks( mainECHO_SERVER_STACK_SIZE, mainECHO_SERVER_TASK_PRIORITY );
			}
			#endif

			// Start a new task to fetch logging lines and send them out. 
			#if( mainCREATE_UDP_LOGGING_TASK == 1 )
			{
				vUDPLoggingTaskCreate();
			}
			#endif

			xTasksAlreadyCreated = pdTRUE;
		}


	}
	
	if( eNetworkEvent == eNetworkDown )
	{
		printf("Network Down\r\n");
	}
}
/*-----------------------------------------------------------*/

/* 	The ocmReadWriteTask is used relay the network packets from
	CAMeL into the OCM and vice versa. The implementation uses
	flags to signal receive and send readiness. The Task waits
	for an incoming TCP connection on port 10310 and then starts
	relaying the packets. The task yields until there is a software
	interrupt from Ipanema on Core1 which signals a request for 
	send/receive 												*/

void ocmReadWriteTask (void * pvParameters)
{
		
	( void ) pvParameters;
	printf("ocmReadWriteTask was started! \n");
	// wait for incoming connection on port 10310
	xConnectedSocket_10310 = FreeRTOS_accept( xListeningSocket_10310, &xClient, &xSize );
	if(xConnectedSocket_10310 == FREERTOS_INVALID_SOCKET)
	{
		printf ("Invalid xConnectedSocket_10310\r\n");
	}
	else
	{
		printf ("Created xConnectedSocket_10310\r\n");
	}
    
    configASSERT( xConnectedSocket_10310 != FREERTOS_INVALID_SOCKET );
	
	
	// set the buffers to memory addresses in OCM
	uint8_t *buf1 = (uint8_t*)BUFFER1_BASE_ADDRESS;
	int bytesToReceive;
	int lBytesReceived = 0;
	
	uint8_t *buf2 = (uint8_t*)BUFFER2_BASE_ADDRESS;
	int bytesToSend;
	int xBytesSent;
	
	for(;;){
		

		// wait for the semaphore from the ISR
		if(xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
		{
			

			// if socket is not connected, reconnect
			if(FreeRTOS_issocketconnected(xConnectedSocket_10310)!=pdTRUE){
				printf ("Reconnecting\r\n");
				FreeRTOS_closesocket( xConnectedSocket_10310 );
				xConnectedSocket_10310 = FreeRTOS_accept( xListeningSocket_10310, &xClient, &xSize );
				if(xConnectedSocket_10310 == FREERTOS_INVALID_SOCKET)
				{
					printf ("Invalid xConnectedSocket_10310\r\n");
				}
				else
				{
					printf ("Created xConnectedSocket_10310\r\n");
				}
				
				configASSERT( xConnectedSocket_10310 != FREERTOS_INVALID_SOCKET );			
			}			
			
			//wait for CPU1 to signal a receive request
			if(Bcm_In32(READ_FLAG_ADDRESS)==2)
			{
				bytesToReceive = Bcm_In32(BYTES_TO_RECEIVE_ADDRESS);		// read the number of bytes to receive from the TCP socket
				lBytesReceived = FreeRTOS_recv( xConnectedSocket_10310, buf1, bytesToReceive, 0 );
				Bcm_Out32(BYTES_RECEIVED_ADDRESS, lBytesReceived);			// store the number of actually received bytes in OCM
				Bcm_Out32(READ_FLAG_ADDRESS, 1);							// signal CPU1 that the receive operation is finished
			}		
			
			// wait for CPU1 to signal a send request
			if(Bcm_In32(WRITE_FLAG_ADDRESS)==2)
			{
				bytesToSend = Bcm_In32(BYTES_TO_SEND_ADDRESS);				// read the number of bytes to send to the TCP socket
				xBytesSent = FreeRTOS_send(xConnectedSocket_10310, buf2, bytesToSend, 0 );
				Bcm_Out32(BYTES_SENT_ADDRESS, xBytesSent);					// store the number of actually sent bytes in OCM
				Bcm_Out32(WRITE_FLAG_ADDRESS, 1);							// signal CPU1 that the send operation is finished
			}
				
		}
		

	
	}
}

/* 	This function opens the TCP sockets for port 10310 and 10400.
	After that the Tasks ocmReadWriteTask and waitForUDPResetTask
	are started.												*/
void initializeTCPSocketsTask(void * pvParameters)
{
    ( void ) pvParameters;
	printf("initializeTCPSocketsTask was started! \n");
    // Attempt to open the socket. 
	xListeningSocket_10310 = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP );
	if(xListeningSocket_10310 == FREERTOS_INVALID_SOCKET)
	{
		printf ("Invalid xListeningSocket_10310\r\n");
	}
	else
	{
		printf ("Created xListeningSocket_10310\r\n");
	}
    // Check the socket was created. 
    configASSERT( xListeningSocket_10310 != FREERTOS_INVALID_SOCKET );

    // If FREERTOS_SO_RCVBUF or FREERTOS_SO_SNDBUF are to be used with
    // FreeRTOS_setsockopt() to change the buffer sizes from their default then do
    // it here!.  (see the FreeRTOS_setsockopt() documentation. 

    // If ipconfigUSE_TCP_WIN is set to 1 and FREERTOS_SO_WIN_PROPERTIES is to
    // be used with FreeRTOS_setsockopt() to change the sliding window size from
    // its default then do it here! (see the FreeRTOS_setsockopt()
    // documentation. 

    // Set a time out so accept() will just wait for a connection. 
 	FreeRTOS_setsockopt( xListeningSocket_10310, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );


    // Set the listening port to 10310. 
    xBindAddress.sin_port = ( uint16_t ) 10310;
    xBindAddress.sin_port = FreeRTOS_htons( xBindAddress.sin_port );

    // Bind the socket to the port that the client RTOS task will send to. 
    FreeRTOS_bind( xListeningSocket_10310, &xBindAddress, sizeof( xBindAddress ) );

    // Set the socket into a listening state so it can accept connections.
    // The maximum number of simultaneous connections is limited to 20. 
    FreeRTOS_listen( xListeningSocket_10310, xBacklog );
	
	
	
	// Attempt to open the socket. 
	xListeningSocket_10400 = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP );
	if(xListeningSocket_10400 == FREERTOS_INVALID_SOCKET)
	{
		printf ("Invalid xListeningSocket_10400\r\n");
	}
	else
	{
		printf ("Created xListeningSocket_10400\r\n");
	}
    // Check the socket was created. 
    configASSERT( xListeningSocket_10400 != FREERTOS_INVALID_SOCKET );

    // If FREERTOS_SO_RCVBUF or FREERTOS_SO_SNDBUF are to be used with
    // FreeRTOS_setsockopt() to change the buffer sizes from their default then do
    // it here!.  (see the FreeRTOS_setsockopt() documentation. 

    // If ipconfigUSE_TCP_WIN is set to 1 and FREERTOS_SO_WIN_PROPERTIES is to
    // be used with FreeRTOS_setsockopt() to change the sliding window size from
    // its default then do it here! (see the FreeRTOS_setsockopt()
    // documentation. 

    // Set a time out so accept() will just wait for a connection. 
 	FreeRTOS_setsockopt( xListeningSocket_10400, 0, FREERTOS_SO_RCVTIMEO, &xDownloadReceiveTimeOut, sizeof( xDownloadReceiveTimeOut ) );


    // Set the listening port to 10400. 
    xBindAddress.sin_port = ( uint16_t ) 10400;
    xBindAddress.sin_port = FreeRTOS_htons( xBindAddress.sin_port );

    // Bind the socket to the port that the client RTOS task will send to. 
    FreeRTOS_bind( xListeningSocket_10400, &xBindAddress, sizeof( xBindAddress ) );

    // Set the socket into a listening state so it can accept connections.
    // The maximum number of simultaneous connections is limited to 20. 
    FreeRTOS_listen( xListeningSocket_10400, xBacklog );

  	xTaskCreate( ocmReadWriteTask, "ocmReadWriteTask", STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &ocmReadWriteTaskHandle );
	xTaskCreate( waitForUDPResetTask, "waitForUDPResetTask", STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &waitForUDPResetTaskHandle );
	vTaskDelete(NULL);
}


/*-----------------------------------------------------------*/

/* Called automatically when a reply to an outgoing ping is received. */

void vApplicationPingReplyHook( ePingReplyStatus_t eStatus, uint16_t usIdentifier )
{
static const char *pcSuccess = "Ping reply received - ";
static const char *pcInvalidChecksum = "Ping reply received with invalid checksum - ";
static const char *pcInvalidData = "Ping reply received with invalid data - ";

	switch( eStatus )
	{
		case eSuccess	:
			FreeRTOS_printf( ( pcSuccess ) );
			break;

		case eInvalidChecksum :
			FreeRTOS_printf( ( pcInvalidChecksum ) );
			break;

		case eInvalidData :
			FreeRTOS_printf( ( pcInvalidData ) );
			break;

		default :
			// It is not possible to get here as all enums have their own
			//case.
			break;
	}


	// Prevent compiler warnings in case FreeRTOS_debug_printf() is not defined.
	( void ) usIdentifier;
}

/*-----------------------------------------------------------*/

/* 	The waitForUDPResetTask waits for an incoming UDP packet
	at port 10300 in a loop. Depending in the string received three 
	actions can be done:
	- Soft reset CPU1 and download a program from CAMeL into RAM
	  then start CPU1 execution
	- Soft reset CPU1 and download a program from CAMeL into RAM,
	  additionally program it into the flash
	- Soft reset CPU1 and erase the flash */

void waitForUDPResetTask(void * pvParameters)
{
	#if 0
	( void ) pvParameters;


	xSocket_t xSocket;
	struct freertos_sockaddr xBindAddress;
	
	xSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );
 	FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );

    // Check the socket was created successfully. 
    configASSERT( xSocket != FREERTOS_INVALID_SOCKET )
  
	/* The socket was created successfully and can now be used to send data
	using the FreeRTOS_sendto() API function.  Sending to a socket that has
	not first been bound will result in the socket being automatically bound
	to a port number.  Use FreeRTOS_bind() to bind the socket to a
	specific port number.  This example binds the socket to port 9999.  The
	port number is specified in network byte order, so FreeRTOS_htons() is
	used. */ 
	xBindAddress.sin_port = FreeRTOS_htons( 10300 );
	configASSERT( FreeRTOS_bind( xSocket, &xBindAddress, sizeof( &xBindAddress ) ) == 0 )
	
	char ucBuffer[ 128 ];
	int8_t cIPAddressString[ 16 ];
	struct freertos_sockaddr xSourceAddress;
	int32_t iReturned = 1000;
	int fileSize;
	socklen_t sourceAddressSize = sizeof( xSourceAddress );
	int stringSize = 0;
	/* Receive into the buffer with ulFlags set to 0, so the FREERTOS_ZERO_COPY bit
	is clear. */ 
	for(;;){
		
		printf("Listening for message from CAMeL-View\r\n");
		iReturned = FreeRTOS_recvfrom(
										// The socket data is being received on. 
										xSocket,
										/* The buffer into which received data will be
										copied. */
										ucBuffer,
										/* The length of the buffer into which data will be
										copied. */ 
										128,
										// ulFlags with the FREERTOS_ZERO_COPY bit clear. 
										0,
										// Will get set to the source of the received data. 
										&xSourceAddress,
										// Not used but should be set as shown. 
										&sourceAddressSize
								   );
		ucBuffer[iReturned] = '\0';
		

		if( iReturned > 0 )
		{
			/* Data was received from the socket.  Prepare the IP address for
			printing to the console by converting it to a string. */
			FreeRTOS_inet_ntoa( xSourceAddress.sin_addr, ( char * ) cIPAddressString );

			// Print out details of the data source. 
			printf( "Received %d bytes from IP address %s port number %d\r\n",
						(int)iReturned, // The number of bytes received. 
						cIPAddressString, // The IP address that sent the data. 
						FreeRTOS_ntohs( xSourceAddress.sin_port ) ); // The source port. 
			printf("Received String: %s\r\n", ucBuffer);			
			if (strncmp(ucBuffer,"QUERY_TARGET_INFO", 17)==0) {
				uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
				char ipAddrBuffer[ 16 ];
				// If the network has just come up...

				/* Create the tasks that use the IP stack if they have not already been
				created. */
				FreeRTOS_GetAddressConfiguration( &ulIPAddress, &ulNetMask, &ulGatewayAddress, &ulDNSServerAddress );
				FreeRTOS_inet_ntoa( ulIPAddress, ipAddrBuffer );
				stringSize = sprintf(ucBuffer,
					"HOSTNAME: %s\r\n"
					"IP_ADDR: %s\r\n"
					"MAC_ADDR: %s\r\n"
					"TARGET: %s\r\n"
					, pcApplicationHostnameHook(), ipAddrBuffer, macAddrString, getTargetName());
				printf("Sent to Host:\r\n%s\r\n", ucBuffer);
				iReturned = FreeRTOS_sendto(
						// The socket being send to. 
						xSocket,
						// The data being sent. 
						ucBuffer,
						// The length of the data being sent. 
						stringSize+1,
						// ulFlags with the FREERTOS_ZERO_COPY bit clear. 
						0,
						// Where the data is being sent. 
						&xSourceAddress,
						// Not used but should be set as shown. 
						sizeof( xSourceAddress )
					 );													
			}
			
			if (strncmp(ucBuffer,"DISABLE_L2CACHE", 15)==0) {
				Bcm_L2CacheDisable();
				stringSize = sprintf(ucBuffer, "L2CACHE_DISABLED");
				printf("L2CACHE disabled.\n");
				iReturned = FreeRTOS_sendto(
						// The socket being send to. 
						xSocket,
						// The data being sent. 
						ucBuffer,
						// The length of the data being sent. 
						stringSize+1,
						// ulFlags with the FREERTOS_ZERO_COPY bit clear. 
						0,
						// Where the data is being sent. 
						&xSourceAddress,
						// Not used but should be set as shown. 
						sizeof( xSourceAddress )
					 );													
			}			
				
			if (strncmp(ucBuffer,"RESET_FOR_FLASH", 15)==0) {
				
				stringSize = sprintf(ucBuffer , "RESET_FOR_FLASH_RECEIVED");
				iReturned = FreeRTOS_sendto(
						// The socket being send to. 
						xSocket,
						// The data being sent. 
						ucBuffer,
						// The length of the data being sent. 
						stringSize+1,
						// ulFlags with the FREERTOS_ZERO_COPY bit clear. 
						0,
						// Where the data is being sent. 
						&xSourceAddress,
						// Not used but should be set as shown. 
						sizeof( xSourceAddress )
					 );							
				
				printf("Resetting, downloading to RAM and programming into Flash\r\n");


				//disable the Calculator Timer Interrupt
				BcmGic_DisableIntr(INTC_DIST_BASE_ADDR, TTC_INTR_ID);
				// wait for return from ISR (if CPU1 is stopped while in ISR the Interrupt is never started again)
				sleep(1);
				
				// Stop CPU1
				( *( ( volatile uint32_t * ) ( 0xf8000008 ) ) ) = 0xdf0d;
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x02;
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x22;
				printf("Stopped CPU1\r\n");
				
				// Start CPU1
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x20;
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x00;
				( *( ( volatile uint32_t * ) ( 0xf8000004 ) ) ) = 0x767b;
				printf("Started CPU1 (wfe), download started\r\n");
				
				stringSize = sprintf(ucBuffer , "RESET_SUCCESSFUL_ERASING_FLASH");
				iReturned = FreeRTOS_sendto(
						// The socket being send to. 
						xSocket,
						// The data being sent. 
						ucBuffer,
						// The length of the data being sent. 
						stringSize+1,
						// ulFlags with the FREERTOS_ZERO_COPY bit clear. 
						0,
						// Where the data is being sent. 
						&xSourceAddress,
						// Not used but should be set as shown. 
						sizeof( xSourceAddress )
					 );
				readFromFlash(FLASH_ADDRESS, CORE1_PRG_SIZE, 4);
				int size = Bcm_In32(CORE1_PRG_SIZE);
				
				if(size!=FLASH_INVALID)
				{
					printf("Erasing old program. Size: %u\n\r", size);
					Bcm_Out32(CORE1_PRG_SIZE, FLASH_INVALID);	
					eraseFlash(FLASH_ADDRESS, size+RESERVERD_SPACE);
					writeToFlash(FLASH_ADDRESS, CORE1_PRG_SIZE, 4);					
				}
		
				
				eraseFlash(FLASH_ADDRESS, RESERVERD_SPACE);
				stringSize = sprintf(ucBuffer , "ERASE_SUCCESSFUL_WAITING_FOR_DOWNLOAD");
				iReturned = FreeRTOS_sendto(
						// The socket being send to. 
						xSocket,
						// The data being sent. 
						ucBuffer,
						// The length of the data being sent. 
						stringSize+1,
						// ulFlags with the FREERTOS_ZERO_COPY bit clear. 
						0,
						// Where the data is being sent. 
						&xSourceAddress,
						// Not used but should be set as shown. 
						sizeof( xSourceAddress )
					 );				
				// download the program for CPU1 from CAMeL to the RAM
				fileSize = download();
				

				Bcm_Out32(CORE1_PRG_SIZE, fileSize);		//write the file size into a specific RAM address for later usage when loading from flash
				if(fileSize != FLASH_INVALID)
				{
					printf("fileSize: %u\n\r", fileSize);
					Bcm_DCacheFlush();	
						
					
					// Reset the flags for OCM communication to the original state
					Bcm_Out32(READ_FLAG_ADDRESS, 1);
					Bcm_Out32(WRITE_FLAG_ADDRESS, 1);
					

					int status = 0;
					// program the CPU1 application into the flash
					status = writeToFlash(FLASH_ADDRESS+RESERVERD_SPACE, CORE1_START_ADDR, fileSize);
					// check if the program was successful or the program size was too big for the Flash
					if(status==0)
					{
						writeToFlash(FLASH_ADDRESS, CORE1_PRG_SIZE, 4);
					}
					else
					{
						printf("Program too big for flash! Flashing failed.\r\n");
					}
					
					
					// wait for CPU1 to be ready
					while(Bcm_In32(CPU1STARTADR)!=0){}
					
					// write the start address into a specific register
					Bcm_Out32(CPU1STARTADR, CORE1_START_ADDR);
					
					// flush the cache to make the data available to CPU1
					Bcm_DCacheFlush();
					dmb();
					
					// send event to wake up CPU1
					sev();

					printf("Started CPU1\r\n");					
				}


			}
			else if(strncmp(ucBuffer,"RESET_FOR_RAM", 13)==0)
			{
				stringSize = sprintf(ucBuffer , "RESET_FOR_RAM_RECEIVED");
				iReturned = FreeRTOS_sendto(
						// The socket being send to. 
						xSocket,
						// The data being sent. 
						ucBuffer,
						// The length of the data being sent. 
						stringSize+1,
						// ulFlags with the FREERTOS_ZERO_COPY bit clear. 
						0,
						// Where the data is being sent. 
						&xSourceAddress,
						// Not used but should be set as shown. 
						sizeof( xSourceAddress )
					 );				
				
				printf("Resetting and downloading to RAM\r\n");



					 
				//disable the Calculator Timer Interrupt
				BcmGic_DisableIntr(INTC_DIST_BASE_ADDR, TTC_INTR_ID);
				// wait for return from ISR (if CPU1 is stopped while in ISR the Interrupt is never started again)
				sleep(1);

				// Stop CPU1
				( *( ( volatile uint32_t * ) ( 0xf8000008 ) ) ) = 0xdf0d;
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x02;
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x22;
				printf("Stopped CPU1\r\n");
				
				// Start CPU1
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x20;
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x00;
				( *( ( volatile uint32_t * ) ( 0xf8000004 ) ) ) = 0x767b;
				printf("Started CPU1 (wfe), download started\r\n");
				
				stringSize = sprintf(ucBuffer , "RESET_SUCCESSFUL_WAITING_FOR_DOWNLOAD");
				iReturned = FreeRTOS_sendto(
						// The socket being send to. 
						xSocket,
						// The data being sent. 
						ucBuffer,
						// The length of the data being sent. 
						stringSize+1,
						// ulFlags with the FREERTOS_ZERO_COPY bit clear. 
						0,
						// Where the data is being sent. 
						&xSourceAddress,
						// Not used but should be set as shown. 
						sizeof( xSourceAddress )
					 );
				
				// download the program for CPU1 from CAMeL to the RAM
				fileSize = download();
				
				if(fileSize != FLASH_INVALID)
				{
					printf("fileSize: %u\n\r", fileSize);
					// Reset the flags for OCM communication to the original state
					Bcm_Out32(READ_FLAG_ADDRESS, 1);
					Bcm_Out32(WRITE_FLAG_ADDRESS, 1);
					

					
					// wait for CPU1 to be ready
					while(Bcm_In32(CPU1STARTADR)!=0){}
					
					// write the start address into a specific register
					Bcm_Out32(CPU1STARTADR, CORE1_START_ADDR);
					
					// flush the cache to make the data available to CPU1
					Bcm_DCacheFlush();
					dmb();
					
					// send event to wake up CPU1
					sev();

					printf("Started CPU1\r\n");					
				}			
				

			}
			else if(strncmp(ucBuffer,"RESET_AND_ERASE_FLASH", 21)==0)
			{
				
				stringSize = sprintf(ucBuffer , "RESET_AND_ERASE_FLASH_RECEIVED");
				iReturned = FreeRTOS_sendto(
						// The socket being send to. 
						xSocket,
						// The data being sent. 
						ucBuffer,
						// The length of the data being sent. 
						stringSize+1,
						// ulFlags with the FREERTOS_ZERO_COPY bit clear. 
						0,
						// Where the data is being sent. 
						&xSourceAddress,
						// Not used but should be set as shown. 
						sizeof( xSourceAddress )
					 );							
				
				printf("Resetting and Erasing Flash\r\n");

	
				//disable the Calculator Timer Interrupt
				BcmGic_DisableIntr(INTC_DIST_BASE_ADDR, TTC_INTR_ID);
				// wait for return from ISR (if CPU1 is stopped while in ISR the Interrupt is never started again)
				sleep(1);
				
				// Stop CPU1
				( *( ( volatile uint32_t * ) ( 0xf8000008 ) ) ) = 0xdf0d;
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x02;
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x22;
				printf("Stopped CPU1\r\n");
				
				// Start CPU1
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x20;
				( *( ( volatile uint32_t * ) ( 0xf8000244 ) ) ) = 0x00;
				( *( ( volatile uint32_t * ) ( 0xf8000004 ) ) ) = 0x767b;
				printf("Started CPU1 (wfe), flash erase started\r\n");
				
				
				// Reset the flags for OCM communication to the original state
				Bcm_Out32(READ_FLAG_ADDRESS, 1);
				Bcm_Out32(WRITE_FLAG_ADDRESS, 1);
				
				// read program size from flash
				readFromFlash(FLASH_ADDRESS, CORE1_PRG_SIZE, 4);
				int size = Bcm_In32(CORE1_PRG_SIZE);				
				// erase the flash
				if(size==FLASH_INVALID)
				{
					
					printf("Flash already erased.\r\n");
					stringSize = sprintf(ucBuffer , "ALREADY_ERASED");
					iReturned = FreeRTOS_sendto(
						// The socket being send to. 
						xSocket,
						// The data being sent. 
						ucBuffer,
						// The length of the data being sent. 
						stringSize+1,
						// ulFlags with the FREERTOS_ZERO_COPY bit clear. 
						0,
						// Where the data is being sent. 
						&xSourceAddress,
						// Not used but should be set as shown. 
						sizeof( xSourceAddress )
				   );
				}
				else
				{
					printf("Erasing program flash. Size: %u\n\r", size);
					Bcm_Out32(CORE1_PRG_SIZE, FLASH_INVALID);	
					eraseFlash(FLASH_ADDRESS, size+RESERVERD_SPACE);
					writeToFlash(FLASH_ADDRESS, CORE1_PRG_SIZE, 4);
					printf("Flash has been erased.\r\n");
					stringSize = sprintf(ucBuffer , "ERASE_SUCCESSFUL");
					iReturned = FreeRTOS_sendto(
						// The socket being send to.
						xSocket,
						// The data being sent.
						ucBuffer,
						// The length of the data being sent.
						stringSize+1,
						// ulFlags with the FREERTOS_ZERO_COPY bit clear.
						0,
						// Where the data is being sent.
						&xSourceAddress,
						// Not used but should be set as shown.
						sizeof( xSourceAddress )
					 );
				}
				

			}
		}
	}
#endif
   	vTaskDelete(NULL);
}