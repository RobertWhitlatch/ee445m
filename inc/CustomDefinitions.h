#ifndef __CUSTOM_DEFINITIONS__
#define __CUSTOM_DEFINITIONS__

#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "FiFo.h"
#include "Stack.h"
#include "SysTick.h"
#include "StandardInOut.h"
#include "MiscFunctions.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define GPIO_LOCK_KEY   0x4C4F434B
#define PA0             (*((volatile uint32_t *)0x40004004))
#define PA1             (*((volatile uint32_t *)0x40004008))
#define PA2             (*((volatile uint32_t *)0x40004010))
#define PA3             (*((volatile uint32_t *)0x40004020))
#define PA4             (*((volatile uint32_t *)0x40004040))
#define PA5             (*((volatile uint32_t *)0x40004080))
#define PA6             (*((volatile uint32_t *)0x40004100))
#define PA7             (*((volatile uint32_t *)0x40004200))
#define PB0             (*((volatile uint32_t *)0x40005004))
#define PB1             (*((volatile uint32_t *)0x40005008))
#define PB2             (*((volatile uint32_t *)0x40005010))
#define PB3             (*((volatile uint32_t *)0x40005020))
#define PB4             (*((volatile uint32_t *)0x40005040))
#define PB5             (*((volatile uint32_t *)0x40005080))
#define PB6             (*((volatile uint32_t *)0x40005100))
#define PB7             (*((volatile uint32_t *)0x40005200))
#define PC0             (*((volatile uint32_t *)0x40006004))
#define PC1             (*((volatile uint32_t *)0x40006008))
#define PC2             (*((volatile uint32_t *)0x40006010))
#define PC3             (*((volatile uint32_t *)0x40006020))
#define PC4             (*((volatile uint32_t *)0x40006040))
#define PC5             (*((volatile uint32_t *)0x40006080))
#define PC6             (*((volatile uint32_t *)0x40006100))
#define PC7             (*((volatile uint32_t *)0x40006200))
#define PD0             (*((volatile uint32_t *)0x40007004))
#define PD1             (*((volatile uint32_t *)0x40007008))
#define PD2             (*((volatile uint32_t *)0x40007010))
#define PD3             (*((volatile uint32_t *)0x40007020))
#define PD4             (*((volatile uint32_t *)0x40007040))
#define PD5             (*((volatile uint32_t *)0x40007080))
#define PD6             (*((volatile uint32_t *)0x40007100))
#define PD7             (*((volatile uint32_t *)0x40007200))
#define PE0             (*((volatile uint32_t *)0x40024004))
#define PE1             (*((volatile uint32_t *)0x40024008))
#define PE2             (*((volatile uint32_t *)0x40024010))
#define PE3             (*((volatile uint32_t *)0x40024020))
#define PE4             (*((volatile uint32_t *)0x40024040))
#define PE5             (*((volatile uint32_t *)0x40024080))
#define PF0             (*((volatile uint32_t *)0x40025004))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF3             (*((volatile uint32_t *)0x40025020))
#define PF4             (*((volatile uint32_t *)0x40025040))

#endif // __CUSTOM_DEFINITIONS__
