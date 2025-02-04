#ifndef __GPIO_H
#define __GPIO_H

#include "bcm_types.h"
#include "cpu_cortexa72.h"
#include "bstatus.h"

void delay(u64 ticks);

struct GpioPinData {
    reg32 reserved;
    reg32 data[2];
};

struct GpioRegs {
    reg32 func_select[6];
    struct GpioPinData output_set;
    struct GpioPinData output_clear;
    struct GpioPinData level;
    struct GpioPinData ev_detect_status;
    struct GpioPinData re_detect_enable;
    struct GpioPinData fe_detect_enable;
    struct GpioPinData hi_detect_enable;
    struct GpioPinData lo_detect_enable;
    struct GpioPinData async_re_detect;
    struct GpioPinData async_fe_detect;
    reg32 reserved;
    reg32 pupd_enable;
    reg32 pupd_enable_clocks[2];
};

#define REGS_GPIO ((struct GpioRegs *)(BCM_RPI_PERIPHERAL_BASEADDR + 0x00200000))


//GPFSEL Register
typedef enum _GpioFunc {
    GFInput = 0,
    GFOutput = 1,
    GFAlt0 = 4,
    GFAlt1 = 5,
    GFAlt2 = 6,
    GFAlt3 = 7,
    GFAlt4 = 3,
    GFAlt5 = 2
} GpioFunc;

//GPFSEL Register
typedef enum _GpioEvent {
    Ev_falling = 0,
    Ev_rising = 1,
    Ev_high = 4,
    Ev_low = 5,
    Ev_async_rising = 6,
    Ev_async_falling = 7
} GpioEvent;

//GPIO_PUP_PDN_CNTRL_REG Register
typedef enum _GpioPUPPDNCNTRL {
    GRNo = 0,   // 00 = No resistor is selected
    GRUP = 1,   // 01 = Pull up resistor is selected
    GRPD = 2,   // 10 = Pull down resistor is selected
    GPres = 3   // 11 = Reserved (reset)
} GpioPUPPDNCNTRL;


u32 gpio_pin_set_func(u8 pinNumber, GpioFunc func);

u32 gpio_pin_set_ev_detection(u8 pinNumber, GpioEvent event);
u32 gpio_pin_clear_ev_detection(u8 pinNumber);

u32 gpio_pin_enable(u8 pinNumber);
u32 gpio_set(u8 pinNumber, u8 value);
u32 gpio_clear(u8 pinNumber, u8 value);
u32 gpio_setPinOutputBool(u8 pinNumber, u8 onOrOff);
u32 gpio_initOutputPinWithPullNone(u8 pinNumber);

#endif //GPIO_H