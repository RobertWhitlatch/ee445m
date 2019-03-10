// \file OS_PeriodicThreads.c
// Uses WideTimer1A/1B/2A/2B to support up to 4 tasks
// at intervals from 1 to 2**32 clock cycles (53.7 seconds).
// (WideTimer0 is used for the system clock.)

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"

#include "OS.h"

#ifdef PRIORITY_SCHEDULER

// // // // // begin private API // // // // //
// timer driver ported from driverlib/timer.c

typedef enum TimerId_e {
  Task1A,
  Task1B,
  Task2A,
  Task2B,
  MAX_PERIODIC_TASKS
} TimerId_t;

typedef struct Timer_s {
  uint32_t moduleNumber;
  uint32_t baseAddress;
  uint32_t interrupts[2];
} Timer_t;

typedef struct SubTimer_s {
  uint32_t enableFlags;
  uint32_t loadRegOffset;
  uint32_t ackFlags;
} SubTimer_t;

#define TIMER_S(id)                       \
  {                                       \
    .moduleNumber = 1 << id,              \
    .baseAddress = WTIMER##id##_BASE,     \
    .interrupts = {                       \
      INT_WTIMER##id##A - 16,             \
      INT_WTIMER##id##B - 16,             \
    }                                     \
  }

#define SUBTIMER_S(id)                    \
  {                                       \
    .enableFlags = TIMER_CTL_T##id##EN,   \
    .loadRegOffset = TIMER_O_T##id##ILR,  \
    .ackFlags = TIMER_ICR_T##id##TOCINT   \
  }

static const Timer_t timers[MAX_PERIODIC_TASKS / 2] = {TIMER_S(1), TIMER_S(2)};

static const SubTimer_t subTimers[2] = {SUBTIMER_S(A), SUBTIMER_S(B)};

static inline void _TimerStart(TimerId_t id,
                               uint32_t interval,
                               uint8_t priority) {
  uint32_t baseAddress = timers[id / 2].baseAddress;
  uint32_t interrupt = timers[id / 2].interrupts[id % 2];

  HWREG(baseAddress + subTimers[id % 2].loadRegOffset) = interval - 1;
  HWREG(baseAddress + TIMER_O_CTL) |= subTimers[id % 2].enableFlags;
  HWREGB(NVIC_PRI0 + interrupt) = (priority % 8) << 5;
  HWREG(NVIC_EN0 + interrupt / 32) = 1 << (interrupt % 32);
}

static inline void _TimerAck(TimerId_t id) {
  HWREG(timers[id / 2].baseAddress + TIMER_O_ICR) |= subTimers[id % 2].ackFlags;
}

static inline void _TimerStop(TimerId_t id) {
  uint32_t baseAddress = timers[id / 2].baseAddress;
  uint32_t interrupt = timers[id / 2].interrupts[id % 2];

  HWREG(baseAddress + TIMER_O_CTL) &= ~subTimers[id % 2].enableFlags;
  HWREG(NVIC_DIS0 + interrupt / 32) = 1 << (interrupt % 32);
}

static inline void _TimerInit(const Timer_t *timer) {
  HWREG(SYSCTL_RCGCWTIMER) |= timer->moduleNumber;
  while (!(HWREG(SYSCTL_PRWTIMER) & timer->moduleNumber)) {
  }
  HWREG(timer->baseAddress + TIMER_O_CTL) = 0;
  HWREG(timer->baseAddress + TIMER_O_CFG) = TIMER_CFG_16_BIT;  // 2x32b timers
  HWREG(timer->baseAddress + TIMER_O_TAMR) = TIMER_TAMR_TAMR_PERIOD;
  HWREG(timer->baseAddress + TIMER_O_TBMR) = TIMER_TBMR_TBMR_PERIOD;
  HWREG(timer->baseAddress + TIMER_O_IMR) = TIMER_IMR_TATOIM | TIMER_IMR_TBTOIM;
}

// // // // // begin interrupt handlers // // // // //

static void (*periodicTasks[MAX_PERIODIC_TASKS])(void);
static Sema4Type periodicTaskMutex;

#define ADD_HANDLER(TMR)                \
  void WideTimer##TMR##_Handler(void) { \
    _TimerAck(Task##TMR);               \
    periodicTasks[Task##TMR]();         \
  }

ADD_HANDLER(1A)
ADD_HANDLER(1B)
ADD_HANDLER(2A)
ADD_HANDLER(2B)

// // // // // begin public API // // // // //

//! \brief Set up the periodic task timers.
void OS_InitPeriodicThreads(void) {
  for (int id = 0; id < MAX_PERIODIC_TASKS / 2; id++) {
    _TimerInit(timers + id);
  }
  for (int id = 0; id < MAX_PERIODIC_TASKS; id++) {
    periodicTasks[id] = 0;
  }
  OS_InitSemaphore(&periodicTaskMutex, 1);
}

//! \brief Schedule a periodic task.
//! \param task A pointer to the task to be executed.
//! \param period The time interval in bus clock periods (12.5ns).
//! \param priority A number from 0 (highest) to 5 (lowest)
//! \retval 1 if the task was added, 0 if all task slots are full.
//! task() runs in Handler mode and must return.
//! A maximum of four periodic tasks can be scheduled at a time.
int OS_AddPeriodicThread_Clara(void (*task)(void), unsigned long period,
                         unsigned long priority) {
  OS_Wait(&periodicTaskMutex);
  for (int taskId = 0; taskId < (int)MAX_PERIODIC_TASKS; taskId++) {
    TimerId_t taskID = (TimerId_t) taskId;
    if (!periodicTasks[taskID]) {
      periodicTasks[taskID] = task;
      _TimerStart(taskID, period, priority);
      OS_Signal(&periodicTaskMutex);
      return 1;
    }
  }
  OS_Signal(&periodicTaskMutex);
  return 0;
}

//! \brief Remove a periodic task from the schedule.
//! \param task A pointer to the task to be deleted.
//! \retval 1 if the task was deleted, 0 if the task was never scheduled.
int OS_DeletePeriodicThread(void (*task)(void)) {
  OS_Wait(&periodicTaskMutex);
  for (int taskId = 0; taskId < (int)MAX_PERIODIC_TASKS; taskId++) {
    TimerId_t taskID = (TimerId_t) taskId;
    if (periodicTasks[taskID] == task) {
      _TimerStop(taskID);
      periodicTasks[taskID] = 0;
      OS_Signal(&periodicTaskMutex);
      return 1;
    }
  }
  OS_Signal(&periodicTaskMutex);
  return 0;
}

#endif // PRIORITY_SCHEDULER
