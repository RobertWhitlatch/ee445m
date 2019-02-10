#ifndef __SYS_TICK__
#define __SYS_TICK__

#include "tm4c123gh6pm.h"

void SysTick_Init(uint32_t period){

    NVIC_ST_CTRL_R = 0x00;                                              // Disable SysTick during Initialization
    NVIC_ST_RELOAD_R = period-1;                                        // Load period into SysTick Reload Register  
    NVIC_ST_CURRENT_R = 0x00;                                           // Reset SysTick Current Register
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;          // Set SysTick Interrupt Priority to 2 
    NVIC_ST_CTRL_R = 0x07;                                              // Enable SysTick concluding Initialization 

}

void SysTick_Disable(void){

    NVIC_ST_CTRL_R = 0x00;

}

void SysTick_Enable(void){

    NVIC_ST_CTRL_R = 0x07;

}

#endif
