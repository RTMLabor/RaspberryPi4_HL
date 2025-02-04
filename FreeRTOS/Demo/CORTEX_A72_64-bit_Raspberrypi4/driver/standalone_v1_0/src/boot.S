/******************************************************************************
*
*
******************************************************************************/
/*****************************************************************************/
/**
* @file boot.S
*
* @addtogroup a72_boot_code Cortex A72 Processor Boot Code
* @{
* <h2> boot.S </h2>
* The boot code performs minimum configuration which is required for an
* application to run starting from processor's reset state. Below is a
* sequence illustrating what all configuration is performed before control
* reaches to main function.
*
* 1. Program vector table base for exception handling
* 2. Invalidate instruction cache, data cache and TLBs
* 3. Program stack pointer for various exceptin levels
* 4. Configure MMU with short descriptor translation table format and program
*    base address of translation table
* 5. Enable data cache, instruction cache and MMU
* 6. Enable Floating point unit
* 7. Transfer control to _start which clears BSS sections, initializes
*    global timer and runs global constructor before jumping to main
*    application
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------- -------- ---------------------------------------------------
* 1.00a ecm/sdm 10/20/09 Initial version
*
* @note
*
* None.
*
******************************************************************************/

//#include "bparameters.h"
//#include "bcm_mmu.h"
#include "mm.h"
#include "reg_cortexa72.h"

.extern MMUTable
.extern Bcm_DisableMMU
.extern l1ptd
.extern l2pte
.extern l1pte_dummy
.extern memset
.extern memzero
.extern main

.global _prestart
.global _boot
.global _reset_system
.extern __stack
.extern __el0_stack
.extern __el1_stack
.extern __el2_stack
.extern _vector_table
.extern _freertos_vector_table
.extern __bss_start__
.extern __bss_end__

/* Stack Pointer locations for boot code */
.set EL0_stack,		__el0_stack
.set EL1_stack,	    __el1_stack
.set EL2_stack,		__el2_stack


.section ".text.boot"


/* this initializes the various processor modes */

_prestart:
_boot:
    // check processor ID is zero (executing on main core)
	mrs x0, mpidr_el1	//Read Multiprocessor Affinity Register
    and     x1, x1, #0  // run core3 (former core0 #0xFF)
    cmp     x1, #0  
    bne     kick_core3

start_el2:
    // enable AArch64 in EL1.
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi4
    msr     hcr_el2, x0
    isb
    // change execution level to EL1.
    mov     x2, #0x3c5         // D=1, A=1, I=1, F=1 M=EL1h
    msr     spsr_el2, x2
    isb
    adr     x2, start_el1
    msr     elr_el2, x2
    isb
    eret

start_el1:
    // enable FPU
    mrs     x1, cpacr_el1
    mov     x0, #(3 << 20)
    orr     x0, x1, x0
    msr     cpacr_el1, x0
    // set sp
    adrp    x1, stack_top
    mov     sp, x1
    // configure MMU
    ldr     x0, =configure_mmu
    blr     x0
    // clear bss
    ldr     x1, =__bss_start__
    ldr     w2, =__bss_size__
1:  cbz     w2, 2f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 1b

2:  bl      main
    ret

kick_core3:
    mov     x1, #0xF0 // See https://github.com/raspberrypi/tools/blob/master/armstubs/armstub8.S
    ldr     x2, =__start
    str     x2, [x1]
    sev
    wfe
    mov     x0, #0x0
    ret

/*



    cbz x0, master
    b proc_hang
master:
    // disable all interrupts
    //MSR DAIFSET, #0xF
    //dsb sy
    //isb sy

    ldr x0, =SCTLR_VALUE_MMU_DISABLED
    msr sctlr_el1, x0

    ldr x0, =HCR_VALUE
    msr hcr_el2, x0

    ldr x0, =SCR_VALUE
    msr scr_el3, x0

    ldr x0, =SPSR_VALUE
    msr spsr_el3, x0

    //init mmu
    
    ldr x0, =CPACR_EL1_VAL
    msr CPACR_EL1, x0

    ldr x0, =TCR_EL1_VAL
    msr TCR_EL1, x0

    ldr x0, =MAIR_VALUE
    msr MAIR_EL1, x0

    adr x0, el1_entry
    msr elr_el3, x0

    eret

el1_entry:
	
    adrp x0, __bss_start__
    add x0, x0, :lo12:__bss_start__
    adrp x1, __bss_end__
    add x1, x1, :lo12:__bss_end__
    sub x1, x1, x0
    bl memzero
/*
    // LLD
    adr x0, __bss_start__
    adr x1, __bss_end__
    sub x1, x1, x0
    bl memzero
*/
    // LLD
    //bl init_mmu

    // configure MMU
    //ldr     x0, =configure_mmu
    //blr     x0
/*
    mrs x0, sctlr_el1
    mov x1, #SCTLR_MMU_ENABLED
    orr x0, x0, x1
    msr sctlr_el1, x0
*/
	/* Get the revision */
	// see cortex_a72_trm page 331

    // setup stack pointers for different exception levels
    
    // Set stack pointer for EL0
    // ldr x0, =EL0_stack
    // msr sp_el0, x0

    // Set stack pointer for EL1
    // ldr x0, =EL1_stack
    // msr sp_el1, x0

    // Set stack pointer for EL2
    //ldr x0, =EL2_stack
    //msr sp_el2, x0
    
    // init vector table
	/* set VBAR to the _vector_table address in linker script */
	/* 
    ldr	x0, =_vector_table // _vector_table for LLDs vector table and _freertos_vector_table for FreeRTOS vector table
	msr	vbar_el1, x0
	isb
*//*
    mov sp, #LOW_MEMORY          // Move the address into sp
    b       main                  // Jump to C startup code
    and     x0, x0, x0              // No-op
	b proc_hang

proc_hang:                          // Paranoia: we should never get here
		wfe
	b	proc_hang

*/
/*
 *************************************************************************
 *
 * invalidate_dcache - invalidate the entire d-cache by set/way
 *
 * Note:
 * Data Cache Invalidation:
 *  The code starts by selecting the L1 Data cache using CSSELR_EL1.
 *  It reads the cache size and properties using CCSIDR_EL1.
 *  It then loops over all sets and ways, invalidating each cache line using DC IVAC.
 * Instruction Cache Invalidation:
 *  The instruction cache is invalidated using IC IALLU.
 * Barriers:
 *  DSB ISH (Data Synchronization Barrier, Inner Shareable): Ensures that all previous cache maintenance operations have completed before executing subsequent instructions.
 *  ISB (Instruction Synchronization Barrier): Ensures that any instructions following the ISB are fetched from memory after the ISB instruction completes.
 *
 *************************************************************************
 */
 /*
invalidate_dcache:
    // Invalidate entire data cache (L1)
    mov     x0, #0                  // Start at level 0
    msr     CSSELR_EL1, x0          // Select L1 Data cache
    isb                             // Ensure selector change takes effect

    mrs     x1, CCSIDR_EL1          // Read cache size ID
    ands    x2, x1, #0x7            // Extract number of sets (minus 1)
    clz     w3, w2                  // Log2 number of sets
    add     w3, w3, #4              // Convert to line length bits

    // Loop over all sets and ways
    mov     x2, #0
    mov     x4, #0
1:
    mov     x5, x4
    mov     x6, #0
2:
    lsl     x7, x6, x3              // Calculate set index
    orr     x7, x7, x5              // OR in the way number
    dc      ivac, x7                // Invalidate by VA to PoC
    add     x6, x6, #1              // Next set
    cmp     x6, x2
    b.lt    2b

    add     x4, x4, #1              // Next way
    cmp     x4, x1
    b.lt    1b

    // Invalidate entire instruction cache
    ic      iallu
    dsb     ish                     // Ensure completion of cache maintenance
    isb                             // Synchronize context on this processor

    ret
.end
 */
/**
* @} End of "addtogroup a72_boot_code".
*/