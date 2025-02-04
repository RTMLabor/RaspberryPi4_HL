/******************************************************************************
*
* Copyright (C) 2011 - 2015 Xilinx, Inc.  All rights reserved.
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
* @file xpm_counter.h
*
* @addtogroup a9_event_counter_apis Cortex A9 Event Counters Functions
*
* Cortex A9 event counter functions can be utilized to configure and control
* the Cortex-A9 performance monitor events.
*
* Cortex-A9 performance monitor has six event counters which can be used to
* count a variety of events described in Coretx-A9 TRM. xpm_counter.h defines
* configurations XPM_CNTRCFGx which can be used to program the event counters
* to count a set of events.
*
* @note
* It doesn't handle the Cortex-A9 cycle counter, as the cycle counter is
* being used for time keeping.
*
* @{
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a sdm  07/11/11 First release
* </pre>
*
******************************************************************************/

#ifndef XPMCOUNTER_H /* prevent circular inclusions */
#define XPMCOUNTER_H /* by using protection macros */

/***************************** Include Files ********************************/

#include <stdint.h>
#include "bpseudo_asm.h"
#include "bcm_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************** Constant Definitions ****************************/

/* Number of performance counters */
#define BPM_CTRCOUNT 6U


// Cortex-A72 Performance Monitor Events

// Instruction cache accesses
#define PMU_EVENT_IC_ACCESS              0x01
// Counts the number of instruction fetches.

// Instruction cache misses
#define PMU_EVENT_IC_MISS                0x03
// Counts the number of instruction cache misses.

// Data cache accesses
#define PMU_EVENT_DC_ACCESS              0x04
// Counts the number of data accesses.

// Data cache misses
#define PMU_EVENT_DC_MISS                0x06
// Counts the number of data cache misses.

// Data read accesses
#define PMU_EVENT_DATA_READ_ACCESS       0x07
// Counts the number of data read accesses.

// Data write accesses
#define PMU_EVENT_DATA_WRITE_ACCESS      0x08
// Counts the number of data write accesses.

// Instruction read accesses
#define PMU_EVENT_INST_READ_ACCESS       0x09
// Counts the number of instruction read accesses.

// Instruction write accesses
#define PMU_EVENT_INST_WRITE_ACCESS      0x0A
// Counts the number of instruction write accesses.

// External memory request accesses
#define PMU_EVENT_EXTERNAL_MEM_ACCESS    0x0B
// Counts the number of external memory request accesses.

// Branch instructions executed
#define PMU_EVENT_BRANCH_EXECUTED        0x0C
// Counts the number of branch instructions executed.

// Branch mispredictions
#define PMU_EVENT_BRANCH_MISPREDICT      0x0D
// Counts the number of incorrectly predicted branches.

// Stalled cycles
#define PMU_EVENT_STALLED_CYCLES         0x0E
// Counts the number of cycles during which the pipeline is stalled.

// Data cache write-back
#define PMU_EVENT_DC_WRITEBACK           0x10
// Counts the number of data cache write-backs.

// Instruction executed
#define PMU_EVENT_INSTRUCTIONS_EXECUTED  0x11
// Counts the number of instructions executed.

// L1 data cache accesses
#define PMU_EVENT_L1_DC_ACCESS           0x14
// Counts the number of L1 data cache accesses.

// L1 data cache misses
#define PMU_EVENT_L1_DC_MISS             0x16
// Counts the number of L1 data cache misses.

// L1 instruction cache accesses
#define PMU_EVENT_L1_IC_ACCESS           0x18
// Counts the number of L1 instruction cache accesses.

// L1 instruction cache misses
#define PMU_EVENT_L1_IC_MISS             0x1A
// Counts the number of L1 instruction cache misses.

// L2 data cache accesses
#define PMU_EVENT_L2_DC_ACCESS           0x1C
// Counts the number of L2 data cache accesses.

// L2 data cache misses
#define PMU_EVENT_L2_DC_MISS             0x1E
// Counts the number of L2 data cache misses.

// L2 instruction cache accesses
#define PMU_EVENT_L2_IC_ACCESS           0x20
// Counts the number of L2 instruction cache accesses.

// L2 instruction cache misses
#define PMU_EVENT_L2_IC_MISS             0x22
// Counts the number of L2 instruction cache misses.

// Bus accesses
#define PMU_EVENT_BUS_ACCESS             0x23
// Counts the number of bus accesses.

// Bus stalls
#define PMU_EVENT_BUS_STALL              0x24
// Counts the number of bus stalls.

// Bus transaction read
#define PMU_EVENT_BUS_READ               0x25
// Counts the number of bus transaction reads.

// Bus transaction write
#define PMU_EVENT_BUS_WRITE              0x26
// Counts the number of bus transaction writes.

// Bus transaction exclusive read
#define PMU_EVENT_BUS_EXCL_READ          0x27
// Counts the number of bus transaction exclusive reads.

// Bus transaction exclusive write
#define PMU_EVENT_BUS_EXCL_WRITE         0x28
// Counts the number of bus transaction exclusive writes.

// Data access read
#define PMU_EVENT_DATA_ACCESS_READ       0x29
// Counts the number of data access reads.

// Data access write
#define PMU_EVENT_DATA_ACCESS_WRITE      0x2A
// Counts the number of data access writes.

// Instruction access read
#define PMU_EVENT_INST_ACCESS_READ       0x2B
// Counts the number of instruction access reads.

// Instruction access write
#define PMU_EVENT_INST_ACCESS_WRITE      0x2C
// Counts the number of instruction access writes.

// Data prefetches
#define PMU_EVENT_DATA_PREFETCH          0x30
// Counts the number of data prefetches.

// Instruction prefetches
#define PMU_EVENT_INST_PREFETCH          0x31
// Counts the number of instruction prefetches.


// Cortex-A72 Performance Monitor Event Configurations

// Configuration 0: Default configuration
#define PMU_CONFIG0 0x00
// Configures event counters for:
// - Instruction cache accesses
// - Data cache accesses
// - Branch instructions executed
// - L1 data cache accesses
// - L1 instruction cache accesses

// Configuration 1: Configuration focusing on cache misses
#define PMU_CONFIG1 0x01
// Configures event counters for:
// - Instruction cache misses
// - Data cache misses
// - L1 data cache misses
// - L1 instruction cache misses
// - L2 data cache misses
// - L2 instruction cache misses

// Configuration 2: Configuration focusing on stalls and waits
#define PMU_CONFIG2 0x02
// Configures event counters for:
// - Stalled cycles
// - Bus stalls
// - Bus access

// Configuration 3: Configuration focusing on branch prediction
#define PMU_CONFIG3 0x03
// Configures event counters for:
// - Branch mispredictions
// - Bus transaction read
// - Bus transaction write
// - Bus transaction exclusive read
// - Bus transaction exclusive write

// Configuration 4: Configuration focusing on data and instruction prefetches
#define PMU_CONFIG4 0x04
// Configures event counters for:
// - Data prefetches
// - Instruction prefetches
// - Data access read
// - Data access write
// - Instruction access read
// - Instruction access write

// Configuration 5: Custom configuration
#define PMU_CONFIG5 0x05
// Configures event counters for a custom set of events. Modify as needed.

// Event constants for configuring performance events

#define ARMV8_EVENT_SW_INCR                 0x00    // Instruction architecturally executed (condition check pass) - Software increment
#define ARMV8_EVENT_L1I_CACHE_REFILL        0x01    // [0] [1] Level 1 instruction cache refill
#define ARMV8_EVENT_L1I_TLB_REFILL          0x02    // [1] [2] Level 1 instruction TLB refill
#define ARMV8_EVENT_L1D_CACHE_REFILL        0x03    //  [2] [3] Level 1 data cache refill
#define ARMV8_EVENT_L1D_CACHE               0x04    // - [5:4] Level 1 data cache access
#define ARMV8_EVENT_L1D_TLB_REFILL          0x05    //  - [7:6] Level 1 data TLB refill
#define ARMV8_EVENT_INST_RETIRED            0x08    //  [6:3] [11:8] Instruction architecturally executed
#define ARMV8_EVENT_EXC_TAKEN               0x09    //  [7] [12] Exception taken
#define ARMV8_EVENT_EXC_RETURN              0x0A    //  [8] [13] Instruction architecturally executed (condition check pass) - Exception return
#define ARMV8_EVENT_CID_WRITE_RETIRED       0x0B    //  - [14] Instruction architecturally executedv (condition check pass) - Write to CONTEXTIDR
#define ARMV8_EVENT_BR_MIS_PRED             0x10    //  [9] [15] Mispredicted or not predicted branch speculatively executed
#define ARMV8_EVENT_CPU_CYCLES              0x11    //  - [16] Cycle
#define ARMV8_EVENT_BR_PRED                 0x12    //  [10] [17] Predictable branch speculatively executed
#define ARMV8_EVENT_MEM_ACCESS              0x13    //  - [19:18] Data memory access
#define ARMV8_EVENT_L1I_CACHE               0x14    //  [11] [20] Level 1 instruction cache access
#define ARMV8_EVENT_L1D_CACHE_WB            0x15    //  [12] [21] Level 1 data cache Write-Back
#define ARMV8_EVENT_L2D_CACHE               0x16    //  - [23:22] Level 2 data cache access
#define ARMV8_EVENT_L2D_CACHE_REFILL        0x17    //  [13] [24] Level 2 data cache refill
#define ARMV8_EVENT_L2D_CACHE_WB            0x18    //  [14] [25] Level 2 data cache Write-Back
#define ARMV8_EVENT_BUS_ACCESS              0x19    //  - [27:26] Bus access
#define ARMV8_EVENT_MEMORY_ERROR            0x1A    //  - [28] Local memory error
#define ARMV8_EVENT_INST_SPEC               0x1B    //  - [30:29] Operation speculatively executed
#define ARMV8_EVENT_TTBR_WRITE_RETIRED      0x1C    //  - [31] Instruction architecturally executed (condition check pass) - Write to translation table base
#define ARMV8_EVENT_BUS_CYCLES              0x1D    //  - [32] Bus cycle
#define ARMV8_EVENT_CHAIN                   0x1E    //  - [33] Odd performance counter chain mode
#define ARMV8_EVENT_L1D_CACHE_RD            0x40    //  [15] [34] Level 1 data cache access - Read
#define ARMV8_EVENT_L1D_CACHE_WR            0x41    //  [16] [35] Level 1 data cache access - Write
#define ARMV8_EVENT_L1D_CACHE_REFILL_RD     0x42    //  [36] Level 1 data cache refill - Read
#define ARMV8_EVENT_L1D_CACHE_REFILL_WR     0x43    //  - [37] Level 1 data cache refill - Write
#define ARMV8_EVENT_L1D_CACHE_REFILL_INNER  0x44    //  - Attributable Level 1 data cache refill - inner
#define ARMV8_EVENT_L1D_CACHE_REFILL_OUTER  0x45    //  -  Attributable Level 1 data cache refill - outer
#define ARMV8_EVENT_L1D_CACHE_WB_VICTIM     0x46    //  - [38] Level 1 data cache Write-back - Victim
#define ARMV8_EVENT_L1D_CACHE_WB_CLEAN      0x47    //  - [39] Level 1 data cache Write-back - Cleaning and coherency
#define ARMV8_EVENT_L1D_CACHE_INVAL         0x48    //  - [40] Level 1 data cache invalidate
#define ARMV8_EVENT_L1D_TLB_REFILL_LD       0x4C    //  [17] [41] Level 1 data TLB refill - Read
#define ARMV8_EVENT_L1D_TLB_REFILL_ST       0x4D    //  [18] [42] Level 1 data TLB refill - Write
#define ARMV8_EVENT_L1D_TLB_RD              0x4E    //  Attributable Level 1 data or unified TLB access, read
#define ARMV8_EVENT_L1D_TLB_WR              0x4F    //  Attributable Level 1 data or unified TLB access, write
#define ARMV8_EVENT_L2D_CACHE_LD            0x50    //  [19] [43] Level 2 data cache access - Read
#define ARMV8_EVENT_L2D_CACHE_ST            0x51    //  [20] [44] Level 2 data cache access - Write
#define ARMV8_EVENT_L2D_CACHE_REFILL_LD     0x52    //  - [45] Level 2 data cache refill - Read
#define ARMV8_EVENT_L2D_CACHE_REFILL_ST     0x53    //  - [46] Level 2 data cache refill - Write
#define ARMV8_EVENT_L2D_CACHE_WB_VICTIM     0x56    //  - [47] Level 2 data cache Write-back - Victim
#define ARMV8_EVENT_L2D_CACHE_WB_CLEAN      0x57    //  - [48] Level 2 data cache Write-back - Cleaning and coherency
#define ARMV8_EVENT_L2D_CACHE_INVAL         0x58    //  - [49] Level 2 data cache invalidate
#define ARMV8_EVENT_L2D_TLB_REFILL_RD       0x5C    //  - Attributable Level 2 data or unified TLB refill, read
#define ARMV8_EVENT_L2D_TLB_REFILL_WR       0x5D    //  - Attributable Level 2 data or unified TLB refill, write
#define ARMV8_EVENT_BUS_ACCESS_LD           0x60    //  - [50] Bus access - Read
#define ARMV8_EVENT_BUS_ACCESS_ST           0x61    //  - [51] Bus access - Write
#define ARMV8_EVENT_BUS_ACCESS_SHARED       0x62    //  - [53:52] Bus access - Normal
#define ARMV8_EVENT_BUS_ACCESS_NOT_SHARED   0x63    //  - [55:54] Bus access - Not normal
#define ARMV8_EVENT_BUS_ACCESS_NORMAL       0x64    //  - [57:56] Bus access - Normal
#define ARMV8_EVENT_BUS_ACCESS_PERIPH       0x65    //  - [59:58] Bus access - Peripheral
#define ARMV8_EVENT_MEM_ACCESS_LD           0x66    //  - [60] Data memory access - Read
#define ARMV8_EVENT_MEM_ACCESS_ST           0x67    //  - [61] Data memory access - Write
#define ARMV8_EVENT_UNALIGNED_LD_SPEC       0x68    //  - [62] Unaligned access - Read
#define ARMV8_EVENT_UNALIGNED_ST_SPEC       0x69    //  - [63] Unaligned access - Write
#define ARMV8_EVENT_UNALIGNED_LDST_SPEC     0x6A    //  - [65:64] Unaligned access
#define ARMV8_EVENT_LDREX_SPEC              0x6C    //  [21] [66] Exclusive operation speculatively executed - LDREX
#define ARMV8_EVENT_STREX_PASS_SPEC         0x6D    //  [22] [67] Exclusive instruction speculatively executed - STREX pass
#define ARMV8_EVENT_STREX_FAIL_SPEC         0x6E    //  [23] [68] Exclusive operation speculatively executed - STREX fail
#define ARMV8_EVENT_LD_SPEC                 0x70    //  - [70:69] Operation speculatively executed - Load
#define ARMV8_EVENT_ST_SPEC                 0x71    //  - [72:71] Operation speculatively executed - Store
#define ARMV8_EVENT_LDST_SPEC               0x72    //  - [74:73] Operation speculatively executed - Load or store
#define ARMV8_EVENT_DP_SPEC                 0x73    //  - [76:75] Operation speculatively executed - Integer data processing
#define ARMV8_EVENT_ASE_SPEC                0x74    //  - [78:77] Operation speculatively executed - Advanced SIMD
#define ARMV8_EVENT_VFP_SPEC                0x75    //  - [80:79] Operation speculatively executed - VFP
#define ARMV8_EVENT_PC_WRITE_SPEC           0x76    //  - [82:81] Operation speculatively executed - Software change of the PC
#define ARMV8_EVENT_CRYPTO_SPEC             0x77    //  - [84:83] Operation speculatively executed, crypto data processing
#define ARMV8_EVENT_BR_IMMED_SPEC           0x78    //  - [85] Branch speculatively executed - Immediate branch
#define ARMV8_EVENT_BR_RETURN_SPEC          0x79    //  - [86] Branch speculatively executed - Procedure return
#define ARMV8_EVENT_BR_INDIRECT_SPEC        0x7A    //  - [87] Branch speculatively executed - Indirect branch
#define ARMV8_EVENT_ISB_SPEC                0x7C    //  - [88] Barrier speculatively executed - ISB
#define ARMV8_EVENT_DSB_SPEC                0x7D    //  [24] [89] Barrier speculatively executed - DSB
#define ARMV8_EVENT_DMB_SPEC                0x7E    //  [24] [90] Barrier speculatively executed - DMB
#define ARMV8_EVENT_EXC_UNDEF               0x81    //  - [91] Exception taken, other synchronous
#define ARMV8_EVENT_EXC_SVC                 0x82    //  - [92] Exception taken, Supervisor Call
#define ARMV8_EVENT_EXC_PABORT              0x83    //  - [93] Exception taken, Instruction Abort
#define ARMV8_EVENT_EXC_DABORT              0x84    //  - [94] Exception taken, Data Abort or SError
#define ARMV8_EVENT_EXC_IRQ                 0x86    //  - [95] Exception taken, IRQ
#define ARMV8_EVENT_EXC_FIQ                 0x87    //  - [96] Exception taken, FIQ
#define ARMV8_EVENT_EXC_SMC                 0x88    //  - [97] Exception taken, Secure Monitor Call
#define ARMV8_EVENT_EXC_HVC                 0x8A    //  - [98] Exception taken, Hypervisor Call
#define ARMV8_EVENT_EXC_TRAP_PABORT         0x8B    //  - [99] Exception taken, Instruction Abort not taken locally
#define ARMV8_EVENT_EXC_TRAP_DABORT         0x8C    //  - [100] Exception taken, Data Abort, or SError not taken locally
#define ARMV8_EVENT_EXC_TRAP_OTHER          0x8D    //  - [101] Exception taken – Other traps not taken locally
#define ARMV8_EVENT_EXC_TRAP_IRQ            0x8E    //  - [102] Exception taken, IRQ not taken locally
#define ARMV8_EVENT_EXC_TRAP_FIQ            0x8F    //  - [103] Exception taken, FIQ not taken locally
#define ARMV8_EVENT_RC_LD_SPEC              0x90    //  - [104] Release consistency instruction speculatively executed – Load- Acquire
#define ARMV8_EVENT_RC_ST_SPEC              0x91    // Release consistency instruction speculatively executed – Store- Release

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/

/* Interface fuctions to access perfromance counters from abstraction layer */
void Bpm_SetEvents(s32 PmcrCfg);
void Xpm_GetEventCounters(u32 *PmCtrValue);

#ifdef __cplusplus
}
#endif

#endif
/**
* @} End of "addtogroup a9_event_counter_apis".
*/