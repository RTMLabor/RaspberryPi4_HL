/******************************************************************************
*

*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file bcm_types.h
*
* @addtogroup common_types Basic Data types for Broadcom &reg; 
*
* The bcm_types.h file contains basic types for Broadcom.
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date   Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00 hbm  07/14/09 First release
* </pre>
*
******************************************************************************/

#ifndef BCM_TYPES_H	/* prevent circular inclusions */
#define BCM_TYPES_H	/* by using protection macros */

#include <stdint.h>
#include <stddef.h>

/************************** Constant Definitions *****************************/

#ifndef TRUE
#  define TRUE		1U
#endif

#ifndef FALSE
#  define FALSE		0U
#endif

#ifndef NULL
#define NULL		0U
#endif

#define BCM_COMPONENT_IS_READY     0x11111111U  /**< In device drivers, This macro will be
                                                 assigend to "IsReady" member of driver
												 instance to indicate that driver
												 instance is initialized and ready to use. */
#define BCM_COMPONENT_IS_STARTED   0x22222222U  /**< In device drivers, This macro will be assigend to
                                                 "IsStarted" member of driver instance
												 to indicate that driver instance is
												 started and it can be enabled. */

/* @name New types
 * New simple types.
 * @{
 */
#ifndef __KERNEL__
#ifndef BBASIC_TYPES_H
/*
 * guarded against bbasic_types.h.
 */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

/** @}*/
#define __BUINT64__
typedef struct
{
	u32 Upper;
	u32 Lower;
} Buint64;


/*****************************************************************************/
/**
* @brief    Return the most significant half of the 64 bit data type.
*
* @param    x is the 64 bit word.
*
* @return   The upper 32 bits of the 64 bit word.
*
******************************************************************************/
#define BUINT64_MSW(x) ((x).Upper)

/*****************************************************************************/
/**
* @brief    Return the least significant half of the 64 bit data type.
*
* @param    x is the 64 bit word.
*
* @return   The lower 32 bits of the 64 bit word.
*
******************************************************************************/
#define BUINT64_LSW(x) ((x).Lower)

#endif /* BBASIC_TYPES_H */

/*
 * bbasic_types.h does not typedef s* or u64
 */
/** @{ */
typedef char char8;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint64_t u64;
typedef int sint32;

typedef intptr_t INTPTR;
typedef uintptr_t UINTPTR;
typedef ptrdiff_t PTRDIFF;
/** @}*/
#if !defined(LONG) || !defined(ULONG)
typedef long LONG;
typedef unsigned long ULONG;
#endif

#define ULONG64_HI_MASK	0xFFFFFFFF00000000U
#define ULONG64_LO_MASK	~ULONG64_HI_MASK

#else
#include <linux/types.h>
#endif

/** @{ */
/**
 * This data type defines an interrupt handler for a device.
 * The argument points to the instance of the component
 */
typedef void (*BInterruptHandler) (void *InstancePtr);

/**
 * This data type defines an exception handler for a processor.
 * The argument points to the instance of the component
 */
typedef void (*BExceptionHandler) (void *InstancePtr);

/**
 * @brief  Returns 32-63 bits of a number.
 * @param  n : Number being accessed.
 * @return Bits 32-63 of number.
 *
 * @note    A basic shift-right of a 64- or 32-bit quantity.
 *          Use this to suppress the "right shift count >= width of type"
 *          warning when that quantity is 32-bits.
 */
#define UPPER_32_BITS(n) ((u32)(((n) >> 16) >> 16))

/**
 * @brief  Returns 0-31 bits of a number
 * @param  n : Number being accessed.
 * @return Bits 0-31 of number
 */
#define LOWER_32_BITS(n) ((u32)(n))




/************************** Constant Definitions *****************************/

#ifndef TRUE
#define TRUE		1U
#endif

#ifndef FALSE
#define FALSE		0U
#endif

#ifndef NULL
#define NULL		0U
#endif

#endif	/* end of protection macro */
/**
* @} End of "addtogroup common_types".
*/
