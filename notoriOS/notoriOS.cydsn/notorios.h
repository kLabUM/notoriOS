#ifndef NOTORIOS_H
#define NOTORIOS_H

#include <stdint.h>

#define field_size(t, f) (sizeof(((t*)0)->f))

#define N_TASKS 3        /* Total number of possible tasks */
#define N_READY 4        /* Max ready tasks, must be power of 2 > N_TASKS */
#define STACK_SIZE 2048  /* Stack space for each task in bytes */
#define N_MESSAGES 16    /* Max buffered messages in each port */
#define N_PORTS 2        /* Number of shared message ports  */
#define N_TIMERS 16      /* Max concurrently running timers */

#define MODEM_STREAM 10
#define ULTRASONIC_MB7384 0
#define ULTRASONIC_MB7383 1

/* === Driver Interface === */

typedef struct { 
  void *main;         /* Sensor driver function */
  uint32_t interval;  /* Sleep interval in seconds */
  char name[17];      /* Name of sensor instance for reading labels */
  uint8_t model;      /* Specific sensor model (for generic drivers) */
  uint8_t mux;        /* Sensor power and mux pin number */
  uint8_t port;       /* Port to send reading messages to */
} sensor_t;

typedef struct {
  float value;        /* Value read from sensor */
  uint32_t time;      /* Timestamp of reading */
  char label[23];     /* Reading label (name + 6 chars) */
  uint8_t sensor;     /* Specific sensor model */
} reading_t;

typedef struct {
  uint32_t magic;     /* Protocol magic bytes */
  uint32_t hash;      /* Firmware version hash */
  char id[24];        /* Up to 24 digit MEID / CCID */
} header_t;

typedef struct {
  void *main;         /* Modem driver function */
  uint32_t interval;  /* Sleep interval in seconds */
  header_t header;    /* See comments above */
  char host[90];      /* Server base url (with port) */
  char apn[16];       /* Modem APN */
  char auth[48];      /* X-Api-Key value */
  uint8_t port_in;    /* Local port to get messages from */
  uint8_t port_out;   /* Local port to send messages to */
} modem_t;

/* === Task Scheduling === */

void block(void);
void yield(void);
void terminate(void);
void awake(uint8_t pid);
void spawn(uint8_t pid, void *main, void *param);
void start_scheduler(void);

/* === Message Passing === */

typedef struct msg_t {
  uint8_t sender;
  union {
    uint8_t data[31];
    reading_t reading;
  };
} msg_t;

uint8_t num_msgs(uint8_t port);
void send(uint8_t port, msg_t *msg_in);
msg_t *recv(uint8_t port);
void unack(uint8_t port, uint8_t n);

/* === Timing === */

void sleep(uint32_t ticks);
void wait_ms(uint32_t milliseconds);
uint32_t time(void);
void set_time(uint32_t time);

#endif
