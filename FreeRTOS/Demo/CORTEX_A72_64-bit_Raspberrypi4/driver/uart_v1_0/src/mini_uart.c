#include "gpio.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "mini_uart.h"

// Uart on GPIO 0 and GPIO1
#define TXD 0
#define RXD 1

struct UARTCTL {
    SemaphoreHandle_t tx_mux;
    QueueHandle_t     rx_queue;
};
struct UARTCTL *uartctl;


/******************************************************************/
/**
 * @brief Initializes the UART (Universal Asynchronous Receiver-Transmitter) for serial communication.
 *
 * This function sets up the UART for communication by configuring the GPIO pins
 * for transmit (TXD) and receive (RXD) functionality, enabling the UART module,
 * and configuring baud rate, control, and mode registers. After initialization,
 * it sends a newline to signal readiness.
 *
 * @note The baud rate is specifically set for 115200 baud on a system clock of 500 MHz.
 *       GPIO and AUX (Auxiliary) registers must be accessible for correct configuration.
 */
void uart_init() {
    gpio_pin_set_func(TXD, GFAlt5);
    gpio_pin_set_func(RXD, GFAlt5);

    gpio_pin_enable(TXD);
    gpio_pin_enable(RXD);

    REGS_AUX->enables = 1;
    REGS_AUX->mu_control = 0;
    REGS_AUX->mu_ier = 2;   // former 0xD
    REGS_AUX->mu_lcr = 3;
    REGS_AUX->mu_mcr = 0;

    // RPi4-specific
    REGS_AUX->mu_baud_rate = 541; // = 115200 @ 500 Mhz


    REGS_AUX->mu_control = 3;

    uart_send('\r');
    uart_send('\n');
    uart_send('\n');
    
    uart_send_string("uart initialized!\n");
}

/******************************************************************/
/**
 * @brief Sends a single character via UART.
 *
 * This function waits until the UART is ready to send data, then writes a character
 * to the UART’s data register to transmit it. This function blocks until the UART
 * is ready to transmit, ensuring data consistency.
 *
 * @param c The character to send over UART.
 */
void uart_send(char c) {
    while(!(REGS_AUX->mu_lsr & 0x20));

    REGS_AUX->mu_io = c;
}
/******************************************************************/
/**
 * @brief Receives a single character via UART.
 *
 * This function waits until a character is available in the UART’s data register,
 * then reads and returns it. It blocks until data is received, making it a
 * synchronous receive function.
 *
 * @return The character received via UART, returned as an 8-bit ASCII character.
 */
char uart_recv() {
    while(!(REGS_AUX->mu_lsr & 1));

    return REGS_AUX->mu_io & 0xFF;
}
/******************************************************************/
/**
 * @brief Sends a null-terminated string via UART.
 *
 * This function sends each character in a string over UART until reaching the null
 * terminator. It replaces newline characters ('\\n') with a carriage return and newline
 * sequence for proper display on certain terminals.
 *
 * @param str A pointer to the null-terminated string to be sent.
 */
void uart_send_string(char *str) {
    while(*str) {
        if (*str == '\n') {
            uart_send('\r');
        }

        uart_send(*str);
        str++;
    }
}
/******************************************************************/
/**
 * @brief Sends a single character to UART, with optional newline handling.
 *
 * This function is designed to output a character for a logging or debugging
 * interface, inserting a carriage return before each newline character.
 *
 * @param p A pointer to additional context (unused).
 * @param c The character to send to UART.
 */
void mini_uart_putc(void *p, char c){
    if (c == '\n') {
        uart_send('\r');
    }

    uart_send(c);
    
	/* to avoid warning unused parameters */
	( void ) p;
}
/******************************************************************/
/**
 * @brief Wrapper function to call the UART interrupt service routine (ISR).
 *
 * This function acts as a bridge to invoke the `uart_isr` function, allowing
 * it to be used as an interrupt handler. Any necessary context or parameters
 * can be passed through this wrapper.
 *
 * @param param Pointer to any parameters passed to the ISR (unused).
 */
void uart_isr_wrapper(void *param){
    uart_isr();
    
	/* to avoid warning unused parameters */
	( void ) param;
}
/******************************************************************/
/**
 * @brief UART Interrupt Service Routine (ISR) for receiving data.
 *
 * This ISR handles incoming UART data by checking if data is available
 * in the UART data register. If data is present, it reads the character,
 * and sends it to a queue for further processing by other parts of the system.
 *
 * @note This function interacts with the FreeRTOS queue `rx_queue` and is 
 *       designed to be called in an interrupt context.
 */
void uart_isr( void )
{
    /* RX data */
    if( !(UART_FR & (0x1U << 4)) ) {
        uint8_t c = (uint8_t) 0xFF & UART_DR;
        xQueueSendToBackFromISR(uartctl->rx_queue, &c, NULL);
    }
}