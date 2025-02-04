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
* @file xpm_counter.c
*
* This file contains APIs for configuring and controlling the Cortex-A9
* Performance Monitor Events. For more information about the event counters,
* see xpm_counter.h.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a sdm  07/11/11 First release
* 4.2	pkp	 07/21/14 Corrected reset value of event counter in function
*					  Xpm_ResetEventCounters to fix CR#796275
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xpm_counter.h"
#include "bcm_types.h"
#include "reg_cortexa72.h"

/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

typedef const u32 PmcrEventCfg32[BPM_CTRCOUNT];

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

void Bpm_DisableEventCounters(void);
void Bpm_EnableEventCounters (void);
void Bpm_ResetEventCounters (void);

/******************************************************************************/

/****************************************************************************/
/**
*
* @brief	This function disables the Cortex A72 event counters.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void Bpm_DisableEventCounters(void)
{
	u32 Reg;
	mfcp(PMCR_EL0, Reg);

	Reg &= ~(PMCR_EL0_E_BIT);
	/* Disable the event counters */
	mtcp(PMCR_EL0, Reg);
}

/****************************************************************************/
/**
*
* @brief	This function enables the Cortex A72 event counters.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void Bpm_EnableEventCounters(void)
{
	u32 Reg;
	mfcp(PMCR_EL0, Reg);

	Reg |= PMCR_EL0_E_BIT;
	/* Enable the event counters */
	mtcp(PMCR_EL0, Reg);
}

/****************************************************************************/
/**
*
* @brief	This function resets the Cortex A72 event counters.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void Bpm_ResetEventCounters(void)
{
	u32 Reg;
#if defined (__GNUC__) || defined (__ICCARM__)
	mfcp(PMCR_EL0, Reg);
#else	// not adapted to Rpi4
	{ register u32 C15Reg __asm(XREG_CP15_PERF_MONITOR_CTRL);
	  Reg = C15Reg; }
#endif
	Reg |= (PMCR_EL0_P_BIT); /* reset event counters */
	mtcp(PMCR_EL0, Reg);

}

/****************************************************************************/
/**
* @brief	This function configures the Cortex A9 event counters controller,
*			with the event codes, in a configuration selected by the user and
*			enables the counters.
*
* @param	PmcrCfg: Configuration value based on which the event counters
*			are configured. XPM_CNTRCFG* values defined in xpm_counter.h can
*			be utilized for setting configuration.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void Bpm_SetEvents(s32 PmcrCfg) {
    u32 Counter;
    static PmcrEventCfg32 PmcrEvents[] = {
        {
			ARMV8_EVENT_SW_INCR,
			ARMV8_EVENT_L1I_CACHE_REFILL,
			ARMV8_EVENT_L1I_TLB_REFILL,
			ARMV8_EVENT_L1D_CACHE_REFILL,
			ARMV8_EVENT_L1D_CACHE_REFILL,
			ARMV8_EVENT_L1D_TLB_REFILL
		},
		{
			ARMV8_EVENT_INST_RETIRED,
			ARMV8_EVENT_EXC_TAKEN,
			ARMV8_EVENT_EXC_RETURN,
			ARMV8_EVENT_CID_WRITE_RETIRED,
			ARMV8_EVENT_BR_MIS_PRED,
			ARMV8_EVENT_CPU_CYCLES
		},
		{
			ARMV8_EVENT_BR_PRED,
			ARMV8_EVENT_MEM_ACCESS,
			ARMV8_EVENT_L1I_CACHE,
			ARMV8_EVENT_L1D_CACHE_WB,
			ARMV8_EVENT_L2D_CACHE,
			ARMV8_EVENT_L2D_CACHE_REFILL
		},
		{
			ARMV8_EVENT_L2D_CACHE_WB,
			ARMV8_EVENT_BUS_ACCESS,
			ARMV8_EVENT_MEMORY_ERROR,
			ARMV8_EVENT_INST_SPEC,
			ARMV8_EVENT_TTBR_WRITE_RETIRED,
			ARMV8_EVENT_BUS_CYCLES
		},
		{
			ARMV8_EVENT_CHAIN,
			ARMV8_EVENT_L1D_CACHE_RD,
			ARMV8_EVENT_L1D_CACHE_WR,
			ARMV8_EVENT_L1D_CACHE_REFILL_RD,
			ARMV8_EVENT_L1D_CACHE_REFILL_WR,
			ARMV8_EVENT_L1D_CACHE_REFILL_INNER
		},
		{
			ARMV8_EVENT_L1D_CACHE_REFILL_OUTER,
			ARMV8_EVENT_L1D_CACHE_WB_VICTIM,
			ARMV8_EVENT_L1D_CACHE_WB_CLEAN,
			ARMV8_EVENT_L1D_CACHE_INVAL,
			ARMV8_EVENT_L1D_TLB_REFILL_LD,
			ARMV8_EVENT_L1D_TLB_REFILL_ST
		},
		{
			ARMV8_EVENT_L1D_TLB_RD,
			ARMV8_EVENT_L1D_TLB_WR,
			ARMV8_EVENT_L2D_CACHE_LD,
			ARMV8_EVENT_L2D_CACHE_ST,
			ARMV8_EVENT_L2D_CACHE_REFILL_LD,
			ARMV8_EVENT_L2D_CACHE_REFILL_ST
		},
		{
			
			ARMV8_EVENT_L2D_CACHE_WB_VICTIM,
			ARMV8_EVENT_L2D_CACHE_WB_CLEAN,
			ARMV8_EVENT_L2D_CACHE_INVAL,
			ARMV8_EVENT_L2D_TLB_REFILL_RD,
			ARMV8_EVENT_L2D_TLB_REFILL_WR,
			ARMV8_EVENT_BUS_ACCESS_LD,
		},
		{
			
			ARMV8_EVENT_BUS_ACCESS_ST,
			ARMV8_EVENT_BUS_ACCESS_SHARED,
			ARMV8_EVENT_BUS_ACCESS_NOT_SHARED,
			ARMV8_EVENT_BUS_ACCESS_NORMAL,
			ARMV8_EVENT_BUS_ACCESS_PERIPH,
			ARMV8_EVENT_MEM_ACCESS_LD
		},
		{
			
			ARMV8_EVENT_MEM_ACCESS_ST,
			ARMV8_EVENT_UNALIGNED_LD_SPEC,
			ARMV8_EVENT_UNALIGNED_ST_SPEC,
			ARMV8_EVENT_UNALIGNED_LDST_SPEC,
			ARMV8_EVENT_LDREX_SPEC,
			ARMV8_EVENT_STREX_PASS_SPEC
		},
		{
			
			ARMV8_EVENT_STREX_FAIL_SPEC,
			ARMV8_EVENT_LD_SPEC,
			ARMV8_EVENT_ST_SPEC,
			ARMV8_EVENT_LDST_SPEC,
			ARMV8_EVENT_DP_SPEC,
			ARMV8_EVENT_ASE_SPEC
		},
		{
			
			ARMV8_EVENT_VFP_SPEC,
			ARMV8_EVENT_PC_WRITE_SPEC,
			ARMV8_EVENT_CRYPTO_SPEC,
			ARMV8_EVENT_BR_IMMED_SPEC,
			ARMV8_EVENT_BR_RETURN_SPEC,
			ARMV8_EVENT_BR_INDIRECT_SPEC
		},
		{
			
			ARMV8_EVENT_ISB_SPEC,
			ARMV8_EVENT_DSB_SPEC,
			ARMV8_EVENT_DMB_SPEC,
			ARMV8_EVENT_EXC_UNDEF,
			ARMV8_EVENT_EXC_SVC,
			ARMV8_EVENT_EXC_PABORT
		},
		{
			ARMV8_EVENT_EXC_DABORT,
			ARMV8_EVENT_EXC_IRQ,
			ARMV8_EVENT_EXC_FIQ,
			ARMV8_EVENT_EXC_SMC,
			ARMV8_EVENT_EXC_HVC,
			ARMV8_EVENT_EXC_TRAP_PABORT
		},
		{
			
			ARMV8_EVENT_EXC_TRAP_DABORT,
			ARMV8_EVENT_EXC_TRAP_OTHER,
			ARMV8_EVENT_EXC_TRAP_IRQ,
			ARMV8_EVENT_EXC_TRAP_FIQ,
			ARMV8_EVENT_RC_LD_SPEC,
			ARMV8_EVENT_RC_ST_SPEC
		},
		};

    const u32 *ptr = (const u32 *)&PmcrEvents[PmcrCfg];

    Bpm_DisableEventCounters();

    for (Counter = 0U; Counter < BPM_CTRCOUNT; Counter++) {
        // Select event counter
        mtcp(PMSELR_EL0, Counter);

        // Set the event type
        mtcp(PMXEVTYPER_EL0, ptr[Counter]);
    }

    Bpm_ResetEventCounters();
    Bpm_EnableEventCounters();
}
/****************************************************************************/
/**
*
* @brief	This function disables the event counters and returns the counter
*			values.
*
* @param	PmCtrValue: Pointer to an array of type u32 PmCtrValue[6].
*			It is an output parameter which is used to return the PM
*			counter values.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void Xpm_GetEventCounters(u32 *PmCtrValue)
{
	u32 Counter;

	Bpm_DisableEventCounters();

	for(Counter = 0U; Counter < BPM_CTRCOUNT; Counter++) {

		mtcp(PMSELR_EL0, Counter);

#if defined (__GNUC__) || defined (__ICCARM__)
		mfcp(PMCCNTR_EL0, PmCtrValue[Counter]);
#else	// not adapted to Rpi4
		//{ register u32 Cp15Reg __asm(XREG_CP15_PERF_MONITOR_COUNT);
		  //PmCtrValue[Counter] = Cp15Reg; }
#endif
	}
}
