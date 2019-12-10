#ifndef NOTORIOS_H
#define NOTORIOS_H

#include <stdint.h>

#define N_TASKS 3        /* Total number of possible tasks */
#define N_READY 4        /* Max ready tasks, must be power of 2 > N_TASKS */
#define STACK_SIZE 2048  /* Stack space for each task in bytes */
#define N_MESSAGES 16    /* Max buffered messages in each port */
#define N_PORTS 1        /* Number of shared message ports  */
#define N_TIMERS 16      /* Max concurrently running timers */

#define MODEM_STREAM 10
#define ULTRASONIC_MB7384 0
#define ULTRASONIC_MB7383 1

/* === Driver Interface === */

typedef struct {
  void *main;         /* Modem driver function */
  uint32_t interval;  /* Sleep interval in seconds */
  char node_id[20];   /* Up to 20 digit MEID / CCID */
  char host[48];      /* Server base url (with port) */
  char apn[16];       /* APN */
  char auth[31];      /* Base64 encoded username:password */
  uint8_t port;       /* Local port to get messages from */
} modem_t;

typedef struct { 
  void *main;         /* Sensor driver function */
  uint32_t interval;  /* Sleep interval in seconds */
  char name[17];      /* Name of sensor instance for reading labels */
  uint8_t model;      /* Specific sensor model (for generic drivers) */
  uint8_t mux;        /* Sensor power and mux pin number */
  uint8_t port;       /* Port to send reading messages to */
} sensor_t;

typedef struct {
  float value;     /* Value read from sensor */
  uint32_t time;   /* Timestamp of reading */
  char label[23];  /* Reading label (name + 6 chars) */
  uint8_t sensor;  /* Specific sensor model */
} reading_t;

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

/* === Timing === */

void sleep(uint32_t ticks);
void wait_ms(uint32_t milliseconds);
uint32_t time(void);
void set_time(uint32_t time);

#endif
