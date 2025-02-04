/******************************************************************************
*
* Copyright (C) 2012 - 2015 Xilinx, Inc.  All rights reserved.
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
* @file xil_mmu.c
*
* This file provides APIs for enabling/disabling MMU and setting the memory
* attributes for sections, in the MMU translation table.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 1.00a sdm  01/12/12 Initial version
* 3.05a asa  03/10/12 Modified the Xil_EnableMMU to invalidate the caches
*		      before enabling back.
* 3.05a asa  04/15/12 Modified the Xil_SetTlbAttributes routine so that
*		      translation table and branch predictor arrays are
*		      invalidated, D-cache flushed before the attribute
*		      change is applied. This is done so that the user
*		      need not call Xil_DisableMMU before calling
*		      Xil_SetTlbAttributes.
* 3.10a  srt 04/18/13 Implemented ARM Erratas. Please refer to file
*		      'xil_errata.h' for errata description
* 3.11a  asa 09/23/13 Modified Xil_SetTlbAttributes to flush the complete
*			 D cache after the translation table update. Removed the
*			 redundant TLB invalidation in the same API at the beginning.
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "bcm_cache.h"
#include "bpseudo_asm.h"
#include "bcm_types.h"
#include "bcm_mmu.h"
#include "bcm_mem.h"
#include "reg_cortexa72.h"

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Constant Definitions *****************************/


/* Page table configuration array */
struct ptc_t pt_config[NUM_PT_CONFIGS] =
{
    { /* Code region (Read only) */
        .addr = 0x40000000ULL,
        .size = SIZE_128M,
        .executable = XN_OFF,
        .sharable = NON_SHARABLE,
        .permission = READ_WRITE,
        .policy = TYPE_MEM_CACHE_WB,
    },
    { /* Data region */
        .addr = 0x47A13000ULL,
        .size = SIZE_64M,
        .executable = XN_ON,
        .sharable = INNER_SHARABLE,
        .permission = READ_WRITE,
        .policy = TYPE_MEM_CACHE_WB,
    },
    { /* Data region (Shared for OpenAMP) */
        .addr = 0x20600000ULL,
        .size = SIZE_2M,
        .executable = XN_ON,
        .sharable = OUTER_SHARABLE,
        .permission = READ_WRITE,
        .policy = TYPE_MEM_CACHE_WT,
    },
    { /* Page table (Private) */
        .addr = 0x20800000ULL,
        .size = SIZE_2M,
        .executable = XN_ON,
        .sharable = NON_SHARABLE,
        .permission = READ_WRITE,
        .policy = TYPE_MEM_CACHE_WB,
    },
    { /* Peripheral devices (MMIO) */
        .addr = 0xFC000000ULL,
        .size = SIZE_64M,
        .executable = XN_ON,
        .sharable = OUTER_SHARABLE,
        .permission = READ_WRITE,
        .policy = TYPE_DEVICE,
    },
};

/* Page table descriptors and entries */
static __attribute__((aligned(4096))) uint64_t l1ptd[NUM_L1_DESC] __attribute__((section(".pt")));
static __attribute__((aligned(4096))) uint64_t l2pte[NUM_L1_DESC * NUM_L2_ENTRY] __attribute__((section(".pt")));
static __attribute__((aligned(4096))) uint64_t l1ptd_dummy[NUM_L1_DESC] __attribute__((section(".pt")));

/************************** Variable Definitions *****************************/

#define MMUTableSize 4096
extern u32 MMUTable[MMUTableSize];

/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
* @brief	This function sets the memory attributes for a section covering 1MB
*			of memory in the translation table.
*
* @param	Addr: 64-bit address for which memory attributes need to be set.
* @param	attrib: Attribute for the given memory region. bcm_mmu.h contains
*			definitions of commonly used memory attributes which can be
*			utilized for this function.
*
*
* @return	None.
*
* @note		The MMU or D-cache does not need to be disabled before changing a
*			translation table entry.
*
******************************************************************************/
void Bcm_SetTlbAttributes(uintptr_t Addr, u32 attrib)
{
	u32 *ptr;
	u32 section;
	// calculate the section index
	section = Addr / 0x100000UL;
	// point to the section in the MMU table
	ptr = &MMUTable[section];
	// Set attributes for the section
	if(ptr != NULL) {
		*ptr = (Addr & 0xFFFFFFFFF00000UL) | attrib;
	}
	// Flush the data cache to ensure all updates are visible to the MMU
	Bcm_DCacheFlush();

	// Invalidate the entire Unified TLB
	asm_inval_all_tlbi();
	/* Invalidate all branch predictor array */
	asm_inval_ic_line_mva_pou(0);

	dsb(); /* ensure completion of the BP and TLB invalidation */
    isb(); /* synchronize context on this processor */
}

/*****************************************************************************/
/**
* @brief	Enable MMU for cortex A72 processor. This function invalidates the
*			instruction and data caches, and then enables MMU.
*
* @param	None.
* @return	None.
*
******************************************************************************/
void Bcm_EnableMMU(void)
{
	u32 Reg;
	Bcm_DCacheInvalidate();
	Bcm_ICacheInvalidate();

#ifdef __GNUC__
	mfcp(SCTLR_EL1, Reg);
#elif defined (__ICCARM__)
	mfcp(SCTLR_EL1, Reg);
#else	// not adapted to Rpi4
	{ volatile register u32 Cp15Reg __asm(XREG_CP15_SYS_CONTROL);
	  Reg = Cp15Reg; }
#endif
	Reg |= (SCTLR_EL1_M_BIT | SCTLR_EL1_C_BIT | SCTLR_EL1_I_BIT);
	mtcp(SCTLR_EL1, Reg);

	dsb();
	isb();
}

/*****************************************************************************/
/**
* @brief	Disable MMU for Cortex A9 processors. This function invalidates
*			the TLBs, Branch Predictor Array and flushed the D Caches before
*			disabling the MMU.
*
* @param	None.
*
* @return	None.
*
* @note		When the MMU is disabled, all the memory accesses are treated as
*			strongly ordered.
******************************************************************************/
void Bcm_DisableMMU(void)
{
	u32 Reg;

	asm_inval_all_tlbi();
	asm_inval_ic_line_mva_pou(0);
	Bcm_DCacheFlush();

#ifdef __GNUC__
	mfcp(SCTLR_EL1, Reg);
#elif defined (__ICCARM__)
	mfcp(SCTLR_EL1), Reg);
#else	// not adapted to Rpi4
	{ volatile register u32 Cp15Reg __asm(XREG_CP15_SYS_CONTROL);
	  Reg = Cp15Reg; }
#endif
	Reg &= ~(SCTLR_EL1_M_BIT | SCTLR_EL1_C_BIT | SCTLR_EL1_I_BIT);
	mtcp(SCTLR_EL1, Reg);

	dsb();
	isb();

}
// from TImada

//void *memset(void *dest, int c, size_t n)
// defined in bcm_memcpy.c

/* Zeros all the page table descriptors and entries */
void init_pt(void)
{

    /* 1st stage descriptors */
    memset(l1ptd, 0x0ULL, NUM_L1_DESC * sizeof(uint64_t));
    /* 2nd stage entries */
    memset(l2pte, 0x0ULL, NUM_L1_DESC * NUM_L2_ENTRY * sizeof(uint64_t));

    /* 1st upper (dummy) */
    memset(l1ptd_dummy, 0x0ULL, NUM_L1_DESC * sizeof(uint64_t));

    return;
}

/*  Initialize MMU related registers */
void init_regs(void)
{
    volatile uint64_t reg;

    /* Disable MMU */
    Bcm_DisableMMU();

    /* TLB invalidation */
    __asm__ __volatile__ ("tlbi vmalle1");

    /* Cache flush (data and instruction) */
    __asm__ __volatile__ ("ic iallu");
    Bcm_DCacheInvalidate();

    __asm__ __volatile__ ("dsb sy");
    __asm__ __volatile__ ("isb");

    /* MAIR_EL1 configuration */
    reg = ((0xFFULL << (TYPE_MEM_CACHE_WB * 8)) |   /* Write-back cache enabled memory */
           (0xBBULL << (TYPE_MEM_CACHE_WT * 8)) |   /* Write-through cache enabled memory */
           (0x44ULL << (TYPE_MEM_NONCACHE * 8)) |   /* Cache disabled memory */
           (0x00ULL << (TYPE_DEVICE * 8)));         /* Device memory */
    __asm__ __volatile__ ("msr mair_el1, %0" : : "r" (reg));
    __asm__ __volatile__ ("isb");

    /* TTBR{0,1}_EL1 configuration */
    __asm__ __volatile__ ("msr ttbr0_el1, %0" : : "r" ((uint64_t)&l1ptd[0]));
    __asm__ __volatile__ ("msr ttbr1_el1, %0" : : "r" ((uint64_t)&l1ptd_dummy[0]));
    __asm__ __volatile__ ("isb");

    /* TCR_EL1 configuration */
    reg = ((0x1ULL << 38) | /* Top byte not ignored */
           (0x1ULL << 37) | /* Top byte not ignored */
           (0x1ULL << 36) | /* 16-bit ASID */
           (0x1ULL << 32) | /* 36-bit IPS */
           (0x0ULL << 30) | /* 4KB granule size for TG1 */
           (0x2ULL << 28) | /* Outer sharable */
           (0x1ULL << 26) | /* Outer write-back cacheable */
           (0x1ULL << 24) | /* Inner write-back cacheable */
           (0x1ULL << 23) | /* TTBR1_EL1 page table walk disabled */
           (29ULL << 16)  | /* 32GB region size */
           (0x0ULL << 14) | /* 4KB granule size for TG0 */
           (0x2ULL << 12) | /* Outer sharable */
           (0x1ULL << 10) | /* Outer write-back cacheable */
           (0x1ULL << 8)  | /* Inner write-back cacheable */
           (29ULL << 0));   /* 32GB region size */
    __asm__ __volatile__ ("msr tcr_el1, %0" : : "r" (reg));
    __asm__ __volatile__ ("isb");

    return;
}

/* Set page tables */
void set_pt(struct ptc_t *config)
{
    volatile uint64_t pi, num, reg;
    uint64_t i, j;
    uint64_t index;

    /* General page table configuration */
    for (i = 0; i < NUM_L1_DESC; i++) {
        /* 1st stage page table (overwrite) */
        l1ptd[i] = (((((uint64_t)&l2pte[i * NUM_L2_ENTRY] & GLANULE_4KB_MASK) >> 12 ) << 12) | /* 2nd stage entry address */
                   (0x0ULL << 63) | /* NStable (ignored for NS) */
                   (0x0ULL << 61) | /* APTable disabled */
                   (0x0ULL << 60) | /* UXNTable disabled */
                   (0x0ULL << 59) | /* PXNTable disabled */
                   (0x3ULL << 0));  /* Entry type */

        /* 2nd stage page table */
        for (j = 0; j < NUM_L2_ENTRY; j++) {
            reg = ((XN_ON << 53)                |  /* UXN + PXN */
                ((i * NUM_L2_ENTRY + j) << 21)  |  /* Output address */
                (0x1ULL << 10)                  |  /* Access flag */
                (OUTER_SHARABLE << 8)           |  /* Sharablility */
                (READ_WRITE << 6)               |  /* Access permission */
                (TYPE_DEVICE << 2)              |  /* Policy index */
                (0x1ULL << 0));                    /* Entry type */
            l2pte[i * NUM_L2_ENTRY + j] = reg;
        }
    }

    /* User defined page table configuration */
    for (i = 0; i < NUM_PT_CONFIGS; i++) {
        /* 1st stage page table (overwrite) */
        index = (config[i].addr >> 30);
        l1ptd[index] = (((((uint64_t)&l2pte[index * NUM_L2_ENTRY] & GLANULE_4KB_MASK) >> 12 ) << 12) | /* 2nd stage entry address */
                   (0x0ULL << 63) | /* NStable (ignored for NS) */
                   (0x0ULL << 61) | /* APTable disabled */
                   (0x0ULL << 60) | /* UXNTable disabled */
                   (0x0ULL << 59) | /* PXNTable disabled */
                   (0x3ULL << 0));  /* Entry type */

        pi = config[i].addr >> 21;  /* Page table entry index */
        num = config[i].size >> 21;

        /* 2nd stage page table */
        for (j = 0; j < num; j++) {
            reg = ((config[i].executable << 53) |  /* UXN + PXN */
                ((pi + j) << 21)                |  /* Output address */
                (0x1ULL << 10)                  |  /* Access flag */
                (config[i].sharable << 8)       |  /* Sharablility */
                (config[i].permission << 6)     |  /* Access permission */
                (config[i].policy << 2)         |  /* Policy index */
                (0x1ULL << 0));                    /* Entry type */
            l2pte[pi + j] = reg;
        }
    }

    return;
}

/* Update page tables */
void update_pt(void)
{
    volatile uint64_t reg;

    /* Set page pables */
    set_pt(&pt_config[0]);

    /* TTBR{0,1}_EL1 configuration */
    __asm__ __volatile__ ("msr ttbr0_el1, %0" : : "r" (0x0ULL));
    __asm__ __volatile__ ("msr ttbr0_el1, %0" : : "r" ((uint64_t)&l1ptd[0]));
    __asm__ __volatile__ ("isb");

    /* Enable MMU */
    __asm__ __volatile__ ("mrs %0, sctlr_el1" : "=r" (reg));
    reg |= ((0x1ULL << 12) |    /* I-cache enabled */
            (0x1ULL << 4)  |    /* EL0 stack alignment check enabled */
            (0x1ULL << 3)  |    /* Stack alignment check enabled */
            (0x1ULL << 2)  |    /* D-cache enabled */
            (0x0ULL << 1)  |    /* Alignment check disabled */
            (0x1ULL << 0));     /* MMU enabled */
    reg &= ~(0x1ULL << 19);
    __asm__ __volatile__ ("msr sctlr_el1, %0" : : "r" (reg));
    __asm__ __volatile__ ("dsb sy");
    __asm__ __volatile__ ("isb");

    return;
}

/* Main MMU configuration */
void configure_mmu(void)
{
    init_regs();
    init_pt();
    update_pt();

    return;
}