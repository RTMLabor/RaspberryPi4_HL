/******************************************************************************
*
* Technische Hochschule Lübeck
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
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file bcm_cache.h
*
* @addtogroup a9_cache_apis Cortex A9 Processor Cache Functions
*
* Cache functions provide access to cache related operations such as flush
* and invalidate for instruction and data caches. It gives option to perform
* the cache operations on a single cacheline, a range of memory and an entire
* cache.
*
* @{
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a ecm  01/29/10 First release
* 3.04a sdm  01/02/12 Remove redundant dsb/dmb instructions in cache maintenance
*		      APIs.
* </pre>
*
******************************************************************************/
#ifndef BCM_CACHE_H
#define BCM_CACHE_H

#include "bcm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__

//data-cache line functions
#define asm_clean_dc_line_mva_poc(param) __asm__ __volatile__("DC CVAC, %0" :: "r" (param))

#define asm_inval_dc_line_mva_poc(param) __asm__ __volatile__("DC IVAC, %0" :: "r" (param))

#define asm_clean_inval_dc_line_mva_poc(param) __asm__ __volatile__("DC CIVAC, %0" :: "r" (param))

#define asm_inval_dc_line_sw(param) __asm__ __volatile__("DC ISW, %0" :: "r" (param))

#define asm_clean_inval_dc_line_sw(param) __asm__ __volatile__("DC CISW, %0" :: "r" (param))

//instruction-cache line functions
#define asm_inval_ic_line_mva_pou(param) __asm__ __volatile__("IC IALLU")// :: "r" (param))

#define asm_inval_ic_line_inner_shareable_pou(param) __asm__ __volatile__("IC IALLUIS")// :: "r" (param))

// TLBI
#define asm_inval_all_tlbi() __asm__ __volatile__("tlbi vmalle1");

#elif defined (__ICCARM__)

// general asm-functions
#define dsb() __asm volatile ("dsb sy" : : : "memory")

#define isb() __asm volatile("isb" : : : "memory")

#define asm_disable_intr_except() __asm volatile ("msr daifset, #2")

#define asm_enable_intr_except() __asm volatile("msr daifclr, #2")

//data-cache line functions
#define asm_clean_dc_line_mva_poc(param) __asm volatile ("DC CVAC, %0" :: "r" (param))

#define asm_inval_dc_line_mva_poc(param) __asm volatile ("DC IVAC, %0" :: "r" (param))

#define asm_clean_inval_dc_line_mva_poc(param) __asm volatile ("DC CIVAC, %0" :: "r" (param))

#define asm_inval_dc_line_sw(param) __asm volatile ("DC ISW, %0" :: "r" (param))

#define asm_clean_inval_dc_line_sw(param) __asm volatile ("DC CISW, %0" :: "r" (param))

//instruction-cache line functions
#define asm_inval_ic_line_mva_pou(param) __asm volatile ("IC IALLU")// :: "r" (param))

#define asm_inval_ic_line_inner_shareable_pou(param) __asm volatile ("IC IALLUIS")// :: "r" (param))

// TLBI
#define asm_inval_all_tlbi() __asm volatile ("tlbi vmalle1");

#endif

#define CACHE_LINE_SIZE 64U

void Bcm_DCacheEnable(void);
void Bcm_DCacheDisable(void);
void Bcm_DCacheInvalidate(void);
void Bcm_DCacheInvalidateRange(uintptr_t adr, uintptr_t len);
void Bcm_DCacheFlush(void);
void Bcm_DCacheFlushRange(uintptr_t adr, uintptr_t len);

void Bcm_ICacheEnable(void);
void Bcm_ICacheDisable(void);
void Bcm_ICacheInvalidate(void);
void Bcm_ICacheInvalidateRange(INTPTR adr, u32 len);

#ifdef __cplusplus
}
#endif

#endif
/**
* @} End of "addtogroup a9_cache_apis".
*/