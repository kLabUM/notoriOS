#include "notorios.h"

/* === Sensor Configuration === */

void ultrasonic(const config_t *config);

#define N_SENSORS 2
static config_t sensors[] = {
  {
    .name = "Ultrasonic 1",
    .main = ultrasonic,
    .model = ULTRASONIC_MB7374,
    .pwr = 2, .mux = 0,
    .interval = 1 * 60
  }, {
    .name = "Ultrasonic 2",
    .main = ultrasonic,
    .model = ULTRASONIC_MB7383,
    .pwr = 3, .mux = 1,
    .interval = 1 * 60
  }
}

/* === Debugging === */

int _write(int file, const char *buf, int len) {
  for (int i = 0; i < len; ++i)
    Debug_PutChar(*buf++);
  return len;
}

void logger(void) {
  Debug_Start();
  while (true) {
    msg_t *msg_in;
    while (!(msg_in = recv(READINGS_PORT))) sleep(1);
    reading_t *reading = &msg_in->reading;
    printf("[%s] %f", reading->name, reading->value);
  }
}

int main(void) {
  RTC_Start();
  CyGlobalIntEnable;

  spawn(0, logger, NULL);
  for (uint8_t i = 1; i < N_SENSORS + 1; ++i)
    spawn(1, sensors[i].main, sensors[i]);
  start_scheduler();
}
