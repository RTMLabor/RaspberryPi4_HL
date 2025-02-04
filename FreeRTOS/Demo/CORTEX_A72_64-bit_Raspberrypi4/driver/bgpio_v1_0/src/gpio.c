#include "gpio.h"
#include "bparameters.h"

enum {
    GPFSEL0         = BCM_RPI_PERIPHERAL_BASEADDR + 0x200000,
    GPSET0          = BCM_RPI_PERIPHERAL_BASEADDR + 0x20001C,
    GPCLR0          = BCM_RPI_PERIPHERAL_BASEADDR + 0x200028,
    GPPUPPDN0       = BCM_RPI_PERIPHERAL_BASEADDR + 0x2000E4,
    GPIO_MAX_PIN       = 53
};

/******************************************************************/
/**
 * @brief Writes a value to a memory-mapped I/O (MMIO) register.
 *
 * This function writes a 32-bit unsigned integer value to a specified
 * memory-mapped I/O address. The address should correspond to a device
 * register for hardware peripherals, allowing the program to configure
 * or control that device.
 *
 * @param reg The address of the memory-mapped I/O register to which
 *            the value should be written. This should be a valid MMIO
 *            address corresponding to the peripheral.
 * @param val The 32-bit unsigned integer value to be written to the
 *            specified memory-mapped I/O register. This value may be
 *            used to set control bits or to send commands to the device.
 *
 * @note This function uses volatile pointers to ensure that the compiler
 *       does not optimize away the write operation, which is crucial
 *       for correctly interfacing with hardware registers. The function
 *       does not return a value, and it is assumed that the caller will
 *       handle any necessary validation or error checking for the
 *       write operation.
 */
void mmio_write(long reg, unsigned int val) { *(volatile unsigned int *)reg = val; }
/******************************************************************/
/**
 * @brief Reads a value from a memory-mapped I/O (MMIO) register.
 *
 * This function reads a 32-bit unsigned integer value from a specified
 * memory-mapped I/O address. The address should correspond to a device
 * register for hardware peripherals, allowing the program to retrieve
 * the current state or data from that device.
 *
 * @param reg The address of the memory-mapped I/O register from which
 *            to read the value. This should be a valid MMIO address
 *            corresponding to the peripheral.
 * 
 * @return The 32-bit unsigned integer value read from the specified
 *         memory-mapped I/O register. The value represents the state or
 *         configuration of the device at that address.
 *
 * @note This function uses volatile pointers to ensure that the compiler
 *       does not optimize away the read operation, which is crucial
 *       for correctly interfacing with hardware registers. The returned
 *       value should be treated as a hardware state and not optimized
 *       or cached by the compiler.
 */
unsigned int mmio_read(long reg) { return *(volatile unsigned int *)reg; }
/******************************************************************/
/**
 * @brief Sets a specific field in a GPIO register for a given pin.
 *
 * This function updates a specified field in a GPIO register associated with 
 * a pin, allowing for customized control over the pin's functionality. 
 * It performs bounds checking on the `pin_number` and `value` to ensure they 
 * do not exceed the specified limits. The function calculates the appropriate 
 * register address based on the provided base address and field size.
 *
 * @param pin_number The GPIO pin number to modify
 * @param value The value to set for the specified pin's field
 * @param base The base address of the GPIO register to modify.
 * @param field_size The size of the field in bits
 *
 * @return BST_SUCCESS if the operation is successful; otherwise, 
 *         BST_FAILURE to indicate an error (e.g., invalid pin_number or value).
 */
unsigned int gpio_call(unsigned int pin_number, unsigned int value, unsigned int base, unsigned int field_size) {
    unsigned int field_mask = (1 << field_size) - 1;
  
    if (pin_number > GPIO_MAX_PIN) return BST_FAILURE;
    if (value > field_mask) return BST_FAILURE; 

    unsigned int num_fields = 32 / field_size;
    unsigned int reg = base + ((pin_number / num_fields) * 4);
    unsigned int shift = (pin_number % num_fields) * field_size;

    unsigned int curval = mmio_read(reg);
    curval &= ~(field_mask << shift);
    curval |= value << shift;
    mmio_write(reg, curval);

    return BST_SUCCESS;
}
/******************************************************************/
/**
 * @brief Sets a specified GPIO pin to a high state.
 *
 * This function sets the state of a given GPIO pin to high (1). 
 * It calls the `gpio_call` function with the appropriate register 
 * address for setting the pin.
 *
 * @param pin_number The GPIO pin number to set (0-57).
 * @param value The value to set for the pin (should be 1 to set the pin high).
 *
 * @return Returns the status of the operation; typically BST_SUCCESS if successful.
 */
u32 gpio_set     (u8 pin_number, u8 value) {
     return gpio_call(pin_number, value, GPSET0, 1); 
}
/******************************************************************/
/**
 * @brief Clears a specified GPIO pin to a low state.
 *
 * This function clears the state of a given GPIO pin to low (0). 
 * It calls the `gpio_call` function with the appropriate register 
 * address for clearing the pin.
 *
 * @param pin_number The GPIO pin number to clear (0-31).
 * @param value The value to set for the pin (should be 1 to clear the pin).
 *
 * @return Returns the status of the operation; typically BST_SUCCESS (0) if successful.
 */
u32 gpio_clear   (u8 pin_number, u8 value) {
     return gpio_call(pin_number, value, GPCLR0, 1); 
}
/******************************************************************/
/**
 * @brief Configures the pull-up/pull-down setting for a GPIO pin.
 *
 * This function sets the pull-up or pull-down resistor for the specified 
 * GPIO pin. It calls the `gpio_call` function with the appropriate 
 * register address for pull-up/pull-down configuration.
 *
 * @param pin_number The GPIO pin number to configure (0-57).
 * @param value The pull configuration value (GRNo = No resistor, GRUP = Pull up, GRPD = Pull down, GPres = reset)
 *
 * @return Returns the status of the operation; typically BST_SUCCESS (0) if successful.
 */
u8 gpio_pull    (unsigned int pin_number, unsigned int value) {
     return gpio_call(pin_number, value, GPPUPPDN0, 2); 
}
/******************************************************************/
/**
 * @brief Configures the function of a specific GPIO pin.
 *
 * This function sets the function (such as input, output, alternate functions)
 * of a GPIO pin by adjusting the appropriate bits in the GPIO function select register.
 * The function works by determining the correct register and bit offset for the specified pin.
 *
 * @param pinNumber The GPIO pin number to configure (0-57 on RPi4).
 * @param func The desired function to set for the specified GPIO pin:
 *                 - Input,
 *                 - Output
 *                 - Alternative 0,
 *                 - Alternative 1,
 *                 - Alternative 2,
 *                 - Alternative 3,
 *                 - Alternative 4,
 *                 - Alternative 5
 * 
 * @return BST_SUCCESS (typically 0) to indicate successful configuration of the pin function.
 *         Other return codes may indicate errors, but this function always returns success.
 */
u32 gpio_pin_set_func(u8 pinNumber, GpioFunc func){

    u8 bitStart = (pinNumber * 3) % 30;
    u8 reg = pinNumber / 10;

    u32 selector = REGS_GPIO->func_select[reg];
    selector &= ~(7 << bitStart);
    selector |= (func << bitStart);

    REGS_GPIO->func_select[reg] = selector;

    return BST_SUCCESS;
}
/**
 * @brief Configures event detection for a specified GPIO pin and event type.
 *
 * This function enables the detection of specific events (e.g., falling edge, rising edge,
 * high level, or low level) on a given GPIO pin by updating the corresponding event detection
 * enable registers. The event type is specified as an input parameter, and the function 
 * updates the appropriate register to enable detection for that event.
 *
 * @param pinNumber The GPIO pin number to configure (0-57 for Raspberry Pi GPIO).
 * @param event The event type to detect, specified as a value of the GpioEvent enum
 *              (e.g., Ev_falling, Ev_rising, Ev_high, Ev_low, Ev_async_rising, Ev_async_falling).
 *
 * @return BST_SUCCESS if the event detection was successfully configured, or 
 *         BST_FAILURE if an invalid event type was specified.
 */
u32 gpio_pin_set_ev_detection(u8 pinNumber, GpioEvent event){

    u8 bitStart = pinNumber % 32;
    u8 reg = pinNumber / 32;

    // Enable the event detection for the specific pin
    switch (event) {
        case Ev_falling:
            REGS_GPIO->fe_detect_enable.data[reg] |= (1U << bitStart);
            break;
        case Ev_rising:
            REGS_GPIO->re_detect_enable.data[reg] |= (1U << bitStart);
            break;
        case Ev_high:
            REGS_GPIO->hi_detect_enable.data[reg] |= (1U << bitStart);
            break;
        case Ev_low:
            REGS_GPIO->lo_detect_enable.data[reg] |= (1U << bitStart);
            break;
        case Ev_async_rising:
            REGS_GPIO->async_re_detect.data[reg] |= (1U << bitStart);
            break;
        case Ev_async_falling:
            REGS_GPIO->async_fe_detect.data[reg] |= (1U << bitStart);
            break;
        default:
            return BST_FAILURE; // Invalid event type
    }

    return BST_SUCCESS;
}
/**
 * @brief Clears the event detection status for a specific GPIO pin.
 *
 * This function checks the event detection status of a specified GPIO pin. If an event 
 * has been detected on the pin, the function resets the event detection for that pin. 
 * It clears the relevant bit in the event detection status register for the pin.
 *
 * @param pinNumber The GPIO pin number (0-57) for which the event detection status should be cleared.
 * 
 * @return Returns `BST_SUCCESS` if the event detection status was cleared successfully, 
 *         or `BST_FAILURE` if no event had been detected on the pin.
 */

u32 gpio_pin_clear_ev_detection(u8 pinNumber){

    u8 bitStart = pinNumber % 32;
    u8 reg = pinNumber / 32;

    if (REGS_GPIO->ev_detect_status.data[reg] == 1)
    {
        // reset event detection for the specific pin
        REGS_GPIO->ev_detect_status.data[reg] |= (0U << bitStart);
        return BST_SUCCESS;

    }else
    {
        return BST_FAILURE;
    }
       
}

/******************************************************************/
/**
 * @brief Enables a specified GPIO pin by configuring its pull-up/down settings.
 *
 * This function enables a GPIO pin by manipulating the pull-up/pull-down 
 * settings in the GPIO control registers. It temporarily enables the pull-up/down 
 * feature for the specified pin, waits for a brief period, then disables it, 
 * allowing the pin to be configured properly.
 *
 * @param pinNumber The GPIO pin number to enable
 * 
 * @return BST_SUCCESS to indicate that the pin has been successfully 
 *         enabled. The function does not indicate specific errors.
 */
u32 gpio_pin_enable(u8 pinNumber){

    REGS_GPIO->pupd_enable = 0;
    delay(150);
    REGS_GPIO->pupd_enable_clocks[pinNumber / 32] = 1 << (pinNumber % 32);
    delay(150);
    REGS_GPIO->pupd_enable = 0;
    REGS_GPIO->pupd_enable_clocks[pinNumber / 32] = 0;

    return BST_SUCCESS;
}
/******************************************************************/
/**
 * @brief Initializes a GPIO pin as an output with no pull-up or pull-down resistor.
 *
 * This function configures a specified GPIO pin to operate as a digital output
 * without any internal pull resistors. It sets the pull configuration to none 
 * and sets the pin function to output.
 *
 * @param pinNumber The GPIO pin number to initialize
 * 
 * @return BST_SUCCESS to indicate that the pin has been successfully
 *         initialized as an output with no pull resistors. 
 *         The function does not indicate specific errors.
 */
u32 gpio_initOutputPinWithPullNone(u8 pinNumber){

    gpio_pull(pinNumber, GRNo);
    gpio_pin_set_func(pinNumber, GFOutput);
    return BST_SUCCESS;
}
/******************************************************************/
/**
 * @brief Sets the output state of a specified GPIO pin.
 *
 * This function controls the output state of a GPIO pin, turning it either 
 * high (on) or low (off) based on the provided boolean value. 
 * If `onOrOff` is non-zero, the pin is set high; otherwise, it is set low.
 *
 * @param pinNumber The GPIO pin number to control
 * @param onOrOff A boolean value indicating the desired output state:
 *                    - Non-zero value (true) to set the pin high (on).
 *                    - Zero value (false) to set the pin low (off).
 * 
 * @return BST_SUCCESS to indicate that the operation was successful.
 *         The function does not indicate specific errors.
 */
u32 gpio_setPinOutputBool(u8 pinNumber, u8 onOrOff){

    if (onOrOff) {
        gpio_set(pinNumber, 1);
    } else {
        gpio_clear(pinNumber, 1);
    }
    return BST_SUCCESS;
}
/******************************************************************/

bool gpio_getPinLevel(u8 pinNumber){

    u32 bitMask = 1 << (pinNumber % 32);
    u8 reg = pinNumber / 32;

    return (bitMask & REGS_GPIO->level.data[reg]) != 0;


}