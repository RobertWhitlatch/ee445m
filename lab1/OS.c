#include "CustomDefinitions.h"

void (*PeriodicTask)(void);

// ***************** OS_AddPeriodicThread ****************
// Submits a task to be preformed periodically
// Inputs:  task is a pointer to a user function,
//          period in usec, priority of the task
// Outputs: none
void OS_AddPeriodicThread(void(*task)(void), uint32_t period, uint32_t priority){
    SYSCTL_RCGCTIMER_R |= 0x10;                             // 0) activate TIMER4
    period *= 80;
    priority &= 0x00000007;
    PeriodicTask = task;                                    // user function
    TIMER4_CTL_R = 0x00000000;                              // 1) disable TIMER4A during setup
    TIMER4_CFG_R = 0x00000000;                              // 2) configure for 32-bit mode
    TIMER4_TAMR_R = 0x00000012;                             // 3) configure for periodic mode, default down-count settings
    TIMER4_TAILR_R = period-1;                              // 4) reload value
    TIMER4_TAPR_R = 0;                                      // 5) bus clock resolution
    TIMER4_ICR_R = 0x00000001;                              // 6) clear TIMER4A timeout flag
    TIMER4_IMR_R = 0x00000001;                              // 7) arm timeout interrupt
    NVIC_PRI17_R = (NVIC_PRI17_R&0xFF00FFFF)|(priority<<21);// 8) priority
    NVIC_EN2_R = 1<<(70-64);                                // 9) enable IRQ 70 in NVIC
    TIMER4_CTL_R = 0x00000001;                              // 10) enable TIMER4A
}

// ***************** OS_ClearPeriodicTime ****************
// Resets periodic timer
// Inputs:  task is a pointer to a user function,
//          period in usec, priority of the task
// Outputs: none
void OS_ClearPeriodicTime(void){
    TIMER4_TAV_R = 0;
}

// ***************** OS_ReadPeriodicTime ****************
// Return value of periodic timer
// Inputs:  task is a pointer to a user function,
//          period in usec, priority of the task
// Outputs: none
uint32_t OS_ReadPeriodicTime(void){
    return(TIMER4_TAV_R);
}

void Timer4A_Handler(void){
    TIMER4_ICR_R = TIMER_ICR_TATOCINT;                      // acknowledge TIMER4A timeout
    (*PeriodicTask)();                                      // execute user task
}
