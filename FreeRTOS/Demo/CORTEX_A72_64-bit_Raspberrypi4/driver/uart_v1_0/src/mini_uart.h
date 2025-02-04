#ifndef __MINIUART_H
#define __MINIUART_H

#ifdef __cplusplus
extern "C" {
#endif

#define UART_PRIORITY (0xA0)

/* PL011 UART on Raspberry pi 4B */
#define UART_BASE  (0xFE201400U) /* UART2 */
#define UART_DR   (*(volatile unsigned int *)(UART_BASE))
#define UART_FR   (*(volatile unsigned int *)(UART_BASE+0x18U))
#define UART_IBRD (*(volatile unsigned int *)(UART_BASE+0x24U))
#define UART_FBRD (*(volatile unsigned int *)(UART_BASE+0x28U))
#define UART_LCRH (*(volatile unsigned int *)(UART_BASE+0x2CU))
#define UART_CR   (*(volatile unsigned int *)(UART_BASE+0x30U))
#define UART_IFLS (*(volatile unsigned int *)(UART_BASE+0x34U))
#define UART_IMSC (*(volatile unsigned int *)(UART_BASE+0x38U))
#define UART_ICR  (*(volatile unsigned int *)(UART_BASE+0x44U))


void uart_init();
char uart_recv();
void uart_send(char c);
void uart_send_string(char *str);
void mini_uart_putc(void *p, char c);
void uart_isr( void );
void uart_isr_wrapper(void *param);

#ifdef __cplusplus
}
#endif

#endif //__MINIUART_H