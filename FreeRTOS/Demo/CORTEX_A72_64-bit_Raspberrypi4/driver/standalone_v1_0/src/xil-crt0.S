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
* @file xil-crt0.S
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 1.00a ecm  10/20/09 Initial version
* 3.05a sdm  02/02/12 Added code for profiling
* 3.06a sgd  05/16/12 Added global constructors and cleanup code
*                     Uart initialization based on compiler flag
* 3.07a sgd  07/05/12 Updated with reset and start Global Timer
* 3.07a sgd  10/19/12 SMC NOR and SRAM initialization with build option
* 4.2	pkp  08/04/14 Removed PEEP board related code which contained
*		      initialization of uart smc nor and sram
* 5.3	pkp  10/07/15 Added support for OpenAMP by not initializing global
*		      timer when USE_AMP flag is defined
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

	.file	"xil-crt0.S"
	.section ".got2","aw"
	.align	2

	.text
.Lsbss_start:
	.quad	__sbss_start

.Lsbss_end:
	.quad	__sbss_end

.Lbss_start:
	.quad	__bss_start

.Lbss_end:
	.quad	__bss_end

.Lstack:
	.quad	__stack


	.globl	_start
_start:
	bl      __cpu_init		/* Initialize the CPU first (BSP provides this) */

	mov	x0, #0

	/* clear sbss */
	ldr x1, =.Lsbss_start		/* calculate beginning of the SBSS */
	ldr	x2, =.Lsbss_end		/* calculate end of the SBSS */

.Lloop_sbss:
	cmp	x1, x2
	bge	.Lenclsbss		/* If no SBSS, no clearing required */
	str	x0, [x1], #8
	b	.Lloop_sbss

.Lenclsbss:
	/* clear bss */
	ldr	x1, =.Lbss_start		/* calculate beginning of the BSS */
	ldr	x2, =.Lbss_end		/* calculate end of the BSS */

.Lloop_bss:
	cmp	x1,x2
	bge	.Lenclbss		/* If no BSS, no clearing required */
	str	x0, [x1], #8
	b	.Lloop_bss

.Lenclbss:

	/* set stack pointer */
	ldr	x30, =.Lstack		/* stack address */
	mov sp, x30				// set stack pointer

    /* Reset and start Global Timer */
	mov	x0, #0x0
	mov	x1, #0x0

#if USE_AMP != 1
	bl BTime_SetTime
#endif

#ifdef PROFILING			/* defined in Makefile */
	/* Setup profiling stuff */
	bl	_profile_init
#endif /* PROFILING */

   /* run global constructors */
   bl __libc_init_array

	/* make sure argc and argv are valid */
	mov	x0, #0
	mov	x1, #0

	/* Let her rip */
	bl	main

   /* Cleanup global constructors */
   bl __libc_fini_array

#ifdef PROFILING
	/* Cleanup profiling stuff */
	bl	_profile_clean
#endif /* PROFILING */

        /* All done */
	bl	.Lexit

.Lexit:	/* should never get here */
	b .Lexit

.Lstart:
	.size	_start, .Lstart- _start
