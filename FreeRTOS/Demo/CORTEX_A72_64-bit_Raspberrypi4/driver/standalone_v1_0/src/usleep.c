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
*
* @file usleep.c
*
* This function provides a microsecond delay using the Global Timer register in
* the ARM Cortex A9 MP core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 1.00a ecm/sdm  11/11/09 First release
* 3.07a sgd      07/05/12 Upadted micro sleep function to make use Global Timer
* 4.2	pkp		 08/04/14 Removed unimplemented nanosleep routine as it is not
*						  possible to generate timer in nanosecond due to
*						  limited cpu frequency
* 6.0   asa      08/15/16 Updated the usleep signature. Fix for CR#956899.
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/

#include "sleep.h"
#include "btime_l.h"
#include "bparameters.h"
#include "bcm_types.h"
#include "bpseudo_asm.h"
#include "reg_cortexa72.h"
#include "printf.h"

/* Global Timer is always clocked at half of the CPU frequency */
#define COUNTS_PER_USECOND  (CPU_CORTEXA72_CORE_CLOCK_FREQ_HZ / (2U*1000000U))

/*****************************************************************************/
/**
*
* This API gives a delay in microseconds
*
* @param	useconds requested
*
* @return	0 if the delay can be achieved, -1 if the requested delay
*		is out of range
*
* @note		None.
*
****************************************************************************/
int usleep(unsigned long useconds)
{
	BTime tEnd, tCur;

	BTime_GetTime(&tCur);
	tEnd = tCur + (((BTime) useconds) * COUNTS_PER_USECOND);
    printf("tCur: %llu, tEnd: %llu\n", tCur, tEnd); // Debugging
	do
	{
		BTime_GetTime(&tCur);
    	printf("tCur: %llu, tEnd: %llu\n", tCur, tEnd); // Debugging
	} while (tCur < tEnd);

	return 0;
}
