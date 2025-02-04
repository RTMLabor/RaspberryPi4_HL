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
* @file bcm_mmu.h
*
* @addtogroup a9_mmu_apis Cortex A9 Processor MMU Functions
*
* MMU functions equip users to enable MMU, disable MMU and modify default
* memory attributes of MMU table as per the need.
*
* @{
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 1.00a sdm  01/12/12 Initial version
* 4.2	pkp	 07/21/14 Included bcm_types.h file which contains definition for
*					  u32 which resolves issue of CR#805869
* 5.4	pkp	 23/11/15 Added attribute definitions for Xil_SetTlbAttributes API
* </pre>
*
*
******************************************************************************/

#ifndef BCM_MMU_H
#define BCM_MMU_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

/***************************** Include Files *********************************/

#include "bcm_types.h"

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Constant Definitions *****************************/
/**************************** Type Definitions *******************************/

// From Tlmada
/* Rasbperry Pi 4B has a 35-bit (up to 32GB) memory region.
 * We consider 2-stage page table walk with the 4KB granularity.
 * 1st stage: 1GB block size
 * 2nd stage: 2MB block size
 */
#define NUM_L1_DESC     (32ULL)
#define NUM_L2_ENTRY    (512ULL)

/* Page tabe size macros */
#define SIZE_1M         (0x100000ULL)
#define SIZE_2M         (0x200000ULL)
#define SIZE_4M         (0x400000ULL)
#define SIZE_8M         (0x800000ULL)
#define SIZE_16M        (0x1000000ULL)
#define SIZE_32M        (0x2000000ULL)
#define SIZE_64M        (0x4000000ULL)
#define SIZE_128M       (0x8000000ULL)
#define SIZE_256M       (0x10000000ULL)
#define SIZE_512M       (0x20000000ULL)
#define SIZE_1G         (0x40000000ULL)
#define SIZE_2G         (0x80000000ULL)
#define SIZE_4G         (0x100000000ULL)

/* Cache policy macros (MAIR_EL1) */
#define TYPE_DEVICE         (0U) /* Meaning Device-nGnRnE */
#define TYPE_MEM_NONCACHE   (1U) /* Inner-Outer non-cacheable */
#define TYPE_MEM_CACHE_WT   (2U) /* Inner-Outer cacheable, write-through */
#define TYPE_MEM_CACHE_WB   (3U) /* Inner-Outer cacheable, write-back */

/* XN-bit control (UXN + PXN bits for level 2) */
#define XN_ON       (0x0ULL)
#define XN_OFF      (0x2ULL)

/* Memory sharable macros (SH) */
#define NON_SHARABLE        (0U)
#define OUTER_SHARABLE      (2U)
#define INNER_SHARABLE      (3U)

/* Memory permission macros (AP[2:1]) */
#define READ_WRITE  (0x0ULL)
#define READ_ONLY   (0x2ULL)

/* Bit mask for 32GB memory region with 4KB granule size */
#define GLANULE_4KB_MASK    (0x3FFFFF000ULL)

/* Page table configuration data definition */
struct ptc_t
{
    u64 addr;
    u64 size;
    u64 executable;
    u16 sharable;
    u8 permission;
    u8 policy;
};

// From Zynq

/* Memory type */
#define NORM_NONCACHE 0x11DE2 	/* Normal Non-cacheable */
#define STRONG_ORDERED 0xC02	/* Strongly ordered */
#define DEVICE_MEMORY 0xC06		/* Device memory */
#define RESERVED 0x0			/* reserved memory */

/* Normal write-through cacheable shareable */
#define NORM_WT_CACHE 0x16DEA

/* Normal write back cacheable shareable */
#define NORM_WB_CACHE 0x15DE6

/* shareability attribute */
#define SHAREABLE (0x1 << 16)
#define NON_SHAREABLE	(~(0x1 << 16))

/* Execution type */
#define EXECUTE_NEVER ((0x1 << 4) | (0x1 << 0))

/* # of page table configuration entries */
#define NUM_PT_CONFIGS (5)

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

void Bcm_SetTlbAttributes(uintptr_t Addr, u32 attrib);
void Bcm_EnableMMU(void);
void Bcm_DisableMMU(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BCM_MMU_H */
/**
* @} End of "addtogroup a9_mmu_apis".
*/