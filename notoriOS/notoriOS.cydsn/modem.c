#include <assert.h>
#include <stdint.h>
#include "notorios.h"
#include "project.h"
#include "re.h"

/* === Modem UART Functions  */

static char uart_chars[257] = {0};
static uint8_t num_chars = 0;

static char *line = uart_chars;
static int match = -1;
const char *pattern = NULL;

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

/* === Modem Power === */

void modem_start(void) {
  assert(Modem_PWR_Read() == 0);
  Modem_UART_Start();
  Modem_ISR_StartEx(Modem_ISR);
  Modem_PWR_Write(1);
  wait_for("^SYSTART");
}

void modem_stop(void) {
  Modem_PWR_Write(0);
  Modem_ISR_Stop();
  Modem_UART_Stop();
}