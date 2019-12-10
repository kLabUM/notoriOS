#include "notorios.h"
#include "uart.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

/* === Driver Functions === */

#define N_READINGS 1
#define TAKE_AVERAGE true

static float parse(const char *string, const sensor_t *config) {
  const char *depth_str = strstr(string, "TempI\rR") + 7;
  float depth = strtof(depth_str, NULL);
  bool valid = !(
    (config->model == ULTRASONIC_MB7384 && !strcmp(depth_str, "5000")) ||
    (config->model == ULTRASONIC_MB7383 && !strcmp(depth_str, "9999")));
  uart_clear();
  return valid ? depth : NAN;
}

float take_reading(const sensor_t *config) {
  /* Run sensor for 1 second */
  uart_start(config->mux, 9600);
  wait_for("TempI\r"), wait_for("R");
  uart_stop();

  /* Return parsed reading */
  return parse(uart_string(), config);
}

/* === Main Loop === */

void ultrasonic(const sensor_t *config) {
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
      .value = average, .time = time(), 
      .label = config->label,
      .sensor = config->model,
    } };
    send(config->port, &msg_out);
    sleep(config->interval);
  }
}
