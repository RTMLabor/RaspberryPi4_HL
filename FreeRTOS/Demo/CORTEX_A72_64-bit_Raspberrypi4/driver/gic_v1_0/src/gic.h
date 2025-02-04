#ifndef __GIC_H
#define __GIC_H

#include <stdint.h>
#include "bcm_exception.h"
#include "cpu_cortexa72.h"
#include "gic_def.h"

/* Functions */
int isr_register(u32 Int_Id, u32 pri, u32 cpumask, void (*Handler)(void *));
void eoi_notify(uint32_t val);
void wait_gic_init(void);

void show_invalid_entry_message(u32 type, u64 esr, u64 address);
void handle_irq();

// test purpose only
/*
void enable_interrupt_controller();
void irq_init_vectors();
void irq_enable();
void irq_disable();

unsigned long get_el(void);
*/

//6.2.4
// video core (vc) irqs
enum vc_irqs {
    SYS_TIMER_IRQ_0 = 1,
    SYS_TIMER_IRQ_1 = 2,
    SYS_TIMER_IRQ_2 = 4,
    SYS_TIMER_IRQ_3 = 8,
    AUX_IRQ = (1 << 29)
};

// RPi 4
struct arm_irq_regs_2711 {
    reg32 irq0_pending_0;
    reg32 irq0_pending_1;
    reg32 irq0_pending_2;
    reg32 res0;
    reg32 irq0_enable_0;
    reg32 irq0_enable_1;
    reg32 irq0_enable_2;
    reg32 res1;
    reg32 irq0_disable_0;
    reg32 irq0_disable_1;
    reg32 irq0_disable_2;
};

// RPi 3B
struct arm_irq_regs_2837 {
    reg32 irq0_pending_0;
    reg32 irq0_pending_1;
    reg32 irq0_pending_2;
    reg32 fiq_control;
    reg32 irq0_enable_1;
    reg32 irq0_enable_2;
    reg32 irq0_enable_0;
    reg32 res;
    reg32 irq0_disable_1;
    reg32 irq0_disable_2;
    reg32 irq0_disable_0;
};

typedef struct arm_irq_regs_2711 arm_irq_regs;

#define REGS_IRQ ((arm_irq_regs *)(BCM_RPI_PERIPHERAL_BASEADDR + 0x0000B200))


/* The following data type defines each entry in an interrupt vector table.
 * The callback reference is the base address of the interrupting device
 * for the low level driver and an instance pointer for the high level driver.
 */
typedef struct
{
	Bcm_InterruptHandler Handler;
	void *CallBackRef;
} BcmGIC_VectorTableEntry;

/**
 * This typedef contains configuration information for the device.
 */
typedef struct
{
	u16 DeviceId;		/**< Unique ID  of device */
	u32 CpuBaseAddress;	/**< CPU Interface Register base address */
	u32 DistBaseAddress;	/**< Distributor Register base address */
	BcmGIC_VectorTableEntry HandlerTable[BGIC_MAX_NUM_INTR_INPUTS];/**<
				 Vector table of interrupt handlers */
} BcmGIC_Config;

/**
 * The BcmGIC driver instance data. The user is required to allocate a
 * variable of this type for every intc device in the system. A pointer
 * to a variable of this type is then passed to the driver API functions.
 */
typedef struct
{
	BcmGIC_Config *Config;  /**< Configuration table entry */
	u32 IsReady;		 /**< Device is initialized and ready */
	u32 UnhandledInterrupts; /**< Intc Statistics */
} BcmGIC;


/************************** Function Prototypes ******************************/

/*
 * Required functions in gic.c
 */

s32  BcmGic_CfgInitialize(BcmGIC *InstancePtr, BcmGIC_Config *ConfigPtr,
				u32 EffectiveAddr);

BcmGIC_Config *BcmGic_LookupConfig(u16 DeviceId);

s32  BcmGic_Connect(BcmGIC *InstancePtr, u32 Int_Id,
                      Bcm_InterruptHandler Handler, void *CallBackRef);
void BcmGic_Disconnect(BcmGIC *InstancePtr, u32 Int_Id);

void BcmGic_Enable(BcmGIC *InstancePtr, u32 Int_Id);
void BcmGic_Disable(BcmGIC *InstancePtr, u32 Int_Id);
					  
void BcmGic_GetPriorityTriggerType(BcmGIC *InstancePtr, u32 Int_Id,
					u8 *Priority, u8 *Trigger);
void BcmGic_SetPriorityTriggerType(BcmGIC *InstancePtr, u32 Int_Id,
					u8 Priority, u8 Trigger);
void BcmGic_InterruptMaptoCpu(BcmGIC *InstancePtr, u8 Cpu_Id, u32 Int_Id);

#endif //__GIC_H