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
* @file cpu_init.s
*
* This file contains CPU specific initialization. Invoked from main CRT
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------- -------- ---------------------------------------------------
* 1.00a ecm/sdm 10/20/09 Initial version
* 3.04a sdm	01/02/12 Updated to clear cp15 regs with unknown reset values
* 5.0   pkp	12/16/14 removed incorrect initialization of TLB lockdown
*			 register to fix CR#830580
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

	.text
	.global __cpu_init
	.align 2
__cpu_init:

/* Clear system registers with unknown reset values */
    mov     x0, #0x0
    msr     elr_el1, x0          /* Exception Link Register */
    msr     esr_el1, x0          /* Exception Syndrome Register */
    msr     far_el1, x0          /* Fault Address Register */
    msr     pmccntr_el0, x0      /* Performance Monitors Cycle Counter */
    msr     tpidr_el0, x0        /* Thread Pointer (user) */
    msr     tpidrro_el0, x0      /* Read-Only Thread Pointer (user) */

/* Reset and start Cycle Counter */
    mov     x2, #0x80000000      /* clear overflow */
    msr     pmcr_el0, x2
    mov     x2, #0xd             /* D, C, E */
    msr     pmcntenset_el0, x2
    mov     x2, #0x80000000      /* enable cycle counter */
    msr     pmccntr_el0, x2

	ret

.end
