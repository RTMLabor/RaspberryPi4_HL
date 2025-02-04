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
* @file asm_vectors.s
*
* This file contains the initial vector table for the Cortex A9 processor
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------- -------- ---------------------------------------------------
* 1.00a ecm/sdm 10/20/09 Initial version
* 3.05a sdm	02/02/12 Save lr when profiling is enabled
* 3.10a srt     04/18/13 Implemented ARM Erratas. Please refer to file
*			 'xil_errata.h' for errata description
* 4.00a pkp	22/01/14 Modified return addresses for interrupt
*			 handlers (DataAbortHandler and SVCHandler)
*			 to fix CR#767251
* 5.1	pkp	05/13/15 Saved the addresses of instruction causing data
*			 abort and prefetch abort into DataAbortAddr and
*			 PrefetchAbortAddr for further use to fix CR#854523
* 5.4	pkp	12/03/15 Added handler for undefined exception
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

#define __ARM_NEON__ 1
/*
.org 0
.text

.global _vector_table

.include "../../../../../../../Common/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CA72_64_BIT/portASM.S"	// for macros portSAVE_CONTEXT and portRESTORECONTEXT
.extern portSAVE_CONTEXT
.extern portRESTORE_CONTEXT


.section .vectors
.balign 2048
_vector_table:

.set VBAR, _vector_table

	B	_boot
	B	Undefined
	B	SVCHandler
	B	PrefetchAbortHandler
	B	DataAbortHandler
	NOP	// Placeholder for address exception vector
	B	IRQHandler
	B	FIQHandler


IRQHandler:					// IRQ vector handler 
	portSAVE_CONTEXT		// state save from compiled code
#ifdef __ARM_NEON__
	// Save floating-point registers d0 to d7
	stp q0, q1, [sp, #-32]!   // Save q0 and q1 (32 bytes from stack pointer)
	stp q2, q3, [sp, #-32]!   // Save q2 and q3 (32 bytes from stack pointer)
	stp q4, q5, [sp, #-32]!   // Save q4 and q5 (32 bytes from stack pointer)
	stp q6, q7, [sp, #-32]!  // Save q6 and q7 (32 bytes from stack pointer)
	// Save registers d16 to d31 (stored as q8 to q15)
	stp q8, q9, [sp, #-32]!   // Save q8 and q9 (32 bytes from stack pointer)
	stp q10, q11, [sp, #-32]! // Save q10 and q11 (32 bytes from stack pointer)
	stp q12, q13, [sp, #-32]! // Save q12 and q13 (32 bytes from stack pointer)
	stp q14, q15, [sp, #-32]! // Save q14 and q15 (32 bytes from stack pointer)

	mrs x1, fpcr				// Move FPSCR to general-purpose register x1
	str x1, [sp, #-8]!  // Store x1 at [sp - 8] and update sp
#endif

#ifdef PROFILING
	ldr	r2, =prof_pc
	subs	r3, lr, #0
	str	r3, [r2]
#endif

	bl	IRQInterrupt			// IRQ vector 

#ifdef __ARM_NEON__
	ldr		x1, [sp], #8
	msr    fpcr, x1

	// Restore floating-point registers d16 to d31 (q8 to q15)
	ldp q14, q15, [sp], #32 // Load q14 and q15 from stack and increment stack pointer
	ldp q12, q13, [sp], #32 // Load q12 and q13 from stack and increment stack pointer
	ldp q10, q11, [sp], #32 // Load q10 and q11 from stack and increment stack pointer
	ldp q8, q9, [sp], #32   // Load q8 and q9 from stack and increment stack pointer

	// Restore floating-point registers d0 to d7 (q0 to q1)
	ldp q6, q7, [sp], #32   // Load q6 and q7 from stack and increment stack pointer
	ldp q4, q5, [sp], #32   // Load q4 and q5 from stack and increment stack pointer
	ldp q2, q3, [sp], #32   // Load q2 and q3 from stack and increment stack pointer
	ldp q0, q1, [sp], #32   // Load q0 and q1 from stack and increment stack pointer

#endif
	portRESTORE_CONTEXT			// state restore from compiled code 


	sub x30, x30, #4  // Subtract 4 from the link register 
	br x30            // Branch to the adjusted link register address 


FIQHandler:					// FIQ vector handler 
	portSAVE_CONTEXT		// state save from compiled code 
#ifdef __ARM_NEON__
	// Save floating-point registers d0 to d7
	stp q0, q1, [sp, #-32]!   // Save q0 and q1 (32 bytes from stack pointer)
	stp q2, q3, [sp, #-32]!   // Save q2 and q3 (32 bytes from stack pointer)
	stp q4, q5, [sp, #-32]!   // Save q4 and q5 (32 bytes from stack pointer)
	stp q6, q7, [sp, #-32]!  // Save q6 and q7 (32 bytes from stack pointer)
	// Save registers d16 to d31 (stored as q8 to q15)
	stp q8, q9, [sp, #-32]!   // Save q8 and q9 (32 bytes from stack pointer)
	stp q10, q11, [sp, #-32]! // Save q10 and q11 (32 bytes from stack pointer)
	stp q12, q13, [sp, #-32]! // Save q12 and q13 (32 bytes from stack pointer)
	stp q14, q15, [sp, #-32]! // Save q14 and q15 (32 bytes from stack pointer)

	mrs x1, fpcr				// Move FPSCR to general-purpose register x1
	str x1, [sp, #-8]!  // Store r1 at [sp - 8] and update sp
#endif

FIQLoop:
	bl	FIQInterrupt			// FIQ vector 

#ifdef __ARM_NEON__
	ldr		x1, [sp], #8
	msr    fpcr, x1

	// Restore floating-point registers d16 to d31 (q8 to q15)
	ldp q14, q15, [sp], #32 // Load q14 and q15 from stack and increment stack pointer
	ldp q12, q13, [sp], #32 // Load q12 and q13 from stack and increment stack pointer
	ldp q10, q11, [sp], #32 // Load q10 and q11 from stack and increment stack pointer
	ldp q8, q9, [sp], #32   // Load q8 and q9 from stack and increment stack pointer

	// Restore floating-point registers d0 to d7 (q0 to q1)
	ldp q6, q7, [sp], #32   // Load q6 and q7 from stack and increment stack pointer
	ldp q4, q5, [sp], #32   // Load q4 and q5 from stack and increment stack pointer
	ldp q2, q3, [sp], #32   // Load q2 and q3 from stack and increment stack pointer
	ldp q0, q1, [sp], #32   // Load q0 and q1 from stack and increment stack pointer
#endif
	portRESTORE_CONTEXT		// state restore from compiled code 
	sub x30, x30, #4  // Subtract 4 from the link register 
	br x30            // Branch to the adjusted link register address 


Undefined:					// Undefined handler 
	portSAVE_CONTEXT		// state save from compiled code 
	ldr     x0, =UndefinedExceptionAddr
	sub     x1, x30, #4					 // Adjust link register (x30) by subtracting 4 
	str     x1, [x0]            		// Store address of instruction causing undefined exception 

	bl	UndefinedException		// UndefinedException: call C function here 
	portRESTORE_CONTEXT		// state restore from compiled code 
	br	x30						// Branch to the return address in link register (x30) 

SVCHandler:					// SWI handler 
	portSAVE_CONTEXT		// state save from compiled code 

	bl	SWInterrupt			// SWInterrupt: call C function here 

	portRESTORE_CONTEXT		// state restore from compiled code 

	br x30            // Branch to the adjusted link register address 


DataAbortHandler:				// Data Abort handler 
#ifdef CONFIG_ARM_ERRATA_775420
	dsb	sy
#endif
	portSAVE_CONTEXT		// state save from compiled code 
	ldr x0, =DataAbortAddr
	mrs x1, elr_el1          // Get the address of the instruction causing the abort 
    sub x1, x1, #8               // Adjust the address to point to the instruction causing the abort 
    str x1, [x0]                 // Store the adjusted address 

	bl	DataAbortInterrupt		//DataAbortInterrupt :call C function here 

	portRESTORE_CONTEXT		// state restore from compiled code 

	// Return to the instruction that caused the Data Abort exception 
    mrs x1, elr_el1              // Get the exception return address 
    sub x1, x1, #8               // Adjust the address to point to the instruction causing the abort 
    msr elr_el1, x1              // Set the adjusted address back to elr_el1 
    br x0                         // Return from exception 

PrefetchAbortHandler:				// Prefetch Abort handler 
#ifdef CONFIG_ARM_ERRATA_775420
	dsb
#endif
	portSAVE_CONTEXT		// state save from compiled code 
	// Store the address of the instruction that caused the prefetch abort 
    ldr x0, =PrefetchAbortAddr
    mrs x1, elr_el1          // Get the address of the instruction causing the abort 
    sub x1, x1, #8               // Adjust the address to point to the instruction causing the abort 
    str x1, [x0]

	bl	PrefetchAbortInterrupt		// PrefetchAbortInterrupt: call C function here 

	portRESTORE_CONTEXT		// state restore from compiled code 

	// Return to the instruction that caused the Prefetch Abort exception 
    mrs x1, elr_el1              // Get the exception return address 
    sub x1, x1, #8               // Adjust the address to point to the instruction causing the abort 
    msr elr_el1, x1              // Set the adjusted address back to elr_el1 
    br x0

.end
*/