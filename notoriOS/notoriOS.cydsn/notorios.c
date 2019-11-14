/* ========================================
 *
 * OPEN-STORM.ORG
 * notoriOS
 * A non-premptive operating system for node.
 *
 * "To all the teachers that told me I'd never amount to nothin"
 *      - Notorious BIG (1994)
 *
 * ========================================
 */
#include <assert.h>
#include <stdbool.h>
#include "notorios.h"
#include "cyapicallbacks.h"
#include "project.h"

#define STACK_SIZE 2048 /* Stack space for each task in bytes */
#define N_TASKS 4       /* Number of possible tasks, running and blocked */
#define N_MESSAGES 16   /* Max buffered messages in each port */
#define N_PORTS 1       /* Number of shared message ports  */
#define N_TIMERS 16     /* Max concurrently running timers */

/* === Task Scheduling === */

typedef struct {
  uint32_t regs[10];
  uint8_t stack[STACK_SIZE];
  bool blocked;
} task_t;

typedef struct {
  uint8_t buffer[N_TASKS];
  uint8_t head;
  uint8_t tail;
} task_buf;

task_t tasks[N_TASKS];
task_buf ready;
uint8_t running;

static void check_timers(void);

typedef void func_t(uint32_t[10], uint32_t[10]);
const uint16_t switch_task_asm[] = {
    0xf841, 0xdb04, /* str sp, [r1], #4       */
    0xe8a1, 0x4ff0, /* stmia r1!, {r4-r11,lr} */
    0xf850, 0xdb04, /* ldr sp, [r0], #4       */
    0xe8b0, 0x8ff0, /* ldmia r0!, {r4-r11,pc} */
    0x4770, 0x0000, /* bx lr                  */
};
/* Last bit of branch addresses must be 1 for Thumb */
func_t *switch_task = (func_t*)((uint8_t*)switch_task_asm + 1);

static uint8_t num_ready(void) {
  return ready.head - ready.tail;
}

static void push_ready(uint8_t val) {
  //assert(num_ready() != N_TASKS);
  uint8_t idx = ready.head++ & (N_TASKS - 1);
  ready.buffer[idx] = val;
}

static uint8_t pop_ready(void) {
  //assert(num_ready() != 0);
  uint8_t idx = ready.tail++ & (N_TASKS - 1);
  return ready.buffer[idx];
}

/* ==============================================
 * "sippin on gin and juice, Laid back"
 *      - Snoop Dogg (1993)
 *
 * Put all hardware to sleep and go into low power mode
 * ==============================================
 */
static void sys_sleep(void) {
  CyPmSaveClocks();
  CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_ONE_PPS);
  CyPmRestoreClocks();
  check_timers();
}

/* ==============================================
 * "I never sleep, 'cause sleep is the cousin of death"
 *      - NAS (1994)
 *
 * Pop task from ready queue and execute it
 * ==============================================
 */
static void schedule(void) {
  check_timers();
  while (num_ready() == 0)
    sys_sleep();
  uint8_t last = running;
  running = pop_ready();
  switch_task(tasks[running].regs, tasks[last].regs);
}

void block(void) {
  tasks[running].blocked = true;
  schedule();
}

void yield(void) {
  push_ready(running);
  schedule();
}

void terminate(void) {
  tasks[running].blocked = false;
  schedule();
}

void awake(uint8_t pid) {
  if (tasks[pid].blocked) {
    tasks[pid].blocked = false;
    push_ready(pid);
  }
}

void spawn(uint8_t pid, void (*main)(void*), void *param) {
  tasks[pid].regs[0] = (uint32_t)(tasks[pid].stack + STACK_SIZE);
  tasks[pid].regs[1] = (uint32_t)param;
  tasks[pid].regs[9] = (uint32_t)main;
  push_ready(pid);
}

void start_scheduler(void) {
  uint32_t trash[10];
  running = pop_ready();
  switch_task(tasks[running].regs, trash);
}

/* === Message Passing === */

typedef struct {
  msg_t buffer[N_MESSAGES];
  uint8_t head;
  uint8_t tail;
} msg_buf;

msg_buf msgs[N_PORTS];

static uint8_t num_msgs(uint8_t port) {
  return msgs[port].head - msgs[port].tail;
}

static void push_msgs(uint8_t port, const msg_t *val) {
  assert(num_msgs(port) != N_MESSAGES);
  /* if (num_msgs() == N_MESSAGES) msgs.tail++; */
  uint8_t idx = msgs[port].head++ & (N_MESSAGES - 1);
  msgs[port].buffer[idx] = *val;
}

static msg_t *pop_msgs(uint8_t port) {
  assert(num_msgs(port) != 0);
  uint8_t idx = msgs[port].tail++ & (N_MESSAGES - 1);
  return &msgs[port].buffer[idx];
}

void send(uint8_t port, msg_t *msg_in) {
  msg_in->sender = running;
  push_msgs(port, msg_in);
}

msg_t *recv(uint8_t port) {
  if (num_msgs(port) == 0) return NULL;
  return pop_msgs(port);
}

/* === Timing === */

typedef struct {
  uint32_t time;
  uint8_t pid;
} timer_t;

/* See Hierarchial Timing Wheels *
 * if we need better performance */
timer_t timers[N_TIMERS];
uint8_t num_timers;
volatile uint32_t now;

void RTC_ISR_EntryCallback(void) {
  ++now;
}

static void push_timers(const timer_t *val) {
  assert(num_timers != N_TIMERS);
  uint8_t i = num_timers++;
  for (; i > 0 && timers[i - 1].time < val->time; --i) {
    timers[i] = timers[i - 1];
  }
  timers[i] = *val;
}

static timer_t *pop_timers(void) {
  assert(num_timers != 0);
  return &timers[--num_timers];
}

static timer_t *top_timers(void) {
  return &timers[num_timers - 1];
}

static void check_timers(void) {
  while (num_timers != 0 && top_timers()->time <= now) {
    awake(pop_timers()->pid);
  }
}

void sleep(uint32_t ticks) {
  timer_t timer = {now + ticks, running};
  push_timers(&timer);
  block();
}

void ex_sleep(uint32_t ticks) {
  uint32_t wakeup = now + ticks;
  while (now <= wakeup) sys_sleep();
}

uint32_t time(void) {
  return now;
}

void set_time(uint32_t time) {
  int32_t delta = time - now; now = time;
  for (int i = 0; i < num_timers; ++i)
    timers[i].time += delta;
}
