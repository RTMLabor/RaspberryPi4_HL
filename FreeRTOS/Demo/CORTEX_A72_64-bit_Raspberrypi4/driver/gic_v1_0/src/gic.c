#include "gic.h"
#include "bparameters.h"
#include "bstatus.h"
#include "printf.h"
#include "mini_uart.h"


// error codes for IRQ
const char entry_error_messages[16][32] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32"	
};

/************************** Function Prototypes ******************************/

static void StubHandler(void *CallBackRef);

/*****************************************************************************/
/**
*
* A stub for the asynchronous callback. The stub is here in case the upper
* layers forget to set the handler.
*
* @param	CallBackRef is a pointer to the upper layer callback reference
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void StubHandler( void *CallBackRef) {
	/*
	 * verify that the inputs are valid
	 */
	Bcm_AssertVoid(CallBackRef != NULL);

	/*
	 * Indicate another unhandled interrupt for stats
	 */
	((BcmGIC *)((void *)CallBackRef))->UnhandledInterrupts++;
}

/* Vector table */
BcmGIC_VectorTableEntry InterruptHandlerFunctionTable[BGIC_MAX_NUM_INTR_INPUTS] = {0};

/* Interrupt handler registration */
int isr_register(u32 Int_Id, u32 pri, u32 cpumask, void (*Handler)(void *))
{
    u32 Mask, reg, shift;

    if (Int_Id > 0xFFU) {
        return -1;
    }
    if (pri > 0xFFU) {
        return -2;
    }
    if (cpumask > 0xFFU) {
        return -3;
    }
    if (!Handler) {
        return -4;
    }

    /* GICD_ISENABLERn */
	Mask = 0x00000001U << (Int_Id % 32U);
	REGS_GIC_Distributor->ISENABLER[(Int_Id / 32U) * 4U] = Mask;
    
    /* GICD_IPRIORITYRn */
	reg = REGS_GIC_Distributor->IPRIORITYR[(Int_Id / 4U)];
    shift = (Int_Id % 4U) * 8U;
	reg &= ~(0xFFU << shift); // Clear the old priority bits
    REGS_GIC_Distributor->IPRIORITYR[(Int_Id / 4U)] = (reg | pri << shift); // Set the new priority
    
    /* GICD_ITARGETSRn (only for SPIs) */
	reg = REGS_GIC_Distributor->ITARGETSR[Int_Id / 4U];
	shift = (Int_Id % 4U) * 8U;

	// Clear the old CPU mask bits and set the new mask
	reg &= ~(0xFFU << shift); // Clear the bits for the specific interrupt
	REGS_GIC_Distributor->ITARGETSR[Int_Id / 4U] = reg | (cpumask << shift); // Set the new CPU mask

    asm volatile ("isb");

    /* Handler registration */
    InterruptHandlerFunctionTable[Int_Id].Handler = Handler;
    
    return BST_SUCCESS;
}   
/*-----------------------------------------------------------*/

/* EOI notification */
void eoi_notify(u32 val)
{
    REGS_GIC_CPUInterface->EOIR = val;
    asm volatile ("isb");

    return;
}   
/*-----------------------------------------------------------*/

/*
 * wait_gic_init()
 * To check GIC initialization by Linux
 */
void wait_gic_init(void)
{
    reg32 *addr;

    addr = &(REGS_GIC_Distributor->CTLR);

    while (*addr == 0x1U) { /* Wait until Linux disables GICD to set it up */
        ;
    }
    while (*addr == 0x0U) { /* Wait until Linux enables GICD again after completing GICD setting up */
        ;
    }

    return;
}
/*-----------------------------------------------------------*/

BcmGIC_Config BcmGic_ConfigTable[BPAR_GIC_NUM_INSTANCES] =
{
	{
		BCM_RPI_GIC_DEVICE_ID,
		BCM_RPI_GICC_BASEADDR,
		BCM_RPI_GICD_BASEADDR,
		{{0}}		/**< Initialize the HandlerTable to 0 */
	}
};

/*****************************************************************************/
/**
*
* Looks up the device configuration based on the unique device ID. A table
* contains the configuration info for each device in the system.
*
* @param	DeviceId is the unique identifier for a device.
*
* @return	A pointer to the BcmGic configuration structure for the
*		specified device, or NULL if the device was not found.
*
* @note		None.
*
******************************************************************************/
BcmGIC_Config *BcmGic_LookupConfig(u16 DeviceId)
{
	BcmGIC_Config *CfgPtr = NULL;
	u32 Index;

	for (Index=0U; Index < (u32)BPAR_GIC_NUM_INSTANCES; Index++) {
		if (BcmGic_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &BcmGic_ConfigTable[Index];
			break;
		}
	}

	return (BcmGIC_Config *)CfgPtr;
}

/*****************************************************************************/
/**
*
* DoDistributorInit initializes the distributor of the GIC. The
* initialization entails:
*
* - Write the trigger mode, priority and target CPU
* - All interrupt sources are disabled
* - Enable the distributor
*
* @param	InstancePtr is a pointer to the XScuGic instance.
* @param	CpuID is the Cpu ID to be initialized.
*
* @return	None
*
* @note		None.
*
******************************************************************************/
static void DoDistributorInit(BcmGIC *InstancePtr, u32 CpuID)
{
	u32 Int_Id;
	u32 LocalCpuID = CpuID;

	REGS_GIC_Distributor->CTLR = 0U;

	/*
	 * Set the security domains in the int_security registers for
	 * non-secure interrupts
	 * All are secure, so leave at the default. Set to 1 for non-secure
	 * interrupts.
	 */

	/*
	 * For the Shared Peripheral Interrupts INT_ID[MAX..32], set:
	 */

	/*
	 * 1. The trigger mode in the int_config register
	 * Only write to the SPI interrupts, so start at 32
	 */
	for (Int_Id = 32U; Int_Id < BGIC_MAX_NUM_INTR_INPUTS; Int_Id=Int_Id+16U) {
		/*
		 * Each INT_ID uses two bits, or 16 INT_ID per register
		 * Set them all to be level sensitive, active HIGH.
		 */
		REGS_GIC_Distributor->ICFGR[Int_Id / 16U] = 0U;
	}


#define DEFAULT_PRIORITY    0xa0a0a0a0U
	for (Int_Id = 0U; Int_Id < BGIC_MAX_NUM_INTR_INPUTS; Int_Id=Int_Id+4U) {
		/*
		 * 2. The priority using int the priority_level register
		 * The priority_level and spi_target registers use one byte per
		 * INT_ID.
		 * Write a default value that can be changed elsewhere.
		 */
		REGS_GIC_Distributor->IPRIORITYR[Int_Id] = DEFAULT_PRIORITY;
	}

	for (Int_Id = 32U; Int_Id<BGIC_MAX_NUM_INTR_INPUTS;Int_Id=Int_Id+4U) {
		/*
		 * 3. The CPU interface in the spi_target register
		 * Only write to the SPI interrupts, so start at 32
		 */
		LocalCpuID |= LocalCpuID << 8U;
		LocalCpuID |= LocalCpuID << 16U;

		REGS_GIC_Distributor->ITARGETSR[Int_Id] = LocalCpuID;
	}

	for (Int_Id = 0U; Int_Id<BGIC_MAX_NUM_INTR_INPUTS;Int_Id=Int_Id+32U) {
		/*
		 * 4. Enable the SPI using the enable_set register. Leave all
		 * disabled for now.
		 */
		REGS_GIC_Distributor->ICENABLER[Int_Id / 32U] = 0xFFFFFFFFU;

	}

	REGS_GIC_Distributor->CTLR = BGIC_EN_INTR_MASK;

	/* to avoid warning unused parameters */
	( void ) InstancePtr;
}

/*****************************************************************************/
/**
*
* DistributorInit initializes the distributor of the GIC. It calls
* DoDistributorInit to finish the initialization.
*
* @param	InstancePtr is a pointer to the XScuGic instance.
* @param	CpuID is the Cpu ID to be initialized.
*
* @return	None
*
* @note		None.
*
******************************************************************************/
static void DistributorInit(BcmGIC *InstancePtr, u32 CpuID)
{
	u32 Int_Id;
	u32 LocalCpuID = CpuID;
	reg32 RegValue;

#if USE_AMP==1 && (defined (ARMA9) || defined(__aarch64__))
#warning "Building GIC for AMP"
	/*
	 * GIC initialization is taken care by master CPU in
	 * openamp configuration, so do nothing and return.
	 */
	return;
#endif

	RegValue = REGS_GIC_Distributor->CTLR;
	if (!(RegValue & BGIC_EN_INTR_MASK)) {
		Bcm_AssertVoid(InstancePtr != NULL);
		DoDistributorInit(InstancePtr, CpuID);
		return;
	}

	/*
	 * The overall distributor should not be initialized in AMP case where
	 * another CPU is taking care of it.
	 */
	LocalCpuID |= LocalCpuID << 8U;
	LocalCpuID |= LocalCpuID << 16U;
	for (Int_Id = 32U; Int_Id<BGIC_MAX_NUM_INTR_INPUTS;Int_Id=Int_Id+4U) {

		RegValue = REGS_GIC_Distributor->ITARGETSR[Int_Id/4];
		RegValue &= ~(0xFF << ((Int_Id % 4) * 8));
		RegValue |= (LocalCpuID << ((Int_Id % 4) * 8));
		REGS_GIC_Distributor->ITARGETSR[Int_Id/4] = RegValue;
	}
}

/*****************************************************************************/
/**
*
* CPUInitialize initializes the CPU Interface of the GIC. The initialization entails:
*
*	- Set the priority of the CPU
*	- Enable the CPU interface
*
* @param	InstancePtr is a pointer to the XScuGic instance.
*
* @return	None
*
* @note		None.
*
******************************************************************************/
static void CPUInitialize(BcmGIC *InstancePtr)
{
	/*
	 * Program the priority mask of the CPU using the Priority mask register
	 */
	REGS_GIC_CPUInterface->PMR = 0xF0U;


	/*
	 * If the CPU operates in both security domains, set parameters in the
	 * control_s register.
	 * 1. Set FIQen=1 to use FIQ for secure interrupts,
	 * 2. Program the AckCtl bit
	 * 3. Program the SBPR bit to select the binary pointer behavior
	 * 4. Set EnableS = 1 to enable secure interrupts
	 * 5. Set EnbleNS = 1 to enable non secure interrupts
	 */

	/*
	 * If the CPU operates only in the secure domain, setup the
	 * control_s register.
	 * 1. Set FIQen=1,
	 * 2. Set EnableS=1, to enable the CPU interface to signal secure interrupts.
	 * Only enable the IRQ output unless secure interrupts are needed.
	 */
	REGS_GIC_CPUInterface->CTLR = 0x07U;
	
	/* to avoid warning unused parameters */
	( void ) InstancePtr;

}
/*****************************************************************************/
/**
*
* CfgInitialize a specific interrupt controller instance/driver. The
* initialization entails:
*
* - Initialize fields of the BcmGIC structure
* - Initial vector table with stub function calls
* - All interrupt sources are disabled
*
* @param	InstancePtr is a pointer to the BcmGIC instance.
* @param	ConfigPtr is a pointer to a config table for the particular
*		device this driver is associated with.
* @param	EffectiveAddr is the device base address in the virtual memory
*		address space. The caller is responsible for keeping the address
*		mapping from EffectiveAddr to the device physical base address
*		unchanged once this function is invoked. Unexpected errors may
*		occur if the address mapping changes after this function is
*		called. If address translation is not used, use
*		Config->BaseAddress for this parameters, passing the physical
*		address instead.
*
* @return
*		- BST_SUCCESS if initialization was successful
*
* @note		None.
*
******************************************************************************/
s32  BcmGic_CfgInitialize(BcmGIC *InstancePtr,
				BcmGIC_Config *ConfigPtr,
				u32 EffectiveAddr)
{
	u32 Int_Id;
	u32 Cpu_Id = (u32)PAR_CPU_ID + (u32)1;
	(void) EffectiveAddr;

	Bcm_AssertNonvoid(InstancePtr != NULL);
	Bcm_AssertNonvoid(ConfigPtr != NULL);
	
	if(InstancePtr->IsReady != BCM_COMPONENT_IS_READY) {

		InstancePtr->IsReady = 0U;
		InstancePtr->Config = ConfigPtr;


		for (Int_Id = 0U; Int_Id<BGIC_MAX_NUM_INTR_INPUTS;Int_Id++) {
			/*
			* Initalize the handler to point to a stub to handle an
			* interrupt which has not been connected to a handler. Only
			* initialize it if the handler is 0 which means it was not
			* initialized statically by the tools/user. Set the callback
			* reference to this instance so that unhandled interrupts
			* can be tracked.
			*/
			if 	((InstancePtr->Config->HandlerTable[Int_Id].Handler == NULL)) {
				InstancePtr->Config->HandlerTable[Int_Id].Handler =
									StubHandler;
			}
			InstancePtr->Config->HandlerTable[Int_Id].CallBackRef =
								InstancePtr;
		}

		DistributorInit(InstancePtr, Cpu_Id);
		CPUInitialize(InstancePtr);

		InstancePtr->IsReady = BCM_COMPONENT_IS_READY;
	}

	return BST_SUCCESS;
}

/*****************************************************************************/
/**
*
* Makes the connection between the Int_Id of the interrupt source and the
* associated handler that is to run when the interrupt is recognized. The
* argument provided in this call as the Callbackref is used as the argument
* for the handler when it is called.
*
* @param	InstancePtr is a pointer to the BcmGIC instance.
* @param	Int_Id contains the ID of the interrupt source and should be
*		in the range of 0 to BGIC_MAX_NUM_INTR_INPUTS - 1
* @param	Handler to the handler for that interrupt.
* @param	CallBackRef is the callback reference, usually the instance
*		pointer of the connecting driver.
*
* @return
*
*		- BST_SUCCESS if the handler was connected correctly.
*
* @note
*
* WARNING: The handler provided as an argument will overwrite any handler
* that was previously connected.
*
****************************************************************************/

s32  BcmGic_Connect(BcmGIC *InstancePtr, u32 Int_Id,
                      Bcm_InterruptHandler Handler, void *CallBackRef)
{
	/*
	 * Assert the arguments
	 */
	Bcm_AssertNonvoid(InstancePtr != NULL);
	Bcm_AssertNonvoid(Int_Id < BGIC_MAX_NUM_INTR_INPUTS);
	Bcm_AssertNonvoid(Handler != NULL);
	Bcm_AssertNonvoid(InstancePtr->IsReady == BCM_COMPONENT_IS_READY);

	/*
	 * The Int_Id is used as an index into the table to select the proper
	 * handler
	 */
	InstancePtr->Config->HandlerTable[Int_Id].Handler = Handler;
	InstancePtr->Config->HandlerTable[Int_Id].CallBackRef = CallBackRef;

	return BST_SUCCESS;
}

/*****************************************************************************/
/**
*
* Updates the interrupt table with the Null Handler and NULL arguments at the
* location pointed at by the Int_Id. This effectively disconnects that interrupt
* source from any handler. The interrupt is disabled also.
*
* @param	InstancePtr is a pointer to the XScuGic instance to be worked on.
* @param	Int_Id contains the ID of the interrupt source and should
*		be in the range of 0 to BGIC_MAX_NUM_INTR_INPUTS - 1
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void BcmGic_Disconnect(BcmGIC *InstancePtr, u32 Int_Id)
{
	u32 Mask;

	/*
	 * Assert the arguments
	 */
	Bcm_AssertVoid(InstancePtr != NULL);
	Bcm_AssertVoid(Int_Id < BGIC_MAX_NUM_INTR_INPUTS);
	Bcm_AssertVoid(InstancePtr->IsReady == BCM_COMPONENT_IS_READY);

	/*
	 * The Int_Id is used to create the appropriate mask for the
	 * desired bit position. Int_Id currently limited to 0 - 31
	 */
	Mask = 0x00000001U << (Int_Id % 32U);

	/*
	 * Disable the interrupt such that it won't occur while disconnecting
	 * the handler, only disable the specified interrupt id without modifying
	 * the other interrupt ids
	 */
	REGS_GIC_Distributor->ICENABLER[(Int_Id / 32U) * 4U] = Mask;

	/*
	 * Disconnect the handler and connect a stub, the callback reference
	 * must be set to this instance to allow unhandled interrupts to be
	 * tracked
	 */
	InstancePtr->Config->HandlerTable[Int_Id].Handler = StubHandler;
	InstancePtr->Config->HandlerTable[Int_Id].CallBackRef = InstancePtr;
}

/*****************************************************************************/
/**
*
* Enables the interrupt source provided as the argument Int_Id. Any pending
* interrupt condition for the specified Int_Id will occur after this function is
* called.
*
* @param	InstancePtr is a pointer to the BcmGic instance.
* @param	Int_Id contains the ID of the interrupt source and should be
*		in the range of 0 to BGIC_MAX_NUM_INTR_INPUTS - 1
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void BcmGic_Enable(BcmGIC *InstancePtr, u32 Int_Id)
{
	u32 Mask;

	/*
	 * Assert the arguments
	 */
	Bcm_AssertVoid(InstancePtr != NULL);
	Bcm_AssertVoid(Int_Id < BGIC_MAX_NUM_INTR_INPUTS);
	Bcm_AssertVoid(InstancePtr->IsReady == BCM_COMPONENT_IS_READY);

	/*
	 * The Int_Id is used to create the appropriate mask for the
	 * desired bit position. Int_Id currently limited to 0 - 31
	 */
	Mask = 0x00000001U << (Int_Id % 32U);

	/*
	 * Enable the selected interrupt source by setting the
	 * corresponding bit in the Enable Set register.
	 */
	REGS_GIC_Distributor->ISENABLER[(Int_Id / 32U) * 4U] = Mask;
}

/*****************************************************************************/
/**
*
* Disables the interrupt source provided as the argument Int_Id such that the
* interrupt controller will not cause interrupts for the specified Int_Id. The
* interrupt controller will continue to hold an interrupt condition for the
* Int_Id, but will not cause an interrupt.
*
* @param	InstancePtr is a pointer to the BcmGic instance.
* @param	Int_Id contains the ID of the interrupt source and should be
*		in the range of 0 to BGIC_MAX_NUM_INTR_INPUTS - 1
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void BcmGic_Disable(BcmGIC *InstancePtr, u32 Int_Id)
{
	u32 Mask;

	/*
	 * Assert the arguments
	 */
	Bcm_AssertVoid(InstancePtr != NULL);
	Bcm_AssertVoid(Int_Id < BGIC_MAX_NUM_INTR_INPUTS);
	Bcm_AssertVoid(InstancePtr->IsReady == BCM_COMPONENT_IS_READY);

	/*
	 * The Int_Id is used to create the appropriate mask for the
	 * desired bit position. Int_Id currently limited to 0 - 31
	 */
	Mask = 0x00000001U << (Int_Id % 32U);

	/*
	 * Disable the interrupt, only disable the specified interrupt id without modifying
	 * the other interrupt ids
	 */
	REGS_GIC_Distributor->ICENABLER[(Int_Id / 32U) * 4U] = Mask;
}

/****************************************************************************/
/**
* Sets the interrupt priority and trigger type for the specificd IRQ source.
*
* @param	InstancePtr is a pointer to the instance to be worked on.
* @param	Int_Id is the IRQ source number to modify
* @param	Priority is the new priority for the IRQ source. 0 is highest
* 			priority, 0xF8 (248) is lowest. There are 32 priority levels
*			supported with a step of 8. Hence the supported priorities are
*			0, 8, 16, 32, 40 ..., 248.
* @param	Trigger is the new trigger type for the IRQ source.
* Each bit pair describes the configuration for an INT_ID.
* SFI    Read Only    b10 always
* PPI    Read Only    depending on how the PPIs are configured.
*                    b01    Active HIGH level sensitive
*                    b11 Rising edge sensitive
* SPI                LSB is read only.
*                    b01    Active HIGH level sensitive
*                    b11 Rising edge sensitive/
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void BcmGic_SetPriorityTriggerType(BcmGIC *InstancePtr, u32 Int_Id,
					u8 Priority, u8 Trigger)
{
	reg32 RegValue;
	u8 LocalPriority;
	LocalPriority = Priority;

	Bcm_AssertVoid(InstancePtr != NULL);
	Bcm_AssertVoid(InstancePtr->IsReady == BCM_COMPONENT_IS_READY);
	Bcm_AssertVoid(Int_Id < BGIC_MAX_NUM_INTR_INPUTS);
	Bcm_AssertVoid(Trigger < (u8)BGIC_INTR_CFG_MASK);
	Bcm_AssertVoid(LocalPriority < (u8)BGIC_MAX_INTR_PRIO_VAL);

	/*
	 * Determine the register to write to using the Int_Id.
	 */
	RegValue = REGS_GIC_Distributor->IPRIORITYR[Int_Id / 4U];

	/*
	 * The priority bits are Bits 7 to 3 in GIC Priority Register. This
	 * means the number of priority levels supported are 32 and they are
	 * in steps of 8. The priorities can be 0, 8, 16, 32, 48, ... etc.
	 * The lower order 3 bits are masked before putting it in the register.
	 */
	LocalPriority = LocalPriority & (u8)BGIC_INTR_PRIO_MASK;
	/*
	 * Shift and Mask the correct bits for the priority and trigger in the
	 * register
	 */
	RegValue &= ~(BGIC_PRIORITY_MASK << ((Int_Id % 4U) * 8U));
	RegValue |= (u32)LocalPriority << ((Int_Id % 4U) * 8U);

	/*
	 * Write the value back to the register.
	 */
	REGS_GIC_Distributor->IPRIORITYR[Int_Id / 4U] = RegValue;

	/*
	 * Determine the register to write to using the Int_Id.
	 */
	RegValue = REGS_GIC_Distributor->ICFGR[Int_Id / 16U];

	/*
	 * Shift and Mask the correct bits for the priority and trigger in the
	 * register
	 */
	RegValue &= ~(BGIC_INTR_CFG_MASK << ((Int_Id % 16U) * 2U));
	RegValue |= (u32)Trigger << ((Int_Id % 16U) * 2U);

	/*
	 * Write the value back to the register.
	 */
	REGS_GIC_Distributor->ICFGR[Int_Id / 16U];

}

/****************************************************************************/
/**
* Gets the interrupt priority and trigger type for the specificd IRQ source.
*
* @param	InstancePtr is a pointer to the instance to be worked on.
* @param	Int_Id is the IRQ source number to modify
* @param	Priority is a pointer to the value of the priority of the IRQ
*		source. This is a return value.
* @param	Trigger is pointer to the value of the trigger of the IRQ
*		source. This is a return value.
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void BcmGic_GetPriorityTriggerType(BcmGIC *InstancePtr, u32 Int_Id,
					u8 *Priority, u8 *Trigger)
{
	reg32 RegValue;

	Bcm_AssertVoid(InstancePtr != NULL);
	Bcm_AssertVoid(InstancePtr->IsReady == BCM_COMPONENT_IS_READY);
	Bcm_AssertVoid(Int_Id < BGIC_MAX_NUM_INTR_INPUTS);
	Bcm_AssertVoid(Priority != NULL);
	Bcm_AssertVoid(Trigger != NULL);

	/*
	 * Determine the register to read to using the Int_Id.
	 */
	RegValue = REGS_GIC_Distributor->IPRIORITYR[Int_Id / 4U];

	/*
	 * Shift and Mask the correct bits for the priority and trigger in the
	 * register
	 */
	RegValue = RegValue >> ((Int_Id%4U)*8U);
	*Priority = (u8)(RegValue & BGIC_PRIORITY_MASK);

	/*
	 * Determine the register to read to using the Int_Id.
	 */
	RegValue = REGS_GIC_Distributor->ICFGR[Int_Id / 16U];

	/*
	 * Shift and Mask the correct bits for the priority and trigger in the
	 * register
	 */
	RegValue = RegValue >> ((Int_Id%16U)*2U);

	*Trigger = (u8)(RegValue & BGIC_INTR_CFG_MASK);
}
/****************************************************************************/
/**
* Sets the target CPU for the interrupt of a peripheral
*
* @param	InstancePtr is a pointer to the instance to be worked on.
* @param	Cpu_Id is a CPU number for which the interrupt has to be targeted
* @param	Int_Id is the IRQ source number to modify
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void BcmGic_InterruptMaptoCpu(BcmGIC *InstancePtr, u8 Cpu_Id, u32 Int_Id)
{
	reg32 RegValue, Offset;

	RegValue = REGS_GIC_Distributor->ITARGETSR[Int_Id / 4U];

	Offset = (Int_Id & 0x3U);
	Cpu_Id = (0x1U << Cpu_Id);

	RegValue = (RegValue & (~(0xFFU << (Offset*8U))) );
	RegValue |= ((Cpu_Id) << (Offset*8U));

	REGS_GIC_Distributor->ITARGETSR[Int_Id / 4U] = RegValue;
	
	/* to avoid warning unused parameters */
	( void ) InstancePtr;
}

void show_invalid_entry_message(u32 type, u64 esr, u64 address) {
    printf("ERROR CAUGHT: %s - %d, ESR: %X, Address: %X\n", 
        entry_error_messages[type], type, esr, address);
}

// test purpose only
void enable_interrupt_controller() {
    REGS_IRQ->irq0_enable_0 = AUX_IRQ;
}

void handle_irq() {
    u32 irq;

    irq = REGS_IRQ->irq0_pending_0;

    while(irq) {
        if (irq & AUX_IRQ) {
            irq &= ~AUX_IRQ;

			// Does receiver holds a valid byte (register mu_iir)
            while((REGS_AUX->mu_iir & 4) == 4) {
                printf("UART Recv: ");
                uart_send(uart_recv());
                printf("\n");
            }
        }
/*
        if (irq & SYS_TIMER_IRQ_1) {
            irq &= ~SYS_TIMER_IRQ_1;

            handle_timer_1();
        }

        if (irq & SYS_TIMER_IRQ_3) {
            irq &= ~SYS_TIMER_IRQ_3;

            handle_timer_3();
        }*/
    
	}

}

#if 0   //Timer irq from LLD
#include "printf.h"
#include "mini_uart.h"
#include "timer.h"
#include "bstatus.h"





void enable_interrupt_controller() {
        REGS_IRQ->irq0_enable_0 = AUX_IRQ | SYS_TIMER_IRQ_1 | SYS_TIMER_IRQ_3;
}

#endif
/*
unsigned long get_el(void) {
    unsigned long el;
    
    // Inline assembly to read CurrentEL and shift to get the EL level
    __asm__ volatile(
        "mrs %0, CurrentEL\n"    // Read the CurrentEL register into %0 (el)
        "lsr %0, %0, #2\n"       // Logical shift right by 2 to get the EL level
        : "=r" (el)              // Output operand: store result in 'el'
        :                        // No input operands
        : "x0"                   // Clobber list: 'x0' is modified
    );

    return el;
}
*/