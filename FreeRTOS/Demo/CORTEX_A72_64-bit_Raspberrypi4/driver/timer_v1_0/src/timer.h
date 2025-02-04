#ifndef __TIMER_H
#define __TIMER_H

#include "bcm_types.h"
#include "cpu_cortexa72.h"


//10.2
struct timer_regs {
    reg32 control_status;
    reg32 counter_lo;
    reg32 counter_hi;
    reg32 compare[4];
};

#define REGS_TIMER ((struct timer_regs *)(BCM_RPI_PERIPHERAL_BASEADDR + 0x00003000))


u32 HAL_GetTick(void);
void HAL_Delay(unsigned int ms);


#if 0   //Timer irq from LLD
void handle_timer_1();
void handle_timer_3();
#endif
void timer_init();
void timer_sleep(u32 ms);
u64 timer_get_ticks();


#endif