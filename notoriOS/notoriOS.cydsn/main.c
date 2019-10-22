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

typedef void func_t(uint32_t[10], uint32_t[10]);
const uint32_t switch_task_asm[] = {
    0xe8a16ff0, /* stmia r1!, {r4-r11,sp,lr} */
    0xe8b0aff0, /* ldmia r0!, {r4-r11,sp,pc} */
    0xe12fff1e, /* bx lr                     */
};
func_t *switch_task = (func_t*)switch_task_asm;

static uint8_t num_ready(void) {
  return ready.head - ready.tail;
}

static void push_ready(uint8_t val) {
  assert(num_ready() != N_TASKS);
  uint8_t idx = ready.head++ & (N_TASKS - 1);
  ready.buffer[idx] = val;
}

static uint8_t pop_ready(void) {
  assert(num_ready() != 0);
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

void spawn(uint8_t pid, const void (*main)(void)) {
  tasks[pid].regs[8] = (uint32_t)(&tasks[pid].stack + STACK_SIZE);
  tasks[pid].regs[9] = (uint32_t)main;
  push_ready(pid);
}

void start_scheduler(void) {
  uint32_t trash[10];
  running = pop_ready();
  switch_task(tasks[running].regs, trash);
}

/* === Message Passing === */

typedef struct msg_t {
  uint8_t data[63];
  uint8_t sender;
} msg_t;

typedef struct {
  msg_t buffer[N_MESSAGES];
  uint8_t head;
  uint8_t tail;
} msg_buf;

msg_buf msgs[N_PORTS];

static uint8_t num_msgs(uint8_t port) {
  return msgs[port].head - msgs[port].tail;
}

static void push_msgs(uint8_t port, msg_t *val) {
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

void RTC_EverySecondHandler_Callback(void) {
  ++now;
}

static void push_timers(timer_t *val) {
  assert(num_timers != N_TIMERS);
  uint8_t i = num_timers++;
  for (; i > 0 && timers[i - 1] < val->time; --i) {
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
  while (top_timers()->time <= now) {
    awake(pop_timers()->pid);
  }
}

void sleep(uint32_t ticks) {
  timer_t timer = {now + ticks, running};
  push_timers(&timer);
  block();
}

/* === Testing === */

char test_buffer[30];

void firstTask(void) {
  while (true) {
    strcpy(&test_buffer, "Switching to otherTask...");
    yield();
    strcpy(&test_buffer, "Returned to mainTask!    ");
  }
}

void secondTask(void) {
  while (true) {
    strcpy(&test_buffer, "Hello multitasking world!");
    yield();
  }
}

int main(void) {
  RTC_Start();
  CyGlobalIntEnable;

  spawn(0, firstTask);
  spawn(1, secondTask);
  start_scheduler();
}
