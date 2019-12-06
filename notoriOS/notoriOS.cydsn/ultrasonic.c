#include "notorios.h"
#include "uart.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

/* === Driver Functions === */

#define N_READINGS 1
#define TAKE_AVERAGE true

static float parse(const char *string, const config_t *config) {
  const char *depth_str = strstr(string, "TempI\rR") + 7;
  float depth = strtof(depth_str, NULL);
  bool valid = !(
    (config->model == ULTRASONIC_MB7384 && !strcmp(depth_str, "5000")) ||
    (config->model == ULTRASONIC_MB7383 && !strcmp(depth_str, "9999")));
  uart_clear();
  return valid ? depth : NAN;
}

float take_reading(const config_t *config) {
  /* Run sensor for 1 second */
  uart_start(config->mux, 9600);
  wait_for("R\d\d\d\d");
  uart_stop();

  /* Return parsed reading */
  return parse(uart_string(), config);
}

/* === Main Loop === */

void ultrasonic(const config_t *config) {
  while (true) {
    float average = 0;
    uint8_t n = N_READINGS;
    for (uint8_t i = 0; i < n; ++i) {
      float reading = take_reading(config);
      if (isnan(reading)) continue;
      average += reading;
      if (!TAKE_AVERAGE) n = 1;
    }
    average /= n;

    msg_t msg_out = { .reading = {
      .value = average, .time = time(), .unit = UNIT_MM
    } };
    memcpy(msg_out.reading.name, config->name, sizeof(config->name));
    send(READINGS_PORT, &msg_out);
    sleep(config->interval);
  }
}