#ifndef NOTORIOS_H
#define NOTORIOS_H

/* === Driver Interface === */

#define READING_PORT 0 
#define UNIT_MM 0
#define ULTRASONIC_MB7384 0
#define ULTRASONIC_MB7383 1

typedef struct { 
  char name[32];        /* Name of sensor in messages */
  void (*main)(void*);  /* Sensor driver function */
  uint8_t model;        /* Specific sensor model (for generic drivers) */
  uint8_t pwr, mux;     /* Sensor power and mux pin number */
  uint32_t interval;    /* Sleep interval in seconds */
} config_t;

typedef struct {
  char name[32];  /* Name of sending sensor */
  float value;    /* Value read from sensor */
  uint32_t time;  /* Timestamp of reading */
  uint8_t unit;   /* Unit of reading (e.g. UNIT_MM) */
} reading_t;

/* === Task Scheduling === */

void block(void);
void yield(void);
void terminate(void);
void awake(uint8_t pid);
void spawn(uint8_t pid, void (*main)(void*), void *param);
void start_scheduler(void);

/* === Message Passing === */

typedef struct msg_t {
  uint8_t sender;
  union {
    uint8_t data[31];
    reading_t reading;
  }
} msg_t;

void send(uint8_t pord, msg_t *msg_in);
msg_t *recv(uint8_t port);

/* === Timing === */

void sleep(uint32_t ticks);
void ex_sleep(uint32_t ticks);
uint32_t time(void);
void set_time(uint32_t time);

#endif
