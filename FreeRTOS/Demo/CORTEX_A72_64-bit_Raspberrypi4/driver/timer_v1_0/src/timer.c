
#include "timer.h"
#include "printf.h"
#include "bparameters.h"
#include "FreeRTOSConfig.h"




#if 0   //Timer irq from LLD

void handle_timer_1() {
    cur_val_1 += interval_1;
    REGS_TIMER->compare[1] = cur_val_1;
    REGS_TIMER->control_status |= SYS_TIMER_IRQ_1;

    //printf("Timer 1 received.\n");
}

void handle_timer_3() {
    cur_val_3 += interval_3;
    REGS_TIMER->compare[3] = cur_val_3;
    REGS_TIMER->control_status |= SYS_TIMER_IRQ_3;

    //printf("Timer 3 received.\n");
}
#endif 
const u32 interval_1 = configCPU_CLOCK_HZ;
u32 cur_val_1 = 0;

const u32 interval_3 = configCPU_CLOCK_HZ / 4;
u32 cur_val_3 = 0;

void timer_init() {
    cur_val_1 = REGS_TIMER->counter_lo;
    cur_val_1 += interval_1;
    REGS_TIMER->compare[1] = cur_val_1;

    cur_val_3 = REGS_TIMER->counter_lo;
    cur_val_3 += interval_3;
    REGS_TIMER->compare[3] = cur_val_3;
}

u64 timer_get_ticks() {
    u32 hi = REGS_TIMER->counter_hi;
    u32 lo = REGS_TIMER->counter_lo;

    //double check hi value didn't change after setting it...
    if (hi != REGS_TIMER->counter_hi) {
        hi = REGS_TIMER->counter_hi;
        lo = REGS_TIMER->counter_lo;
    }

    return ((u64)hi << 32) | lo;
}


//sleep in milliseconds.
void timer_sleep(u32 ms) {
    u64 start = timer_get_ticks();

    while(timer_get_ticks() < start + (ms * 1000)) {

    }
}


u32 HAL_GetTick(void) {
    unsigned int hi = REGS_TIMER->counter_hi;
    unsigned int lo = REGS_TIMER->counter_lo;

    //double check hi value didn't change after setting it...
    if (hi != REGS_TIMER->counter_hi) {
        hi = REGS_TIMER->counter_hi;
        lo = REGS_TIMER->counter_lo;
    }

    return ((unsigned long)hi << 32) | lo;
}

void HAL_Delay(unsigned int ms) {
    unsigned long start = HAL_GetTick();

    while(HAL_GetTick() < start + (ms * 1000));
}