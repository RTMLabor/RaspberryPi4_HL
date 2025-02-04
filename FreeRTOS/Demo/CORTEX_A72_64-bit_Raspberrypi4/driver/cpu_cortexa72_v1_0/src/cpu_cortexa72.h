/******************************************************************************
*

*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file cpu_cortexa72.h
* @addtogroup cpu_cortexa72_v1_0
* @{
* @details
*
* dummy file
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------------
* 1.0   ne   09/10/24 first release
******************************************************************************/
/** @} */
#ifndef CPU_CORTEXA72_H
#define CPU_CORTEXA72_H

#include "bcm_types.h"
#include "bparameters.h"

typedef volatile u32 reg32;
typedef volatile u64 reg64;

//GIC-400 driver-regs

// GIC distributor register map
//gic_architecture_specification on page 406
struct GIC_DistributorRegs {
    reg32 CTLR;                        // Offset: 0x0000 - Distributor Control Register
    reg32 TYPER;                       // Offset: 0x0004 - Interrupt Controller Type Register
    reg32 IIDR;                        // Offset: 0x0008 - Distributor Implementer Identification Register
    reg32 reserved0;                        // Offset: 0x000C - Reserved
    reg32 STATUSR;                     // Offset: 0x0010 - Error Reporting Status Register (optional)
    reg32 reserved1[2];                     // Offset: 0x0014 - 0x001C Reserved
    reg32 IMPLEMENTATION_DEFINED[8];        // Offset: 0x0020 - 0x003C IMPLEMENTATION DEFINED registers
    reg32 SETSPI_NSR;                  // Offset: 0x0040 - Set SPI Register
    reg32 reserved2;                        // Offset: 0x0044 - Reserved
    reg32 CLRSPI_NSR;                  // Offset: 0x0048 - Clear SPI Register
    reg32 reserved3;                        // Offset: 0x004C - Reserved
    reg32 SETSPI_SR;                   // Offset: 0x0050 - Set SPI, Secure Register
    reg32 reserved4;                        // Offset: 0x0054 - Reserved
    reg32 CLRSPI_SR;                   // Offset: 0x0058 - Clear SPI, Secure Register
    reg32 reserved5[8];                     // Offset: 0x005C - 0x007C Reserved
    reg32 IGROUPR[32];                 // Offset: 0x0080 - 0x00FC Interrupt Group Registers
    reg32 ISENABLER[32];               // Offset: 0x0100 - 0x017C Interrupt Set-Enable Registers
    reg32 ICENABLER[32];               // Offset: 0x0180 - 0x01FC Interrupt Clear-Enable Registers
    reg32 ISPENDR[32];                 // Offset: 0x0200 - 0x027C Interrupt Set-Pending Registers
    reg32 ICPENDR[32];                 // Offset: 0x0280 - 0x02FC Interrupt Clear-Pending Registers
    reg32 ISACTIVER[32];               // Offset: 0x0300 - 0x037C Interrupt Set-Active Registers
    reg32 ICACTIVER[32];               // Offset: 0x0380 - 0x03FC Interrupt Clear-Active Registers
    reg32 IPRIORITYR[255];             // Offset: 0x0400 - 0x07F8 Interrupt Priority Registers
    reg32 ITARGETSR[8];                // Offset: 0x0800 - 0x081C Interrupt Processor Targets Registers
    reg32 reserved6[508];                   // Offset: 0x0820 - 0x0BF8 Reserved
    reg32 ICFGR[64];                   // Offset: 0x0C00 - 0x0CFC Interrupt Configuration Registers
    reg32 IGRPMODR[32];                // Offset: 0x0D00 - 0x0D7C Interrupt Group Modifier Registers
    reg32 NSACR[64];                   // Offset: 0x0E00 - 0x0EFC Non-secure Access Control Registers
    reg32 SGIR;                        // Offset: 0x0F00 - Software Generated Interrupt Register
    reg32 CPENDSGIR[4];                // Offset: 0x0F10 - 0x0F1C SGI Clear-Pending Registers
    reg32 SPENDSGIR[4];                // Offset: 0x0F20 - 0x0F2C SGI Set-Pending Registers
    reg32 reserved7[756];                   // Offset: 0x0F30 - 0x60FC Reserved
    reg32 IROUTER[1020];               // Offset: 0x6100 - 0x7FD8 Interrupt Routing Registers
    reg32 reserved8[1020];                  // Offset: 0x7FDC - 0xBFFC Reserved
    reg32 IMPLEMENTATION_DEFINED_2[128];    // Offset: 0xC000 - 0xFFCC IMPLEMENTATION DEFINED
    reg32 Identification[13];          // Offset: 0xFFD0 - 0xFFFC Identification registers
};

#define REGS_GIC_Distributor ((struct GIC_DistributorRegs *) (BCM_RPI_GICD_BASEADDR))

//GIC CPU interface register map
//gic_architecture_specification in page 523
struct GIC_CPUInterfaceRegs {
    reg32 CTLR;        // 0x0000: CPU Interface Control Register
    reg32 PMR;         // 0x0004: Interrupt Priority Mask Register
    reg32 BPR;         // 0x0008: Binary Point Register
    reg32 IAR;         // 0x000C: Interrupt Acknowledge Register
    reg32 EOIR;        // 0x0010: End of Interrupt Register
    reg32 RPR;         // 0x0014: Running Priority Register
    reg32 HPPIR;       // 0x0018: Highest Priority Pending Interrupt Register
    reg32 ABPR;        // 0x001C: Aliased Binary Point Register
    reg32 AIAR;        // 0x0020: Aliased Interrupt Acknowledge Register
    reg32 AEOIR;       // 0x0024: Aliased End of Interrupt Register
    reg32 AHPPIR;      // 0x0028: Aliased Highest Priority Pending Interrupt Register
    reg32 STATUSR;     // 0x002C: Error Reporting Status Register (optional)
    reg32 reserved1[4];     // 0x0030 - 0x003C: Reserved
    reg32 IMPLEMENTATION_DEFINED[48]; // 0x0040 - 0x00CF: IMPLEMENTATION DEFINED registers
    reg32 APR[4];      // 0x00D0 - 0x00DC: Active Priorities Registers
    reg32 NSAPR[4];    // 0x00E0 - 0x00EC: Non-secure Active Priorities Registers
    reg32 reserved2[1];     // 0x00ED - 0x00F8: Reserved
    reg32 IIDR;        // 0x00FC: CPU Interface Identification Register
    reg32 reserved3[960];   // Reserved from 0x0100 to 0x0FFC
    reg32 DIR;         // 0x1000: Deactivate Interrupt Register
};

#define REGS_GIC_CPUInterface ((struct GIC_CPUInterfaceRegs *) (BCM_RPI_GICC_BASEADDR))

//auxilliary
struct AuxRegs {
    reg32 irq_status;
    reg32 enables;
    reg32 reserved[14];
    reg32 mu_io;
    reg32 mu_ier;
    reg32 mu_iir;
    reg32 mu_lcr;
    reg32 mu_mcr;
    reg32 mu_lsr;
    reg32 mu_msr;
    reg32 mu_scratch;
    reg32 mu_control;
    reg32 mu_status;
    reg32 mu_baud_rate;
};

#define REGS_AUX ((struct AuxRegs *)(BCM_RPI_PERIPHERAL_BASEADDR + 0x00215000))

//aarch64 generic timer regs


// generic timer register map
// see on cortex_a72_trm on page 343
struct GTRegs {
    reg32 CNTKCTL_EL1;   // Timer Control register (EL1)
    reg32 reserved1[3];   // Reserved space to align registers
    reg32 CNTFRQ_EL0;     // Timer Counter Frequency register
    reg32 reserved2[3];   // Reserved space
    reg64 CNTPCT_EL0;     // Physical Timer Count register
    reg64 CNTVCT_EL0;     // Virtual Timer Count register
    reg32 CNTP_TVAL_EL0;  // Physical Timer TimerValue (EL0)
    reg32 CNTP_CTL_EL0;   // Physical Timer Control register (EL0)
    reg64 CNTP_CVAL_EL0;  // Physical Timer CompareValue register (EL0)
    reg32 CNTV_TVAL_EL0;  // Virtual Timer TimerValue register
    reg32 CNTV_CTL_EL0;   // Virtual Timer Control register
    reg64 CNTV_CVAL_EL0;  // Virtual Timer CompareValue register
    reg64 CNTVOFF_EL2;    // Virtual Timer Offset register
    reg32 CNTHCTL_EL2;    // Timer Control register (EL2)
    reg32 reserved3[3];   // Reserved space
    reg32 CNTHP_TVAL_EL2;  // Physical Timer TimerValue register (EL2)
    reg32 CNTHP_CTL_EL2;   // Physical Timer Control register (EL2)
    reg64 CNTHP_CVAL_EL2;  // Physical Timer CompareValue register (EL2)
    reg32 CNTPS_TVAL_EL1;  // Physical Timer TimerValue register (EL1)
    reg32 CNTPS_CTL_EL1;   // Physical Secure Timer Control register (EL1)
    reg64 CNTPS_CVAL_EL1;  // Physical Secure Timer CompareValue register (EL1)
};

#define REGS_GT ((struct GTRegs *) (BCM_RPI_GT_BASEADDR))

#endif //CPU_CORTEXA72_H