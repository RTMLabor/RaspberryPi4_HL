/******************************************************************************
*
* Copyright (C) 2009 - 2016 Xilinx, Inc.  All rights reserved.
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
* @file vectors.h
*
* This file contains the C level vector prototypes for the ARM Cortex A9 core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 1.00a ecm  10/20/10 Initial version, moved over from bsp area
* 6.0   mus  07/27/16 Consolidated vectors for a9,a53 and r5 processors
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

#ifndef _VECTORS_H_
#define _VECTORS_H_

/***************************** Include Files *********************************/

#include "bcm_types.h"
#include "bcm_assert.h"

#ifdef __cplusplus
extern "C" {
#endif
/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Constant Definitions *****************************/

/************************** Function Prototypes ******************************/

void FIQInterrupt(void);
void IRQInterrupt(void);
#if !defined (__aarch64__)
void SWInterrupt(void);
void DataAbortInterrupt(void);
void PrefetchAbortInterrupt(void);
void UndefinedException(void);
#else
void SynchronousInterrupt(void);
void SErrorInterrupt(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* protection macro */
