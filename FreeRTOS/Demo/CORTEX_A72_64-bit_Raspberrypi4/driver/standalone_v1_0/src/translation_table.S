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
* @file translation_table.s
*
* @addtogroup a72_boot_code
* @{
* <h2> translation_table.S </h2>
* translation_table.S contains a static page table required by the MMU for
* the Cortex-A72 used in Raspberry Pi 4. This translation table is flat-mapped 
* (input address = output address) with default memory attributes defined for 
* the Raspberry Pi 4 architecture. It utilizes the short descriptor 
* translation table format, with each section defining 1MB of memory.
*
******************************************************************************/

/*
* The overview of translation table memory attributes is described below.
*
*|                       | Memory Range            | Definition in Translation Table   |
*|-----------------------|-------------------------|-----------------------------------|
*| DDR                   | 0x00000000 - 0x3FFFFFFF | Normal write-back Cacheable       |
*| Peripheral Devices     | 0x3F000000 - 0x3FFFFFFF | Device Memory                     |
*| Reserved              | 0x40000000 - 0x7FFFFFFF | Unassigned                        |
*| GPU Memory            | 0xC0000000 - 0xCFFFFFFF | Device Memory                     |
*| Reserved              | 0xD0000000 - 0xFFFFFFFF | Unassigned                        |
*
******************************************************************************/

	.globl  MMUTable

	.section .mmu_tbl,"a"

MMUTable:
	/* Each table entry occupies one 32-bit word and there are
	 * 4096 entries, so the entire table takes up 16KB.
	 * Each entry covers a 1MB section.
	 */
.set SECT, 0

// from "bparameters.h"
#ifdef RPI4_DDR_BASE
.set DDR_START, RPI4_DDR_BASE
.set DDR_END, RPI4_DDR_HIGH
.set DDR_SIZE, (DDR_END - DDR_START) + 1
.set DDR_REG, DDR_SIZE / 0x100000
#else
.set DDR_REG, 0
#endif

.set UNDEF_REG, 0x3FF - DDR_REG

				/*0x00000000 - 0x00100000 (DDR Cacheable)*/
.word	SECT + 0x15DE6  // S=b1 TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1
.set	SECT, SECT + 0x100000

.rept	DDR_REG  // (DDR Cacheable)
.word	SECT + 0x15DE6
.set	SECT, SECT + 0x100000
.endr

.rept	0x1F  // (Unassigned/Reserved). Generates a translation fault if accessed
.word	SECT + 0x0  // S=b0 TEX=b000 AP=b00, Domain=b0, C=b0, B=b0
.set	SECT, SECT + 0x100000
.endr

.rept	0x1  // 0x3F000000 - 0x3FFFFFFF (Peripheral Devices)
.word	SECT + 0xC06  // S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1
.set	SECT, SECT + 0x100000
.endr

.rept	0x3F  // Reserved
.word	SECT + 0x0  // S=b0 TEX=b000 AP=b00, Domain=b0, C=b0, B=b0
.set	SECT, SECT + 0x100000
.endr

.rept	0x1F  // 0xC0000000 - 0xCFFFFFFF (GPU Memory)
.word	SECT + 0xC06  // S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1
.set	SECT, SECT + 0x100000
.endr

.rept	0x1F  // Reserved
.word	SECT + 0x0  // S=b0 TEX=b000 AP=b00, Domain=b0, C=b0, B=b0
.set	SECT, SECT + 0x100000
.endr

.end