#include "CustomDefinitions.h"

#ifndef __OS_H__
#define __OS_H__

#define MAX_THREADS 8
#define STACK_SIZE 512

typedef struct TCB TCB_t;

struct TCB {
    uint32_t* sp;
    TCB_t* next;
    TCB_t* prev;
    uint32_t stack[STACK_SIZE];
    uint32_t pid;
    uint32_t sleep;
    uint32_t priority;
    uint32_t blocked;
};

// ***************** OS_AddPeriodicThread ****************
// Submits a task to be preformed periodically
// Inputs:  task is a pointer to a user function,
//          period in usec, priority of the task
// Outputs: none
void OS_AddPeriodicThread(void(*task)(void), uint32_t period, uint32_t priority);

// ***************** OS_ClearPeriodicTime ****************
// Resets periodic timer
// Inputs:  task is a pointer to a user function,
//          period in usec, priority of the task
// Outputs: none
void OS_ClearPeriodicTime(void);

// ***************** OS_ReadPeriodicTime ****************
// Return value of periodic timer
// Inputs:  task is a pointer to a user function,
//          period in usec, priority of the task
// Outputs: none
uint32_t OS_ReadPeriodicTime(void);

#endif
