/******************************************************************************
*

*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file bcm_io.c
*
* Contains I/O functions for memory-mapped or non-memory-mapped I/O
* architectures.
*
* @note
*
* This file contains architecture-dependent code.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 5.00 	pkp  	 05/29/14 First release
* </pre>
******************************************************************************/


/***************************** Include Files *********************************/
#include "bcm_io.h"
#include "bcm_types.h"

/*****************************************************************************/
/**
*
* @brief    Perform a 16-bit endian converion.
*
* @param	Data: 16 bit value to be converted
*
* @return	16 bit Data with converted endianess
*
******************************************************************************/
u16 Bcm_EndianSwap16(u16 Data)
{
	return (u16) (((Data & 0xFF00U) >> 8U) | ((Data & 0x00FFU) << 8U));
}

/*****************************************************************************/
/**
*
* @brief    Perform a 32-bit endian converion.
*
* @param	Data: 32 bit value to be converted
*
* @return	32 bit data with converted endianess
*
******************************************************************************/
u32 Bcm_EndianSwap32(u32 Data)
{
	u16 LoWord;
	u16 HiWord;

	/* get each of the half words from the 32 bit word */

	LoWord = (u16) (Data & 0x0000FFFFU);
	HiWord = (u16) ((Data & 0xFFFF0000U) >> 16U);

	/* byte swap each of the 16 bit half words */

	LoWord = (((LoWord & 0xFF00U) >> 8U) | ((LoWord & 0x00FFU) << 8U));
	HiWord = (((HiWord & 0xFF00U) >> 8U) | ((HiWord & 0x00FFU) << 8U));

	/* swap the half words before returning the value */

	return ((((u32)LoWord) << (u32)16U) | (u32)HiWord);
}
