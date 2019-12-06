#include "uart.h"
#include <assert.h>
#include <stdint.h>
#include "notorios.h"
#include "project.h"
#include "re.h"

#define MASTER_CLOCK_FREQ 24000000  // 24 MHz Clock

/* Circular buffer of recently received chars */
static char uart_chars[257] = {0};
static uint8_t num_chars = 0;

static char *line = uart_chars;
static int match = -1;
const char *pattern = NULL;

static void uart_set_baud(uint32_t baud) {
  uint32_t clock = 8 * baud;
  uint16_t divider = MASTER_CLOCK_FREQ / clock + 1;
  Sensors_Clock_SetDividerValue(divider);
}

void uart_start(uint8_t idx, uint32_t baud) {
  assert(Sensors_PWR_Read() == 0);
  Sensors_Mux_Write(idx);
  uart_set_baud(baud);

  Sensors_UART_Start();
  Sensors_ISR_StartEx(Sensors_ISR);
  Sensors_PWR_Write(1);
}

void uart_stop(void) {
  Sensors_PWR_Write(0);
  Sensors_ISR_Stop();
  Sensors_UART_Stop();
}

const char *uart_string(void) {
  return (char*)uart_chars;
}

uint8_t uart_len(void) {
  return num_chars;
}

void uart_clear(void) {
  Sensors_UART_ClearRxBuffer();
  memset((void*)uart_chars, 0, 256);
  num_chars = 0;
}

void wait_for(const char *regex, uint32_t timeout) {
  pattern = regex, match = -1;
  uint32_t end = time() + timeout;
  while (match == -1 && time() < end);
}

CY_ISR(Sensors_ISR) {
  /* Store received char in uart_buf */
  char rx_char = Sensors_UART_GetChar();
  if (rx_char) uart_chars[num_chars++] = rx_char;

  /* Check regex at end of line */
  if (rx_char == '\r') {
    match = re_match(pattern, line);
    line = uart_chars + num_chars;
  }
}