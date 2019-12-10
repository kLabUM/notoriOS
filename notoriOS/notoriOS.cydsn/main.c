#include <stdbool.h>
#include <stdio.h>
#include "notorios.h"
#include "uart.h"
#include "project.h"

/* === Sensor Configuration === */

void run_ultrasonic(sensor_t *config);
void run_modem(modem_t *config);

// Initalize instance variables

static sensor_t sensors[] = {
  {
    .main = run_ultrasonic,
    .interval = 1 * 15,
    .name = "Ultrasonic 1",
    .model = ULTRASONIC_MB7384,
    .mux = 0, .port = 0
  }, {
    .main = run_ultrasonic,
    .interval = 1 * 15,
    .name = "Ultrasonic 2",
    .model = ULTRASONIC_MB7383,
    .mux = 1, .port = 0
  }
};

static modem_t modem = {
  .main = run_modem, .port = 0,
  .interval = 1 * 15,
  .node_id = "invalid_node",
  .apn = "VZWINTERNET",
  .host = "http://167.99.145.246:27317",
  .auth = "dXN1cjpFV1m+PS1MnXTz1tSUkA="
};

/* === Debugging === */

int _write(int file, char *buf, int len) {
  if (file == 1) {
    // for (int i = 0; i < len; ++i)
    //   Debug_UART_PutChar(*buf++);
    return len;
  } else if (file == MODEM_STREAM) {
    for (int i = 0; i < len; ++i)
      Modem_UART_PutChar(*buf++);
    return len;
  }
  return 0;
}

void logger(void) {
  // Debug_Start();
  while (true) {
    msg_t *msg_in;
    while (!(msg_in = recv(0))) yield();
    reading_t *reading = &msg_in->reading;
    printf("[%s at %d] depth: %d\r\n", reading->label,
      (int)reading->time, (int)reading->value);
  }
}

int main(void) {
  RTC_Start();
  CyGlobalIntEnable;
  // eventually runs out of heap space
  // so we're leaking memory somewhere

  spawn(0, modem.main, &modem);
  for (uint8_t i = 0; i < 2; ++i)
    spawn(i + 1, sensors[i].main, &sensors[i]);
  start_scheduler();
}
