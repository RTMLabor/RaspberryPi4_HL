/******************************************************************************
*
* Copyright (C) 2009 - 2015 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file btime_l.c
*
* This file contains low level functions to get/set time from the Generic Timer
* register in the ARM Cortex A72.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- ---------------------------------------------------
* 1.00a rp/sdm 11/03/09 Initial release.
* 3.07a sgd    07/05/12 Upadted get/set time functions to make use Global Timer
* </pre>
*
* @note		None.
*
******************************************************************************/
/***************************** Include Files *********************************/

#include "btime_l.h"
#include "bpseudo_asm.h"
#include "bcm_types.h"
#include "bcm_assert.h"
#include "bcm_io.h"

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Constant Definitions *****************************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

/****************************************************************************/
/**
* @brief	Set the time in the Generic Timer Counter Register.
*
* @param	Btime_Generic: 64-bit Value to be written to the Generic Timer
*			Counter Register.
*
* @return	None.
*
* @note		When this function is called by any one processor in a multi-
*			processor environment, reference time will reset/lost for all
*			processors.
*
****************************************************************************/
void BTime_SetTime(BTime Btime_Generic)
{
	/* Disable Physical Timer */
	REGS_GT->CNTP_CTL_EL0 &= ~0x1;	// Clear enable bit in CNTK_CTL_EL1

	/* Updating Physical Timer Counter Register */
	REGS_GT->CNTP_CVAL_EL0 = Btime_Generic;

	/* Enable Physical Timer */
	REGS_GT->CNTP_CTL_EL0 |= 0x1;
	Bcm_Out32((u32)GT_TMR_BASEADDR + (u32)GTIMER_CONTROL_OFFSET, (u32)0x1);
}

/****************************************************************************/
/**
* @brief	Get the time from the Global Timer Counter Register.
*
* @param	Btime_Generic: Pointer to the 64-bit location which will be
*			updated with the current timer value.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void BTime_GetTime(BTime *Btime_Generic)
{
	/* Reading Global Timer Counter Register */
	*Btime_Generic = REGS_GT->CNTPCT_EL0;
}
