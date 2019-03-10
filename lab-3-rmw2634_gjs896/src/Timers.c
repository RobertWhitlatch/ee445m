// Timers.c

// Hardware timer usage:
// TIMER0-3:  ultrasonic distance sensors
// TIMER4-5:  unused
// WTIMER0:   unused
// WTIMER1-2: periodic tasks
// WTIMER3-4: switch debounce
// WTIMER5:   system time

#include "CustomDefinitions.h"

void SystemTime_Init(void){
    SYSCTL_RCGCWTIMER_R = 0x20;
    while((SYSCTL_RCGCWTIMER_R & 0x20) == 0);
    WTIMER5_CTL_R = 0x00000000;
    WTIMER5_CFG_R = 0x00000004;
    WTIMER5_TAMR_R = 0x00000012;
    WTIMER5_TAMATCHR_R = 0xFFFFFFFF;
    WTIMER5_CTL_R = 0x00000001;
}

void PeriodicTimer1_Init(unsigned long period, unsigned long priority){
    SYSCTL_RCGCTIMER_R |= 0x10;                             // 0) activate TIMER4
    priority = (priority > 5) ? 5 : priority;
    TIMER4_CTL_R = 0x00000000;                              // 1) disable TIMER4A during setup
    TIMER4_CFG_R = 0x00000000;                              // 2) configure for 32-bit mode
    TIMER4_TAMR_R = 0x00000012;                             // 3) configure for periodic mode, default down-count settings
    TIMER4_TAILR_R = period-1;                              // 4) reload value
    TIMER4_TAPR_R = 0;                                      // 5) bus clock resolution
    TIMER4_ICR_R = 0x00000001;                              // 6) clear TIMER4A timeout flag
    TIMER4_IMR_R = 0x00000001;                              // 7) arm timeout interrupt
    NVIC_PRI17_R = (NVIC_PRI17_R&0xFF00FFFF)|(priority<<21);// 8) priority
    NVIC_EN2_R |= 1<<(70-64);                               // 9) enable IRQ 70 in NVIC
    TIMER4_CTL_R = 0x00000001;                              // 10) enable TIMER4A
}

void PeriodicTimer2_Init(unsigned long period, unsigned long priority){
    SYSCTL_RCGCTIMER_R |= 0x20;                             // 0) activate TIMER5
    priority = (priority > 5) ? 5 : priority;
    TIMER5_CTL_R = 0x00000000;                              // 1) disable TIMER5A during setup
    TIMER5_CFG_R = 0x00000000;                              // 2) configure for 32-bit mode
    TIMER5_TAMR_R = 0x00000012;                             // 3) configure for periodic mode, default down-count settings
    TIMER5_TAILR_R = period-1;                              // 4) reload value
    TIMER5_TAPR_R = 0;                                      // 5) bus clock resolution
    TIMER5_ICR_R = 0x00000001;                              // 6) clear TIMER5A timeout flag
    TIMER5_IMR_R = 0x00000001;                              // 7) arm timeout interrupt
    NVIC_PRI23_R = (NVIC_PRI23_R&0xFFFFFF00)|(priority<<5); // 8) priority
    NVIC_EN2_R |= 1<<(92-64);                               // 9) enable IRQ 92 in NVIC
    TIMER5_CTL_R = 0x00000001;                              // 10) enable TIMER4A
}
