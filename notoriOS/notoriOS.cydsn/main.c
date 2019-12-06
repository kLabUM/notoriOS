#include <stdbool.h>
#include <stdio.h>
#include "notorios.h"
#include "uart.h"
#include "project.h"

/* === Sensor Configuration === */

void ultrasonic(const config_t *config);

static config_t sensors[] = {
  {
    .name = "Ultrasonic 1",
    .main = ultrasonic,
    .model = ULTRASONIC_MB7384,
    .mux = 0,
    .interval = 1 * 15
  }, {
    .name = "Ultrasonic 2",
    .main = ultrasonic,
    .model = ULTRASONIC_MB7383,
    .mux = 1,
    .interval = 1 * 15
  }
};

/* === Debugging === */

int _write(int file, char *buf, int len) {
  if (file != 1) return 0;
  for (int i = 0; i < len; ++i)
    Debug_PutChar(*buf++);
  return len;
}

void logger(void) {
  Debug_Start();
  const static char *units[] = { "mm" };
  while (true) {
    msg_t *msg_in;
    while (!(msg_in = recv(READINGS_PORT))) yield();
    reading_t *reading = &msg_in->reading;
    printf("[%s at %d] depth: %d%s\r\n", reading->name,
      (int)reading->time, (int)reading->value, units[reading->unit]);
  }
}

int main(void) {
  RTC_Start();
  CyGlobalIntEnable;
  // eventually runs out of heap space
  // so we're leaking memory somewhere

  spawn(0, logger, NULL);
  for (uint8_t i = 0; i < 2; ++i)
    spawn(i + 1, sensors[i].main, &sensors[i]);
  start_scheduler();
}
