/******************************************************************************
*

*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file bcm_io.h
*
* @addtogroup common_io_interfacing_apis Register IO interfacing APIs
*
* The bcm_io.h file contains the interface for the general I/O component, which
* encapsulates the Input/Output functions for the processors that do not
* require any special I/O handling.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 5.00 	pkp  	 05/29/14 First release
* 6.00  mus      08/19/16 Remove checking of __LITTLE_ENDIAN__ flag for
*                         ARM processors
* </pre>
******************************************************************************/

#ifndef BCM_IO_H           /* prevent circular inclusions */
#define BCM_IO_H           /* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "bcm_types.h"
#include "bcm_printf.h"

#include "bpseudo_asm.h"


/************************** Function Prototypes ******************************/
u16 Bcm_EndianSwap16(u16 Data);
u32 Bcm_EndianSwap32(u32 Data);


/***************** Macros (Inline Functions) Definitions *********************/


#define INLINE __inline


/*****************************************************************************/
/**
*
* @brief    Performs an input operation for a memory location by reading
*           from the specified address and returning the 8 bit Value read from
*            that address.
*
* @param	Addr: contains the address to perform the input operation
*
* @return	The 8 bit Value read from the specified input address.

*
******************************************************************************/
static INLINE u8 Bcm_In8(UINTPTR Addr)
{
	return *(volatile u8 *) Addr;
}

/*****************************************************************************/
/**
*
* @brief    Performs an input operation for a memory location by reading from
*           the specified address and returning the 16 bit Value read from that
*           address.
*
* @param	Addr: contains the address to perform the input operation
*
* @return	The 16 bit Value read from the specified input address.
*
******************************************************************************/
static INLINE u16 Bcm_In16(UINTPTR Addr)
{
	return *(volatile u16 *) Addr;
}

/*****************************************************************************/
/**
*
* @brief    Performs an input operation for a memory location by
*           reading from the specified address and returning the 32 bit Value
*           read  from that address.
*
* @param	Addr: contains the address to perform the input operation
*
* @return	The 32 bit Value read from the specified input address.
*
******************************************************************************/
static INLINE u32 Bcm_In32(UINTPTR Addr)
{
	return *(volatile u32 *) Addr;
}

/*****************************************************************************/
/**
*
* @brief     Performs an input operation for a memory location by reading the
*            64 bit Value read  from that address.
*
*
* @param	Addr: contains the address to perform the input operation
*
* @return	The 64 bit Value read from the specified input address.
*
******************************************************************************/
static INLINE u64 Bcm_In64(UINTPTR Addr)
{
	return *(volatile u64 *) Addr;
}

/*****************************************************************************/
/**
*
* @brief    Performs an output operation for an memory location by
*           writing the 8 bit Value to the the specified address.
*
* @param	Addr: contains the address to perform the output operation
* @param	Value: contains the 8 bit Value to be written at the specified
*           address.
*
* @return	None.
*
******************************************************************************/
static INLINE void Bcm_Out8(UINTPTR Addr, u8 Value)
{
	volatile u8 *LocalAddr = (volatile u8 *)Addr;
	*LocalAddr = Value;
}

/*****************************************************************************/
/**
*
* @brief    Performs an output operation for a memory location by writing the
*            16 bit Value to the the specified address.
*
* @param	Addr contains the address to perform the output operation
* @param	Value contains the Value to be written at the specified address.
*
* @return	None.
*
******************************************************************************/
static INLINE void Bcm_Out16(UINTPTR Addr, u16 Value)
{
	volatile u16 *LocalAddr = (volatile u16 *)Addr;
	*LocalAddr = Value;
}

/*****************************************************************************/
/**
*
* @brief    Performs an output operation for a memory location by writing the
*           32 bit Value to the the specified address.
*
* @param	Addr contains the address to perform the output operation
* @param	Value contains the 32 bit Value to be written at the specified
*           address.
*
* @return	None.
*
******************************************************************************/
static INLINE void Bcm_Out32(UINTPTR Addr, u32 Value)
{
#ifndef ENABLE_SAFETY
	volatile u32 *LocalAddr = (volatile u32 *)Addr;
	*LocalAddr = Value;
#else
	XStl_RegUpdate(Addr, Value);
#endif
}

/*****************************************************************************/
/**
*
* @brief    Performs an output operation for a memory location by writing the
*           64 bit Value to the the specified address.
*
* @param	Addr contains the address to perform the output operation
* @param	Value contains 64 bit Value to be written at the specified address.
*
* @return	None.
*
******************************************************************************/
static INLINE void Bcm_Out64(UINTPTR Addr, u64 Value)
{
	volatile u64 *LocalAddr = (volatile u64 *)Addr;
	*LocalAddr = Value;
}

#if defined (__MICROBLAZE__)
#ifdef __LITTLE_ENDIAN__
# define Bcm_In16LE	Bcm_In16
# define Bcm_In32LE	Bcm_In32
# define Bcm_Out16LE	Bcm_Out16
# define Bcm_Out32LE	Bcm_Out32
# define Bcm_Htons	Bcm_EndianSwap16
# define Bcm_Htonl	Bcm_EndianSwap32
# define Bcm_Ntohs	Bcm_EndianSwap16
# define Bcm_Ntohl	Bcm_EndianSwap32
# else
# define Bcm_In16BE	Bcm_In16
# define Bcm_In32BE	Bcm_In32
# define Bcm_Out16BE	Bcm_Out16
# define Bcm_Out32BE	Bcm_Out32
# define Bcm_Htons(Data) (Data)
# define Bcm_Htonl(Data) (Data)
# define Bcm_Ntohs(Data) (Data)
# define Bcm_Ntohl(Data) (Data)
#endif
#else
# define Bcm_In16LE	Bcm_In16
# define Bcm_In32LE	Bcm_In32
# define Bcm_Out16LE	Bcm_Out16
# define Bcm_Out32LE	Bcm_Out32
# define Bcm_Htons	Bcm_EndianSwap16
# define Bcm_Htonl	Bcm_EndianSwap32
# define Bcm_Ntohs	Bcm_EndianSwap16
# define Bcm_Ntohl	Bcm_EndianSwap32
#endif

#if defined (__MICROBLAZE__)
#ifdef __LITTLE_ENDIAN__
static INLINE u16 Bcm_In16BE(UINTPTR Addr)
#else
static INLINE u16 Bcm_In16LE(UINTPTR Addr)
#endif
#else
static INLINE u16 Bcm_In16BE(UINTPTR Addr)
#endif
{
	u16 value = Bcm_In16(Addr);
	return Bcm_EndianSwap16(value);
}

#if defined (__MICROBLAZE__)
#ifdef __LITTLE_ENDIAN__
static INLINE u32 Bcm_In32BE(UINTPTR Addr)
#else
static INLINE u32 Bcm_In32LE(UINTPTR Addr)
#endif
#else
static INLINE u32 Bcm_In32BE(UINTPTR Addr)
#endif
{
	u32 value = Bcm_In32(Addr);
	return Bcm_EndianSwap32(value);
}

#if defined (__MICROBLAZE__)
#ifdef __LITTLE_ENDIAN__
static INLINE void Bcm_Out16BE(UINTPTR Addr, u16 Value)
#else
static INLINE void Bcm_Out16LE(UINTPTR Addr, u16 Value)
#endif
#else
static INLINE void Bcm_Out16BE(UINTPTR Addr, u16 Value)
#endif
{
	Value = Bcm_EndianSwap16(Value);
	Bcm_Out16(Addr, Value);
}

#if defined (__MICROBLAZE__)
#ifdef __LITTLE_ENDIAN__
static INLINE void Bcm_Out32BE(UINTPTR Addr, u32 Value)
#else
static INLINE void Bcm_Out32LE(UINTPTR Addr, u32 Value)
#endif
#else
static INLINE void Bcm_Out32BE(UINTPTR Addr, u32 Value)
#endif
{
	Value = Bcm_EndianSwap32(Value);
	Bcm_Out32(Addr, Value);
}

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
/**
* @} End of "addtogroup common_io_interfacing_apis".
*/
