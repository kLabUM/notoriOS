#include "notorios.h"
#include "uart.h"
#include <stdbool.h>
#include <math.h>

/* === Driver Functions === */

#define N_READINGS 10
#define TAKE_AVERAGE true

static float parse(const char *string, const config_t *config) {
  const char *depth_str = strstr(string, "TempI\rR") + 7;
  float depth = strtof(depth_str, NULL);
  bool valid = !(
    config->type == ULTRASONIC_MB7384 && !strcmp(depth_str, "5000") ||
    config->type == ULTRASONIC_MB7383 && !strcmp(depth_str, "9999"));
  return valid ? depth : NAN;
}

static float take_reading(const config_t *config) {
  /* Initialize UART */
  uart_set_mux(config->mux);
  uart_clear_string();
  uart_set_baud(9600u);
  uart_start();

  /* Run sensor for 1 second */
  CyPins_SetPin(config->pwr);
  ex_sleep(1);
  CyPins_ClearPin(config->pwr);

  /* Parse reading from sensor */
  uart_stop();
  return parse(uart_get_string());
}

/* === Main Loop === */

void ultrasonic(const config_t *config) {
  while (true) {
    float average = 0;
    uint8_t n = NUM_READINGS;
    for (uint8_t i = 0; i < n; ++i) {
      float reading = take_reading(config);
      if (isnan(reading)) continue;
      average += reading;
      if (!TAKE_AVERAGE) n = 1;
    }
    average /= n;

    msg_t msg_out = { .reading = { config->name, average, time(), UNIT_MM } };
    send(&msg_out, READINGS_PORT);
    sleep(config->interval);
  }
}
