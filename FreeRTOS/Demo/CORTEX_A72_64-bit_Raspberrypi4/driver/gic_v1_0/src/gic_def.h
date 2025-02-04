#ifndef __GIC_DEF_H
#define __GIC_DEF_H


//D1.10.2
#define SYNC_INVALID_EL1t		0 
#define IRQ_INVALID_EL1t		1 
#define FIQ_INVALID_EL1t		2 
#define ERROR_INVALID_EL1t		3 

#define SYNC_INVALID_EL1h		4 
#define IRQ_INVALID_EL1h		5 
#define FIQ_INVALID_EL1h		6 
#define ERROR_INVALID_EL1h		7 

#define SYNC_INVALID_EL0_64	    8 
#define IRQ_INVALID_EL0_64	    9 
#define FIQ_INVALID_EL0_64		10 
#define ERROR_INVALID_EL0_64	11 

#define SYNC_INVALID_EL0_32		12 
#define IRQ_INVALID_EL0_32		13 
#define FIQ_INVALID_EL0_32		14 
#define ERROR_INVALID_EL0_32	15 

//stack frame size
#define S_FRAME_SIZE (16 * 32 + 2 * 16 + 2 * 16 + 32 * 16 + 2 * 8) 
// 32 general-purpose registers (16 bytes each) 
// + 2 for SPSR and ELR 
// + 2 for critical nesting and FPU indicator
// + 32 128-bit FPU registers (16 bytes each)
// + 16 floating-point control register


#endif //__GIC_DEF_H