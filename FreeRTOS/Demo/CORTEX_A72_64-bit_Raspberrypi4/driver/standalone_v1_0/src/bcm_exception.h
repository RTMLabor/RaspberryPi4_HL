/******************************************************************************
*
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file bcm_exception.h
*
* This header file contains ARM Cortex A53,A9,R5 specific exception related APIs.
* For exception related functions that can be used across all Xilinx supported
* processors, please use bcm_exception.h.
*
* @addtogroup arm_exception_apis ARM Processor Exception Handling
* @{
* ARM processors specific exception related APIs for cortex A53,A9 and R5 can
* utilized for enabling/disabling IRQ, registering/removing handler for
* exceptions or initializing exception vector table with null handler.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 5.2	pkp  	 28/05/15 First release
* 6.0   mus      27/07/16 Consolidated file for a53,a9 and r5 processors
* </pre>
*
******************************************************************************/

#ifndef BCM_EXCEPTION_H /* prevent circular inclusions */
#define BCM_EXCEPTION_H /* by using protection macros */

/***************************** Include Files ********************************/

#include "bcm_types.h"
#include "bpseudo_asm.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************** Constant Definitions ****************************/

//#define BCM_EXCEPTION_FIQ	XREG_CPSR_FIQ_ENABLE
//#define BCM_EXCEPTION_ALL	(XREG_CPSR_FIQ_ENABLE | XREG_CPSR_IRQ_ENABLE)

// see GIC_architecture_specification on page 39
#define BCM_EXCEPTION_ID_FIRST			0U
#if defined (__aarch64__)
#define BCM_EXCEPTION_ID_SYNC_INT		1U
#define BCM_EXCEPTION_ID_IRQ_INT		2U
#define BCM_EXCEPTION_ID_FIQ_INT		3U
#define BCM_EXCEPTION_ID_SERROR_ABORT_INT		4U
#define BCM_EXCEPTION_ID_LAST			5U
#else
#define BCM_EXCEPTION_ID_RESET			0U
#define BCM_EXCEPTION_ID_UNDEFINED_INT		1U
#define BCM_EXCEPTION_ID_SWI_INT		2U
#define BCM_EXCEPTION_ID_PREFETCH_ABORT_INT	3U
#define BCM_EXCEPTION_ID_DATA_ABORT_INT		4U
#define BCM_EXCEPTION_ID_IRQ_INT		5U
#define BCM_EXCEPTION_ID_FIQ_INT		6U
#define BCM_EXCEPTION_ID_LAST			6U
#endif

/*
 * BCM_EXCEPTION_ID_INT is defined for all Xilinx processors.
 */
#define BCM_EXCEPTION_ID_INT	BCM_EXCEPTION_ID_IRQ_INT

/**************************** Type Definitions ******************************/

/**
 * This typedef is the exception handler function.
 */
typedef void (*Bcm_ExceptionHandler)(void *data);
typedef void (*Bcm_InterruptHandler)(void *data);

/***************** Macros (Inline Functions) Definitions ********************/

/****************************************************************************/
/**
* @brief	Enable nested interrupts by clearing the I and F bits in CPSR. This
* 			API is defined for cortex-a9 and cortex-r5.
*
* @return   None.
*
* @note     This macro is supposed to be used from interrupt handlers. In the
*			interrupt handler the interrupts are disabled by default (I and F
*			are 1). To allow nesting of interrupts, this macro should be
*			used. It clears the I and F bits by changing the ARM mode to
*			system mode. Once these bits are cleared and provided the
*			preemption of interrupt conditions are met in the GIC, nesting of
*			interrupts will start happening.
*			Caution: This macro must be used with caution. Before calling this
*			macro, the user must ensure that the source of the current IRQ
*			is appropriately cleared. Otherwise, as soon as we clear the I and
*			F bits, there can be an infinite loop of interrupts with an
*			eventual crash (all the stack space getting consumed).
******************************************************************************/
#define Bcm_EnableNestedInterrupts() \
		__asm__ __volatile__ ("msr DAIFClr, #0x2" ::: "memory");

/****************************************************************************/
/**
* @brief	Disable the nested interrupts by setting the I and F bits. This API
*			is defined for cortex-a9 and cortex-r5.
*
* @return   None.
*
* @note     This macro is meant to be called in the interrupt service routines.
*			This macro cannot be used independently. It can only be used when
*			nesting of interrupts have been enabled by using the macro
*			Bcm_EnableNestedInterrupts(). In a typical flow, the user first
*			calls the Bcm_EnableNestedInterrupts in the ISR at the appropriate
*			point. The user then must call this macro before exiting the interrupt
*			service routine. This macro puts the ARM back in IRQ/FIQ mode and
*			hence sets back the I and F bits.
******************************************************************************/
#define Bcm_DisableNestedInterrupts() \
		__asm__ __volatile__ ("msr DAIFSet, #0x2" ::: "memory");

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/

extern void Bcm_ExceptionRegisterHandler(u32 Exception_id,
					 Bcm_ExceptionHandler Handler,
					 void *Data);

extern void Bcm_ExceptionRemoveHandler(u32 Exception_id);

extern void Bcm_ExceptionInit(void);
#if defined (__aarch64__)
void Bcm_SyncAbortHandler(void *CallBackRef);
void Bcm_SErrorAbortHandler(void *CallBackRef);
#else
extern void Bcm_DataAbortHandler(void *CallBackRef);
extern void Bcm_PrefetchAbortHandler(void *CallBackRef);
extern void Bcm_UndefinedExceptionHandler(void *CallBackRef);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BCM_EXCEPTION_H */
/**
* @} End of "addtogroup arm_exception_apis".
*/