#include "uart.h"

#define MASTER_CLOCK_FREQ 24000000  // 24 MHz Clock

/* Circular buffer of recently received chars */
char uart_buf[257] = {0};
uint8_t buf_idx = 0;

static void uart_set_baud(uint32_t baud) {
  uint32_t clock = 8 * baud;
  uint16_t divider = (MASTER_CLOCK_FREQ - 1) / (clock + 1);
  Sensors_Clock_SetDividerValue(divider);
}

void uart_start(uint8_t idx, uint32_t baud) {
  Sensors_Mux_Wakeup();
  Sensors_Mux_Write(idx);
  uart_set_baud(baud);
  Sensors_UART_Start();
  Sensors_ISR_StartEx(Sensors_ISR_Handler);
}

void uart_stop(void) {
  Sensors_UART_ISR_Stop();
  Sensors_UART_Stop();
  Sensors_Mux_Sleep();
}

const char *uart_string(void) {
  return uart_chars;
}

uint8_t uart_len(void) {
  return num_chars;
}

void uart_clear(void) {
  Sensors_UART_ClearRxBuffer();
  memset(uart_chars, 0, 257);
  num_chars = 0;
}

CY_ISR(Sensors_ISR_Handler) {
  /* Store received char in uart_buf */
  char rx_char = Sensors_UART_GetChar();
  if (rx_char) uart_chars[num_chars++] = rx_char;
}
