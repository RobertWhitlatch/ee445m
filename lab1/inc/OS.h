#include "CustomDefinitions.h"

#ifndef __OS_H__
#define __OS_H__

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
