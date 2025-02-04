#include "bparameters.h"
#include "mini_uart.h"

#ifdef __cplusplus
extern "C" {
#endif
void outbyte(char c); 

#ifdef __cplusplus
}
#endif 

void outbyte(char c) {
	 uart_send(c);
}
