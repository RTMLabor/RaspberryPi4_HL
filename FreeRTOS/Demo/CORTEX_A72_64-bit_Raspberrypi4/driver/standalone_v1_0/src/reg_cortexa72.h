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
*
* @file reg_cortexa72.h
*
* This header file contains definitions for using inline assembler code. It is
* written specifically for the GNU, ARMCC compiler.
*
* All of the ARM Cortex A9 GPRs, SPRs, and Debug Registers are defined along
* with the positions of the bits within the registers.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 1.00a ecm/sdm  10/20/09 First release
* </pre>
*
******************************************************************************/
#ifndef BREG_CORTEXA72_H
#define BREG_CORTEXA72_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* GPRs */

/* Coprocessor number defines */

/* Coprocessor control register defines */

/* Current Processor Status Register (CPSR) Bits */

/* C1 Register Defines */
#define SCTLR_EL1_TE_BIT      (1U << 30)   // Thumb Exception enable
#define SCTLR_EL1_AFE_BIT     (1U << 29)   // Access flag enable bit
#define SCTLR_EL1_TRE_BIT     (1U << 28)   // TEX remap enable bit

#define SCTLR_EL1_EE_BIT      (1U << 25)   // Exception Endianness bit

#define SCTLR_EL1_UWXN_BIT    (1U << 20)   // Unprivileged write permission implies PL1 XN
#define SCTLR_EL1_WXN_BIT     (1U << 19)   // Write permission implies XN

#define SCTLR_EL1_V_BIT       (1U << 13)   // Vectors bit
#define SCTLR_EL1_I_BIT       (1U << 12)   // Instruction cache enable bit
#define SCTLR_EL1_Z_BIT       (1U << 11)   // Branch prediction enable bit
#define SCTLR_EL1_SW_BIT      (1U << 10)   // SWP and SWPB enable bit

#define SCTLR_EL1_C_BIT       (1U << 2)    // Cache enable bit
#define SCTLR_EL1_A_BIT       (1U << 1)    // Alignment bit
#define SCTLR_EL1_M_BIT       (1U << 0)    // Address translation enable bit

//D13.2.113

#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN         (0 << 24)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)
#define SCTLR_MMU_ENABLED               (1 << 0)

#define SCTLR_VALUE_MMU_DISABLED (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)
#define SCTLR_VALUE_MMU_ENABLED (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_ENABLED)

#define SCTLR_VALUE_MMU_DISABLED 0x00c018cd    // an FreeRTOSRPi4 angepasst


//D13.2.47

#define HCR_RW                          (1 << 31)
#define HCR_VALUE                       HCR_RW

//D13.2.112

#define SCR_RESERVED                    (3 << 4)
#define SCR_RW                          (1 << 10)
#define SCR_NS                          (1 << 0)
#define SCR_VALUE                       (SCR_RESERVED | SCR_RW | SCR_NS)

//C5.2.19

#define SPSR_MASK_ALL                   (7 << 6) // 15 << 6 with D flag
#define SPSR_EL1h                       (5 << 0)
#define SPSR_EL2h                       (9 << 0)
#define SPSR_VALUE                      (SPSR_MASK_ALL | SPSR_EL1h)


// TCR_TG1_4K: Set the granule size for Translation Control Register 1 (TG1) to 4KB.
#define TCR_TG1_4K     (2 << 30)

// TCR_T1SZ: Set the size of the virtual address space for Translation Control Register 1 (T1) to 48 bits.
#define TCR_T1SZ       ((64 - 48) << 16)

// TCR_TG0_4K: Set the granule size for Translation Control Register 0 (TG0) to 4KB.
#define TCR_TG0_4K     (0 << 14)

// TCR_T0SZ: Set the size of the virtual address space for Translation Control Register 0 (T0) to 48 bits.
#define TCR_T0SZ       (64 - 48)

#define TCR_EL1_VAL    (TCR_TG1_4K | TCR_T1SZ | TCR_TG0_4K | TCR_T0SZ)

#define TCR_EL1_VAL     0x00000071259d251d      // an FreeRTOSRPi4 angepasst


/* architectural feature access control register */
#define CPACR_EL1_FPEN    (1 << 21) | (1 << 20) // don't trap SIMD/FP registers
#define CPACR_EL1_ZEN     (1 << 17) | (1 << 16)  // don't trap SVE instructions
#define CPACR_EL1_VAL     (CPACR_EL1_FPEN | CPACR_EL1_ZEN)

#define CPACR_EL1_VAL     0x300000      // an FreeRTOSRPi4 angepasst



// MMU
/*
 * Memory region attributes:
 *
 *   n = AttrIndx[2:0]
 *			n	MAIR
 *   DEVICE_nGnRnE	000	00000000
 *   NORMAL_NC		001	01000100
 */
#define MT_DEVICE_nGnRnE 		0x0
#define MT_NORMAL_NC			0x1
#define MT_DEVICE_nGnRnE_FLAGS		0x00
#define MT_NORMAL_NC_FLAGS  		0x44
#define MAIR_VALUE			(MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC))
#define MAIR_VALUE      0xffbb4400 // an FreeRTOSRPi4 angepasst

// Performance Monitors Control Register, EL0
#define PMCR_EL0_E_BIT  (1U << 0)    // enable bit. This bit does not disable or enable, counting by event counters reserved for Hyp mode by HDCR.HPMN.
#define PMCR_EL0_P_BIT  (1U << 1)   // Event counter set
#define PMCR_EL0_C_BIT  (1U << 2)   // clock counter reset
#define PMCR_EL0_D_BIT  (1U << 3)   // clock divider
#define PMCR_EL0_X_BIT  (1U << 4)   // expport enable
#define PMCR_EL0_DP_BIT  (1U << 5)   // disable cycle counter
#define PMCR_EL0_LC_BIT  (1U << 6)   // long cycle count enable

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BREG_CORTEXA72_H */
