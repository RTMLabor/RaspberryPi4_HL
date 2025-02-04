/******************************************************************************
*

*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file bgpio.h
* @addtogroup bgpio_v1_0
* @{
* @details
*
* The Broadcom GPIO driver. This driver supports the Broadcom GPIO Header.
*
* The GPIO Controller supports the following features:
*	- 4 banks
*	- Masked writes (There are no masked reads)
*	- Bypass mode
*	- Configurable Interrupts (Level/Edge)
*
* This driver is intended to be RTOS and processor independent. Any needs for
* dynamic memory management, threads or thread mutual exclusion, virtual
* memory, or cache control must be satisfied by the layer above this driver.

* This driver supports all the features listed above, if applicable.
*
* <b>Driver Description</b>
*
* The device driver enables higher layer software (e.g., an application) to
* communicate to the GPIO.
*
* <b>Interrupts</b>
*
* The driver provides interrupt management functions and an interrupt handler.
* Users of this driver need to provide callback functions. An interrupt handler
* example is available with the driver.
*
* <b>Threads</b>
*
* This driver is not thread safe. Any needs for threads or thread mutual
* exclusion must be satisfied by the layer above this driver.
*
* <b>Asserts</b>
*
* Asserts are used within all Xilinx drivers to enforce constraints on argument
* values. Asserts can be turned off on a system-wide basis by defining, at
* compile time, the NDEBUG identifier. By default, asserts are turned on and it
* is recommended that users leave asserts on during development.
*
* <b>Building the driver</b>
*
* The BGpio driver is composed of several source files. This allows the user
* to build and link only those parts of the driver that are necessary.
* <br><br>
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a sv   01/15/10 First Release
* 1.01a sv   04/15/12 Removed the APIs XGpioPs_SetMode, XGpioPs_SetModePin
*                     XGpioPs_GetMode, XGpioPs_GetModePin as they are not
*		      relevant to Zynq device.The interrupts are disabled
*		      for output pins on all banks during initialization.
* 1.02a hk   08/22/13 Added low level reset API
* 2.1   hk   04/29/14 Use Input data register DATA_RO for read. CR# 771667.
* 2.2	sk	 10/13/14 Used Pin number in Bank instead of pin number
* 					  passed to APIs. CR# 822636
* 3.00  kvn  02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.1	kvn  04/13/15 Add support for Zynq Ultrascale+ MP. CR# 856980.
*       ms   03/17/17 Added readme.txt file in examples folder for doxygen
*                     generation.
*       ms   04/05/17 Added tabspace for return statements in functions of
*                     gpiops examples for proper documentation while
*                     generating doxygen.
* 3.3   ms   04/17/17 Added notes about gpio input and output pin description
*                     for zcu102 and zc702 boards in polled and interrupt
*                     example, configured Interrupt pin to input pin for
*                     proper functioning of interrupt example.
* </pre>
*
******************************************************************************/
#ifndef BCMGPIO_H		/* prevent circular inclusions */
#define BCMGPIO_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "bstatus.h"
#include "bplatform_info.h"


/**************************** Type Definitions *******************************/

/****************************************************************************/
/**
 * This handler data type allows the user to define a callback function to
 * handle the interrupts for the GPIO device. The application using this
 * driver is expected to define a handler of this type, to support interrupt
 * driven mode. The handler executes in an interrupt context such that minimal
 * processing should be performed.
 *
 * @param	CallBackRef is a callback reference passed in by the upper layer
 *		when setting the callback functions for a GPIO bank. It is
 *		passed back to the upper layer when the callback is invoked. Its
 *		type is not important to the driver component, so it is a void
 *		pointer.
 * @param	Bank is the bank for which the interrupt status has changed.
 * @param	Status is the Interrupt status of the GPIO bank.
 *
 *****************************************************************************/
typedef void (*BGpio_Handler) (void *CallBackRef, u32 Bank, u32 Status);

/**
 * This typedef contains configuration information for a device.
 */
typedef struct {
	u16 DeviceId;		/**< Unique ID of device */
	u32 BaseAddr;		/**< Register base address */
} BGpio_Config;

/**
 * The BGpio driver instance data. The user is required to allocate a
 * variable of this type for the GPIO device in the system. A pointer
 * to a variable of this type is then passed to the driver API functions.
 */
typedef struct {
	BGpio_Config GpioConfig;	/**< Device configuration */
	u32 IsReady;			/**< Device is initialized and ready */
	BGpio_Handler Handler;	/**< Status handlers for all banks */
	void *CallBackRef; 		/**< Callback ref for bank handlers */
	u32 Platform;			/**< Platform data */
	u32 MaxPinNum;			/**< Max pins in the GPIO device */
	u8 MaxBanks;			/**< Max banks in a GPIO device */
} BGpio;

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/* Functions in xgpiops.c */
s32 BGpio_CfgInitialize(BGpio *InstancePtr, BGpio_Config *ConfigPtr,
			   u32 EffectiveAddr);

/* Bank APIs in xgpiops.c */
u32 XGpioPs_Read(BGpio *InstancePtr, u8 Bank);
void XGpioPs_Write(BGpio *InstancePtr, u8 Bank, u32 Data);
void XGpioPs_SetDirection(BGpio *InstancePtr, u8 Bank, u32 Direction);
u32 XGpioPs_GetDirection(BGpio *InstancePtr, u8 Bank);
void XGpioPs_SetOutputEnable(BGpio *InstancePtr, u8 Bank, u32 OpEnable);
u32 XGpioPs_GetOutputEnable(BGpio *InstancePtr, u8 Bank);
void XGpioPs_GetBankPin(u8 PinNumber,	u8 *BankNumber, u8 *PinNumberInBank);

/* Pin APIs in xgpiops.c */
u32 XGpioPs_ReadPin(BGpio *InstancePtr, u32 Pin);
void XGpioPs_WritePin(BGpio *InstancePtr, u32 Pin, u32 Data);
void XGpioPs_SetDirectionPin(BGpio *InstancePtr, u32 Pin, u32 Direction);
u32 XGpioPs_GetDirectionPin(BGpio *InstancePtr, u32 Pin);
void XGpioPs_SetOutputEnablePin(BGpio *InstancePtr, u32 Pin, u32 OpEnable);
u32 XGpioPs_GetOutputEnablePin(BGpio *InstancePtr, u32 Pin);

/* Diagnostic functions in xgpiops_selftest.c */
s32 XGpioPs_SelfTest(BGpio *InstancePtr);

/* Functions in xgpiops_intr.c */
/* Bank APIs in xgpiops_intr.c */
void XGpioPs_IntrEnable(BGpio *InstancePtr, u8 Bank, u32 Mask);
void XGpioPs_IntrDisable(BGpio *InstancePtr, u8 Bank, u32 Mask);
u32 XGpioPs_IntrGetEnabled(BGpio *InstancePtr, u8 Bank);
u32 XGpioPs_IntrGetStatus(BGpio *InstancePtr, u8 Bank);
void XGpioPs_IntrClear(BGpio *InstancePtr, u8 Bank, u32 Mask);
void XGpioPs_SetIntrType(BGpio *InstancePtr, u8 Bank, u32 IntrType,
			  u32 IntrPolarity, u32 IntrOnAny);
void XGpioPs_GetIntrType(BGpio *InstancePtr, u8 Bank, u32 *IntrType,
			  u32 *IntrPolarity, u32 *IntrOnAny);
void XGpioPs_SetCallbackHandler(BGpio *InstancePtr, void *CallBackRef,
			     XGpioPs_Handler FuncPointer);
void XGpioPs_IntrHandler(BGpio *InstancePtr);

/* Pin APIs in xgpiops_intr.c */
void XGpioPs_SetIntrTypePin(BGpio *InstancePtr, u32 Pin, u8 IrqType);
u8 XGpioPs_GetIntrTypePin(BGpio *InstancePtr, u32 Pin);

void XGpioPs_IntrEnablePin(BGpio *InstancePtr, u32 Pin);
void XGpioPs_IntrDisablePin(BGpio *InstancePtr, u32 Pin);
u32 XGpioPs_IntrGetEnabledPin(BGpio *InstancePtr, u32 Pin);
u32 XGpioPs_IntrGetStatusPin(BGpio *InstancePtr, u32 Pin);
void XGpioPs_IntrClearPin(BGpio *InstancePtr, u32 Pin);

/* Functions in xgpiops_sinit.c */
BGpio_Config *XGpioPs_LookupConfig(u16 DeviceId);

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
/** @} */
