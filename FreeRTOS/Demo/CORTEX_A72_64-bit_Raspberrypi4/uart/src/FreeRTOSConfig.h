/*
 * FreeRTOS Kernel V10.3.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configCPU_CLOCK_HZ						100000000UL
#define configUSE_PORT_OPTIMISED_TASK_SELECTION	1
#define configUSE_TICKLESS_IDLE					0
#define configTICK_RATE_HZ						( ( TickType_t ) 1000 )
#define configPERIPHERAL_CLOCK_HZ  				( 33333000UL )
#define configUSE_PREEMPTION					1
#define configUSE_IDLE_HOOK						1
#define configUSE_TICK_HOOK						1
#define configTICK_RATE_HZ	                    ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES					( 8 )
#define configMINIMAL_STACK_SIZE				( ( unsigned short ) 200 )
#define configTOTAL_HEAP_SIZE					( 124 * 1024 )
#define configMAX_TASK_NAME_LEN					( 16 )
#define configUSE_TRACE_FACILITY				1
#define configUSE_16_BIT_TICKS					0
#define configIDLE_SHOULD_YIELD					1
#define configUSE_MUTEXES						1
#define configQUEUE_REGISTRY_SIZE				8
#define configCHECK_FOR_STACK_OVERFLOW			2
#define configUSE_RECURSIVE_MUTEXES				1
#define configUSE_MALLOC_FAILED_HOOK			1
#define configUSE_APPLICATION_TASK_TAG			0
#define configUSE_COUNTING_SEMAPHORES			1
#define configUSE_QUEUE_SETS					1
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS	3 /* FreeRTOS+FAT requires 2 pointers if a CWD is supported. */

#define configUSE_MUTEXES						1

/* Software timer definitions. */
#define configUSE_TIMERS						1
#define configTIMER_TASK_PRIORITY				( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH				5
#define configTIMER_TASK_STACK_DEPTH			( configMINIMAL_STACK_SIZE * 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskDelay						1

#define INCLUDE_xSemaphoreGetMutexHolder		1

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet				1
#define INCLUDE_uxTaskPriorityGet				1
#define INCLUDE_vTaskDelete						1
#define INCLUDE_vTaskCleanUpResources			1
#define INCLUDE_vTaskSuspend					1
#define INCLUDE_vTaskDelayUntil					1
#define INCLUDE_vTaskDelay						1
#define INCLUDE_xTimerPendFunctionCall			1
#define INCLUDE_eTaskGetState					1
#define INCLUDE_pcTaskGetTaskName				1
#define INCLUDE_xTaskGetSchedulerState			1

void vConfigureTickInterrupt( void );
#define configSETUP_TICK_INTERRUPT() vConfigureTickInterrupt()
void vClearTickInterrupt( void );
#define configCLEAR_TICK_INTERRUPT() vClearTickInterrupt()

#define configINTERRUPT_CONTROLLER_BASE_ADDRESS (0xFF841000U)
#define configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET (0x1000U)
#define configUNIQUE_INTERRUPT_PRIORITIES		(16)
#define configMAX_API_CALL_INTERRUPT_PRIORITY	(14)


/****** TCP demo settings. ****************************************************/

/* Default MAC address configuration.  */

#define configMAC_ADDR0		0xC0
#define configMAC_ADDR1		0xFF
#define configMAC_ADDR2		0xEE
#define configMAC_ADDR3		0xC0
#define configMAC_ADDR4		0xFF
#define configMAC_ADDR5		0xEE

/* Default IP address configuration.  Used in ipconfigUSE_DHCP is set to 0, or
ipconfigUSE_DHCP is set to 1 but a DHCP server cannot be contacted. */
#define configIP_ADDR0		192
#define configIP_ADDR1		168
#define configIP_ADDR2		178
#define configIP_ADDR3		20

/* Default gateway IP address configuration.  Used in ipconfigUSE_DHCP is set to
0, or ipconfigUSE_DHCP is set to 1 but a DHCP server cannot be contacted. */
#define configGATEWAY_ADDR0	192
#define configGATEWAY_ADDR1	168
#define configGATEWAY_ADDR2	178
#define configGATEWAY_ADDR3	1

/* Default DNS server configuration.  OpenDNS addresses are 208.67.222.222 and
208.67.220.220.  Used in ipconfigUSE_DHCP is set to 0, or ipconfigUSE_DHCP is set
to 1 but a DHCP server cannot be contacted.*/
#define configDNS_SERVER_ADDR0 	192
#define configDNS_SERVER_ADDR1 	168
#define configDNS_SERVER_ADDR2 	178
#define configDNS_SERVER_ADDR3 	1

/* Default netmask configuration.  Used in ipconfigUSE_DHCP is set to 0, or
ipconfigUSE_DHCP is set to 1 but a DHCP server cannot be contacted. */
#define configNET_MASK0		255
#define configNET_MASK1		255
#define configNET_MASK2		255
#define configNET_MASK3		0

/****** Demo settings. *************************************************/

#endif /* FREERTOS_CONFIG_H */

