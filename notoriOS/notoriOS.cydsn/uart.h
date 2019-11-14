#ifndef UART_H
#define UART_H

void uart_start(uint8_t idx, uint32_t baud);
void uart_stop(void);
const char *uart_string(void);
uint8_t uart_len(void);
void uart_clear(void);
CY_ISR_PROTO(Sensors_ISR_Handler);

#endif
