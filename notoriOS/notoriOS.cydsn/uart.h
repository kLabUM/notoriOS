#ifndef UART_H
#define UART_H

#include "project.h"

void uart_start(uint8_t idx, uint32_t baud);
void uart_stop(void);
const char *uart_string(void);
uint8_t uart_len(void);
void uart_clear(void);
void wait_for(const char *regex, uint32_t timeout);
CY_ISR_PROTO(Sensors_ISR);

#endif
