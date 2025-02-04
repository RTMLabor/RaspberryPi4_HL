/******************************************************************************

* @file bcm_cache.c
*
* Contains required functions for the ARM cache functionality.
*
*/
/***************************** Include Files *********************************/

#include "bcm_cache.h"
#include "bcm_cache_l.h"
#include "bcm_io.h"
#include "bpseudo_asm.h"
//#include "bparameters.h"
#include "reg_cortexa72.h"
#include "bcm_exception.h"

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

#define IRQ_FIQ_MASK 0xC0U	/* Mask IRQ and FIQ interrupts in Current-Process-Status-Register (CPSR) */

#ifdef __GNUC__
	extern s32  _stack_end__;
	extern s32  __el2_stack;
#endif

#ifndef USE_AMP
/****************************************************************************
*
* Perform L2 Cache Sync Operation.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
static void Bcm_L2CacheSync(void)
{
	dsb();
	isb();
}
#endif

/****************************************************************************/
/**
* @brief	Enable the Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_DCacheEnable(void)
{
	Bcm_L1DCacheEnable();
#ifndef USE_AMP
	Bcm_L2CacheEnable();
#endif
}

/****************************************************************************/
/**
* @brief	Disable the Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_DCacheDisable(void)
{
#ifndef USE_AMP
	Bcm_L2CacheDisable();
#endif
	Bcm_L1DCacheDisable();
}

/****************************************************************************/
/**
* @brief	Invalidate the entire Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_DCacheInvalidate(void)
{
	asm_disable_intr_except();
#ifndef USE_AMP
	Bcm_L2CacheInvalidate();
#endif
	Bcm_L1DCacheInvalidate();

	asm_enable_intr_except();
}

/*****************************************************************************/
/**
* @brief	Invalidate a Data cache line. If the byte specified by the address
* 			(adr) is cached by the Data cache, the CACHE_LINE_SIZE containing that
* 			byte is invalidated. If the CACHE_LINE_SIZE is modified (dirty), the
* 			modified contents are lost and are NOT written to the system memory
* 			before the line is invalidated.
*
* @param	adr: 32bit address of the data to be flushed.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Bcm_DCacheInvalidateLine(u32 adr)
{
	asm_disable_intr_except();
#ifndef USE_AMP
	Bcm_L2CacheInvalidateLine(adr);
#endif
	Bcm_L1DCacheInvalidateLine(adr);

	asm_enable_intr_except();
}


/****************************************************************************/
/**
* @brief   Invalidate the data cache for the given address range.
*          If the bytes specified by the address range are cached by the
*          Data cache, the cache lines containing those bytes are invalidated.
*          If the cache lines are modified (dirty), the modified contents are
*          lost and are NOT written to system memory before the lines are invalidated.
*
* @param   adr: 64-bit start address of the range to be invalidated.
* @param   len: Length of the range to be invalidated in bytes.
*
* @return  None.
*
* @note    The bottom 6 bits of the address are ignored, forced by architecture.
*
****************************************************************************/
void Bcm_DCacheInvalidateRange(uintptr_t adr, uintptr_t len)
{
    uintptr_t LocalAddr = adr;
    uintptr_t end = LocalAddr + len;

    // Ensure starting address is aligned to the cache line size
    LocalAddr &= ~(CACHE_LINE_SIZE - 1U);

    // Invalidate each cache line in the specified range
    while (LocalAddr < end)
    {
        asm_inval_dc_line_mva_poc(LocalAddr);
		LocalAddr += CACHE_LINE_SIZE;
    }

    // Synchronize memory operations
    dsb();
}

/****************************************************************************/
/**
* @brief	Flush the entire Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_DCacheFlush(void)
{
	asm_disable_intr_except();
	Bcm_L1DCacheFlush();
#ifndef USE_AMP
	Bcm_L2CacheFlush();
#endif
	asm_enable_intr_except();
}


/****************************************************************************/
/**
 * @brief Flushes a specific cache line from the L1 Data cache and optionally from the L2 cache.
 *
 * This function invalidates the specified cache line in the L1 Data cache and, if USE_AMP is not defined,
 * in the L2 cache as well. It ensures cache coherency and synchronization using platform-specific 
 * functions and settings. After flushing, it restores the previous processor status.
 *
 * @param adr Address of the cache line to flush.
 *
 * @note This function assumes platform-specific cache handling and synchronization mechanisms. 
 *       L2 cache operations are skipped if USE_AMP is defined.
 * 
****************************************************************************/
void Bcm_DCacheFlushLine(u32 adr)
{
	asm_disable_intr_except();
	Bcm_L1DCacheFlushLine(adr);
#ifndef USE_AMP

	Bcm_L2CacheFlushLine(adr);
	Bcm_L2CacheSync();
#endif
	asm_enable_intr_except();
}

/****************************************************************************/
/**
 * @brief Flushes a range of memory addresses from L1 and optionally L2 caches.
 *
 * This function iterates over the specified memory range and flushes each cache line
 * in L1 Data cache. If USE_AMP is not defined, it also flushes corresponding cache 
 * lines in L2 cache. It ensures cache coherency and synchronization using ARM 
 * assembly instructions.
 *
 * @param adr Starting address of the memory range to flush.
 * @param len Length of the memory range to flush in bytes.
 *
 * @note This function assumes a cache line size of 64 bytes for L1 Data cache.
 *       L2 cache flushing is skipped if USE_AMP is defined.
 *
****************************************************************************/
void Bcm_DCacheFlushRange(uintptr_t adr, uintptr_t len)
{
    uintptr_t LocalAddr = adr;
    uintptr_t end = adr + len;
    
	asm_disable_intr_except();

    if (len != 0U) {
        while (LocalAddr < end) {
            // Flush L1 Data cache line
            asm_clean_inval_dc_line_mva_poc(LocalAddr);

#ifndef USE_AMP
            // Flush L2 cache line
			dsb();
        	asm_inval_ic_line_mva_pou();//LocalAddr);
#endif

            // Move to the next cache line
            LocalAddr += CACHE_LINE_SIZE;
        }
    }

    // Ensure all operations complete before restoring cp
    dsb();
    asm_enable_intr_except();
}
/****************************************************************************/
/**
* @brief	Store a Data cache line. If the byte specified by the address (adr)
* 			is cached by the Data cache and the CACHE_LINE_SIZE is modified (dirty),
* 			the entire contents of the CACHE_LINE_SIZE are written to system memory.
* 			After the store completes, the CACHE_LINE_SIZE is marked as unmodified
* 			(not dirty).
*
* @param	adr: 32bit address of the data to be stored.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Bcm_DCacheStoreLine(u32 adr)
{
	asm_disable_intr_except();

	Bcm_L1DCacheStoreLine(adr);
#ifndef USE_AMP
	Bcm_DCacheFlushRange(adr & (~0x1FU), CACHE_LINE_SIZE);
#endif
	asm_enable_intr_except();
}

/***************************************************************************/
/**
* @brief	Enable the instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_ICacheEnable(void)
{
	Bcm_L1ICacheEnable();
#ifndef USE_AMP
	Bcm_L2CacheEnable();
#endif
}

/***************************************************************************/
/**
* @brief	Disable the instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_ICacheDisable(void)
{
#ifndef USE_AMP
	Bcm_L2CacheDisable();
#endif
	Bcm_L1ICacheDisable();
}


/****************************************************************************/
/**
* @brief	Invalidate the entire instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_ICacheInvalidate(void)
{
	asm_disable_intr_except();
#ifndef USE_AMP
	Bcm_L2CacheInvalidate();
#endif
	Bcm_L1ICacheInvalidate();

	asm_enable_intr_except();
}

/****************************************************************************/
/**
* @brief	Invalidate an instruction cache line. If the instruction specified
*			by the address is cached by the instruction cache, the CACHE_LINE_SIZE
*			containing that instruction is invalidated.
*
* @param	adr: 32bit address of the instruction to be invalidated.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Bcm_ICacheInvalidateLine(u32 adr)
{
	asm_disable_intr_except();
#ifndef USE_AMP
	Bcm_L2CacheInvalidateLine(adr);
#endif
	Bcm_L1ICacheInvalidateLine(adr);
	asm_enable_intr_except();
}

/****************************************************************************/
/**
* @brief	Invalidate the instruction cache for the given address range.
* 			If the instructions specified by the address range are cached by
* 			the instrunction cache, the CACHE_LINE_SIZEs containing those
*			instructions are invalidated.
*
* @param	adr: 32bit start address of the range to be invalidated.
* @param	len: Length of the range to be invalidated in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_ICacheInvalidateRange(INTPTR adr, u32 len)
{
	uintptr_t LocalAddr = adr;
	uintptr_t end = adr + len;

	asm_disable_intr_except();

	if (len != 0U) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		LocalAddr = LocalAddr & ~(CACHE_LINE_SIZE - 1U);

		/* Select cache L0 I-cache in CSSELR_EL1 */
        mtcp(CCSIDR_EL1, 1U);

		while (LocalAddr < end) {

			asm_inval_ic_line_mva_pou();//LocalAddr);
			LocalAddr += CACHE_LINE_SIZE;
		}
	}

	/* Wait for L1 and L2 invalidate to complete */
	dsb();
	asm_enable_intr_except();
}

/****************************************************************************/
/**
* @brief	Enable the level 1 Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L1DCacheEnable(void)
{
	register u32 CtrlReg;

	/* enable caches only if they are disabled */
	mfcp(SCTLR_EL1, CtrlReg);

	if ((CtrlReg & (SCTLR_EL1_C_BIT)) != 0U) {
		return;
	}

	/* clean and invalidate the Data cache */
	Bcm_L1DCacheInvalidate();

	/* enable the Data cache */
	CtrlReg |= (SCTLR_EL1_C_BIT);

	mtcp(SCTLR_EL1, CtrlReg);
}

/***************************************************************************/
/**
* @brief	Disable the level 1 Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L1DCacheDisable(void)
{
	register u32 CtrlReg;

	/* clean and invalidate the Data cache */
	Bcm_L1DCacheFlush();

	mfcp(SCTLR_EL1, CtrlReg);

	CtrlReg &= ~(SCTLR_EL1_C_BIT);

	mtcp(SCTLR_EL1, CtrlReg);
}

/****************************************************************************/
/**
* @brief	Invalidate the level 1 Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		In Cortex A9, there is no cp instruction for invalidating
*			the whole D-cache. This function invalidates each line by
*			set/way.
*
****************************************************************************/
void Bcm_L1DCacheInvalidate(void)
{
	register u32 CsidReg, C7Reg;
	u32 CacheSize, LineSize, NumWays;
	u32 Way, WayIndex, Set, SetIndex, NumSet;

#ifdef __GNUC__
	uintptr_t stack_start,stack_end,stack_size;
#endif

	asm_disable_intr_except();

#ifdef __GNUC__
	stack_end = (uintptr_t)&_stack_end__;
	stack_start = (uintptr_t)&__el2_stack;
	stack_size=stack_start-stack_end;

	/*Flush stack memory to save return address*/
	Bcm_DCacheFlushRange(stack_end, stack_size);
#endif

	/* Select cache level 0 and D cache in Cache Size Selection Register (CSSR) */
	mtcp(CSSELR_EL1, 0U);
	// Read Cache Size ID
	mfcp(CCSIDR_EL1, CsidReg);

	/* Determine Cache Size */
	CacheSize = (CsidReg >> 13U) & 0x1FFU;
	CacheSize +=1U;
	CacheSize *=128U;    /* to get number of bytes */

	/* Number of Ways */
	NumWays = (CsidReg & 0x3ffU) >> 3U;
	NumWays += 1U;

	/* Get the CACHE_LINE_SIZE size, way size, index size from csidr */
	LineSize = (CsidReg & 0x07U) + 4U;

	NumSet = CacheSize/NumWays;
	NumSet /= (0x00000001U << LineSize);

	Way = 0U;
	Set = 0U;

	/* Invalidate all the CACHE_LINE_SIZEs */
	for (WayIndex =0U; WayIndex < NumWays; WayIndex++) {
		for (SetIndex =0U; SetIndex < NumSet; SetIndex++) {
			C7Reg = Way | Set;

		/* Invalidate by Set/Way */
#if defined (__GNUC__) || defined (__ICCARM__)
			asm_inval_dc_line_sw(C7Reg);
#else	// not adapted to Rpi4
			/*mtcp(XREG_INVAL_DC_LINE_SW, C7Reg), */
			{ volatile register u32 Reg
				__asm(XREG_INVAL_DC_LINE_SW);
			  Reg = C7Reg; }
#endif
			Set += (0x00000001U << LineSize);
		}
		Set=0U;
		Way += 0x40000000U;
	}

	/* Wait for L1 invalidate to complete */
	dsb();
	asm_enable_intr_except();
}

/****************************************************************************/
/**
* @brief	Invalidate a level 1 Data cache line. If the byte specified by the
* 			address (Addr) is cached by the Data cache, the CACHE_LINE_SIZE
* 			containing that byte is invalidated. If the CACHE_LINE_SIZE is modified
* 			(dirty), the modified contents are lost and are NOT written to
*			system memory before the line is invalidated.
*
* @param	adr: 32bit address of the data to be invalidated.
*
* @return	None.
*
* @note		The bottom 5 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Bcm_L1DCacheInvalidateLine(u32 adr)
{
	mtcp(CSSELR_EL1, 0U);
	asm_inval_dc_line_mva_poc(adr & (~0x1FU));

	/* Wait for L1 invalidate to complete */
	dsb();
}

/****************************************************************************/
/**
* @brief	Invalidate the level 1 Data cache for the given address range.
* 			If the bytes specified by the address range are cached by the Data
* 			cache, the CACHE_LINE_SIZEs containing those bytes are invalidated. If the
* 			CACHE_LINE_SIZEs are modified (dirty), the modified contents are lost and
* 			NOT written to the system memory before the lines are invalidated.
*
* @param	adr: 32bit start address of the range to be invalidated.
* @param	len: Length of the range to be invalidated in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L1DCacheInvalidateRange(u32 adr, u32 len)
{
	u32 LocalAddr = adr;
	u32 end;
	asm_disable_intr_except();

	if (len != 0U) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = LocalAddr + len;
		LocalAddr = LocalAddr & ~(CACHE_LINE_SIZE - 1U);

		/* Select cache L0 D-cache in CSSR */
		mtcp(CSSELR_EL1, 0);

		while (LocalAddr < end) {

#if defined (__GNUC__) || defined (__ICCARM__)
			asm_inval_dc_line_mva_poc(LocalAddr);
#else	// not adapted to RPi4
			{ volatile register u32 Reg
				__asm(XREG_INVAL_DC_LINE_MVA_POC);
			  Reg = LocalAddr; }
#endif
			LocalAddr += CACHE_LINE_SIZE;
		}
	}

	/* Wait for L1 invalidate to complete */
	dsb();
	asm_enable_intr_except();
}

/****************************************************************************/
/**
* @brief	Flush the level 1 Data cache.
*
* @param	None.
*
* @return	None.
*
****************************************************************************/
void Bcm_L1DCacheFlush(void)
{	
	u32 ccsidReg, clidReg, num_sets, num_ways, set, way, LocalAddress;
	asm_disable_intr_except();

	/* Select cache level 0 and D cache in CSSR */
	mtcp(CSSELR_EL1, 0);

	// Read the CLIDR_EL1
    mfcp(CLIDR_EL1, clidReg);

    // Check if Level 1 data cache is present
    if (((clidReg >> 24) & 0x7) == 0x2) {
        // Read CCSIDR_EL1
        mfcp(CCSIDR_EL1, ccsidReg);

        num_sets = ((ccsidReg >> 13) & 0x7FFF) + 1;
        num_ways = ((ccsidReg >> 3) & 0x3FF) + 1;

        for (set = 0; set < num_sets; set++) {
            for (way = 0; way < num_ways; way++) {
                LocalAddress = (set << 6) | (way << (32 - __builtin_clz(num_ways) - 1));
                asm_clean_inval_dc_line_mva_poc(LocalAddress);
            }
        }
    }

	/* Wait for L1 flush to complete */
	dsb();
	isb();
	asm_enable_intr_except();
}

/****************************************************************************/
/**
* @brief	Flush a level 1 Data cache line. If the byte specified by the
*			address (adr) is cached by the Data cache, the CACHE_LINE_SIZE containing
*			that byte is invalidated. If the CACHE_LINE_SIZE is modified (dirty), the
* 			entire contents of the CACHE_LINE_SIZE are written to system memory
*			before the line is invalidated.
*
* @param	adr: 32bit address of the data to be flushed.
*
* @return	None.
*
* @note		The bottom 5 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Bcm_L1DCacheFlushLine(u32 adr)
{
	mtcp(CSSELR_EL1, 0U);
	asm_clean_inval_dc_line_mva_poc(adr & (~0x1FU));

	/* Wait for L1 flush to complete */
	dsb();
}

/****************************************************************************/
/**
* @brief	Flush the level 1  Data cache for the given address range.
* 			If the bytes specified by the address range are cached by the Data
* 			cache, the CACHE_LINE_SIZE containing those bytes are invalidated. If the
* 			CACHE_LINE_SIZEs are modified (dirty), they are written to system memory
* 			before the lines are invalidated.
*
* @param	adr: 32bit start address of the range to be flushed.
* @param	len: Length of the range to be flushed in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L1DCacheFlushRange(u32 adr, u32 len)
{
	u32 LocalAddr = adr;
	u32 end;
	asm_disable_intr_except();

	if (len != 0U) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = LocalAddr + len;
		LocalAddr = LocalAddr & ~(CACHE_LINE_SIZE - 1U);

		/* Select cache L0 D-cache in CSSR */
		mtcp(CSSELR_EL1, 0U);

		while (LocalAddr < end) {

#if defined (__GNUC__) || defined (__ICCARM__)
			asm_clean_inval_dc_line_mva_poc(LocalAddr);
#else	// not adapted to Rpi4
			{ volatile register u32 Reg
				__asm(XREG_CLEAN_INVAL_DC_LINE_MVA_POC);
			  Reg = LocalAddr; }
#endif
			LocalAddr += CACHE_LINE_SIZE;
		}
	}

	/* Wait for L1 flush to complete */
	dsb();
	asm_enable_intr_except();
}

/****************************************************************************/
/**
* @brief	Store a level 1  Data cache line. If the byte specified by the
* 			address (adr) is cached by the Data cache and the CACHE_LINE_SIZE is
* 			modified (dirty), the entire contents of the CACHE_LINE_SIZE are written
* 			to system memory. After the store completes, the CACHE_LINE_SIZE is
*			marked as unmodified (not dirty).
*
* @param	Address to be stored.
*
* @return	None.
*
* @note		The bottom 5 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Bcm_L1DCacheStoreLine(u32 adr)
{
	mtcp(CSSELR_EL1, 0U);
	asm_clean_dc_line_mva_poc(adr & (~0x1FU));

	/* Wait for L1 store to complete */
	dsb();
}


/****************************************************************************/
/**
* @brief	Enable the level 1 instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L1ICacheEnable(void)
{
	register u32 CtrlReg;

	/* enable caches only if they are disabled */
	mfcp(SCTLR_EL1, CtrlReg);

	if ((CtrlReg & (SCTLR_EL1_I_BIT)) != 0U) {
		return;
	}

	/* invalidate the instruction cache */
	asm_inval_ic_line_mva_pou();//0U);

	/* enable the instruction cache */
	CtrlReg |= (SCTLR_EL1_I_BIT);

	mtcp(SCTLR_EL1, CtrlReg);
}

/****************************************************************************/
/**
* @brief	Disable level 1 the instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L1ICacheDisable(void)
{
	register u32 CtrlReg;

	dsb();

	/* invalidate the instruction cache */
	asm_inval_ic_line_mva_pou();//0U);

	/* disable the instruction cache */
	mfcp(SCTLR_EL1, CtrlReg);

	CtrlReg &= ~(SCTLR_EL1_I_BIT);

	mtcp(SCTLR_EL1, CtrlReg);
}

/****************************************************************************/
/**
* @brief	Invalidate the entire level 1 instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L1ICacheInvalidate(void)
{
	mtcp(CSSELR_EL1, 1U);
	/* invalidate the instruction cache */
	asm_inval_ic_line_mva_pou();//0U);

	/* Wait for L1 invalidate to complete */
	dsb();
}

/****************************************************************************/
/**
* @brief	Invalidate a level 1  instruction cache line. If the instruction
*			specified by the address is cached by the instruction cache, the
*			CACHE_LINE_SIZE containing that instruction is invalidated.
*
* @param	adr: 32bit address of the instruction to be invalidated.
*
* @return	None.
*
* @note		The bottom 5 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Bcm_L1ICacheInvalidateLine(u32 adr)
{
	mtcp(CSSELR_EL1, 1U);
	asm_inval_ic_line_mva_pou();//adr & (~0x1FU));

	/* Wait for L1 invalidate to complete */
	dsb();
	
	/* to avoid warning unused parameters */
	( void ) adr;
}

/****************************************************************************/
/**
* @brief	Invalidate the level 1 instruction cache for the given address
* 			range. If the instrucions specified by the address range are cached
*			by the instruction cache, the CACHE_LINE_SIZE containing those bytes are
*			invalidated.
*
* @param	adr: 32bit start address of the range to be invalidated.
* @param	len: Length of the range to be invalidated in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L1ICacheInvalidateRange(u32 adr, u32 len)
{
	u32 LocalAddr = adr;
	u32 end;
	asm_disable_intr_except();

	if (len != 0U) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = LocalAddr + len;
		LocalAddr = LocalAddr & ~(CACHE_LINE_SIZE - 1U);

		/* Select cache L0 I-cache in CSSR */
		mtcp(CSSELR_EL1, 1U);

		while (LocalAddr < end) {

#if defined (__GNUC__) || defined (__ICCARM__)
			asm_inval_ic_line_mva_pou();//LocalAddr);
#else	// not adapted to Rpi4
			{ volatile register u32 Reg
				__asm(XREG_INVAL_IC_LINE_MVA_POU);
			  Reg = LocalAddr; }
#endif
			LocalAddr += CACHE_LINE_SIZE;
		}
	}

	/* Wait for L1 invalidate to complete */
	dsb();
	asm_enable_intr_except();
}

#ifndef USE_AMP
/****************************************************************************/
/**
* @brief	Enable the L2 cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L2CacheEnable(void)
{
	 uint64_t sctlr_el1;

    // Read the current value of SCTLR_EL1
    mfcp(SCTLR_EL1, sctlr_el1);

    // Check if the L2 cache is already enabled
    if ((sctlr_el1 & SCTLR_EL1_C_BIT) == 0) {
		// Invalidate the entire unified TLB
        asm_inval_all_tlbi();

        // Invalidate the data cache
        asm_inval_dc_line_mva_poc(0);

        // Enable the L2 cache by setting the C bit (bit 2) in SCTLR_EL1
        sctlr_el1 |= SCTLR_EL1_C_BIT;
        // Write back the updated value to SCTLR_EL1
        mtcp(SCTLR_EL1, sctlr_el1);

        // Synchronize the processor
        dsb();
    }
}

/****************************************************************************/
/**
* @brief	Disable the L2 cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L2CacheDisable(void)
{
    uint64_t sctlr_el1;

    // Read the current value of SCTLR_EL1
    mfcp(SCTLR_EL1, sctlr_el1);

    // Check if the L2 cache is enabled
    if ((sctlr_el1 & (SCTLR_EL1_C_BIT)) != 0) {
        // Clean and invalidate the entire data cache
        asm_clean_inval_dc_line_sw(0);
        
        // Invalidate the instruction cache
        asm_inval_ic_line_mva_pou();//0);

        // Disable the L2 cache by clearing the C bit (bit 2) in SCTLR_EL1
        sctlr_el1 &= ~ SCTLR_EL1_C_BIT;

        // Write back the updated value to SCTLR_EL1
        mtcp(SCTLR_EL1, sctlr_el1);

        // Synchronize the processor
        dsb();
    }
}

/*****************************************************************************/
/**
* @brief	Invalidate the entire level 2 cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L2CacheInvalidate(void)
{
	#ifdef __GNUC__
	uintptr_t stack_start,stack_end,stack_size;
	stack_end = (uintptr_t)&_stack_end__;
	stack_start = (uintptr_t)&__el2_stack;
	stack_size=stack_start-stack_end;

	/*Flush stack memory to save return address*/
	Bcm_DCacheFlushRange(stack_end, stack_size);
	#endif
	/* Invalidate the caches */

	asm_inval_dc_line_mva_poc(0);

	asm_inval_ic_line_inner_shareable_pou();//0);

	/* Wait for the invalidate to complete */
	Bcm_L2CacheSync();

	/* synchronize the processor */
	dsb();
}

/*****************************************************************************/
/**
* @brief	Invalidate a level 2 cache line. If the byte specified by the
*			address (adr) is cached by the L2 cache, the CACHE_LINE_SIZE containing
*			that byte is invalidated. If the CACHE_LINE_SIZE is modified (dirty),
*			the modified contents are lost and are NOT written to system memory
*			before the line is invalidated.
*
* @param	adr: 32-bit address of the data/instruction to be invalidated.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Bcm_L2CacheInvalidateLine(u32 adr)
{
    /* Invalidate L2 cache line */
    asm_clean_inval_dc_line_mva_poc(adr);

    /* Ensure the operation completes before continuing */
    dsb();
}

/****************************************************************************/
/**
* @brief	Invalidate the level 2 cache for the given address range.
* 			If the bytes specified by the address range are cached by the L2
* 			cache, the CACHE_LINE_SIZE containing those bytes are invalidated. If the
* 			CACHE_LINE_SIZEs are modified (dirty), the modified contents are lost and
* 			are NOT written to system memory before the lines are invalidated.
*
* @param	adr: 32-bit start address of the range to be invalidated.
* @param	len: Length of the range to be invalidated in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L2CacheInvalidateRange(u32 adr, u32 len)
{
    u32 start_addr = adr;
    u32 end_addr = adr + len;

    // Disable interrupts and exceptions
    asm_disable_intr_except();

    while (start_addr < end_addr) {
        // Invalidate L2 cache line by line
        asm_inval_dc_line_mva_poc(start_addr);

        // Move to the next CACHE_LINE_SIZE boundary
        start_addr += CACHE_LINE_SIZE;
    }

    // Ensure all operations complete before continuing
    dsb();

    // Restore previous state (enable interrupts and exceptions)
    asm_enable_intr_except();
}

/****************************************************************************/
/**
* @brief	Flush the entire level 2 cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L2CacheFlush(void)
{
    u64 clidr_el1, csselr_el1, ccsidr_el1, level, ctype, sets, ways, set, way; //, cache_line_size;

    // Read Cache Level ID Register
	mfcp(CLIDR_EL1, clidr_el1);

    // Iterate through cache levels (L1, L2, L3, etc.)
    for (level = 0; level < 7; level++) {
        ctype = (clidr_el1 >> (level * 3)) & 0x7;

        if (ctype == 0) {
            break;  // No more levels
        }

        if ((ctype == 2) || (ctype == 3) || (ctype == 4)) {
            // L2 cache type (Unified or Data cache)
            csselr_el1 = (level << 1);
			mtcp(CSSELR_EL1, csselr_el1);
            isb();  // Ensure selection is synchronized

            mfcp(CCSIDR_EL1, ccsidr_el1);

            sets = ((ccsidr_el1 >> 13) & 0x7FFF) + 1;
            ways = ((ccsidr_el1 >> 3) & 0x3FF) + 1;
            //cache_line_size = 1 << ((ccsidr_el1 & 0x7) + 4);

            for (set = 0; set < sets; set++) {
                for (way = 0; way < ways; way++) {
                    uint64_t sw = (set << (32 - __builtin_clz(sets - 1))) | (way << (32 - __builtin_clz(ways - 1)));
					asm_clean_inval_dc_line_sw(sw);
                }
            }

            dsb();  // Ensure all cache maintenance operations complete
        }
    }

    // Ensure completion of cache maintenance
    Bcm_L2CacheSync();
}

/****************************************************************************/
/**
* @brief	Flush a level 2 cache line. If the byte specified by the address
* 			(adr) is cached by the L2 cache, the CACHE_LINE_SIZE containing that
* 			byte is invalidated. If the CACHE_LINE_SIZE is modified (dirty), the
* 			entire contents of the CACHE_LINE_SIZE are written to system memory
* 			before the line is invalidated.
*
* @param	adr: 32bit address of the data/instruction to be flushed.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Bcm_L2CacheFlushLine(u32 adr)
{
	asm_inval_dc_line_mva_poc(adr);
	/* synchronize the processor */
	dsb();
}

/****************************************************************************/
/**
* @brief	Flush the level 2 cache for the given address range.
* 			If the bytes specified by the address range are cached by the L2
* 			cache, the CACHE_LINE_SIZE containing those bytes are invalidated. If the
* 			CACHE_LINE_SIZEs are modified (dirty), they are written to the system
* 			memory before the lines are invalidated.
*
* @param	adr: 32bit start address of the range to be flushed.
* @param	len: Length of the range to be flushed in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Bcm_L2CacheFlushRange(u32 adr, u32 len)
{
	u32 LocalAddr = adr;
	u32 end = adr + len;

	asm_disable_intr_except();

	if (len != 0U) {
		while (LocalAddr < end)
		{
			// Flush L2 cache line
			asm_clean_inval_dc_line_mva_poc(LocalAddr);

			// Motve to the next cache line
			LocalAddr += CACHE_LINE_SIZE;
		}	
	}
	/* synchronize the processor */
	dsb();
	asm_enable_intr_except();
}
#endif