#include "CustomDefinitions.h"
#include "OS.h"

unsigned long NumCreated;           // number of foreground threads created

void PortE_Init(void){
    unsigned long volatile delay;
    SYSCTL_RCGC2_R |= 0x10;         // activate port E
    delay = SYSCTL_RCGC2_R;
    delay = SYSCTL_RCGC2_R;
    GPIO_PORTE_DIR_R |= 0x0F;       // make PE3-0 output heartbeats
    GPIO_PORTE_AFSEL_R &= ~0x0F;    // disable alt funct on PE3-0
    GPIO_PORTE_DEN_R |= 0x0F;       // enable digital I/O on PE3-0
    GPIO_PORTE_PCTL_R = ~0x0000FFFF;
    GPIO_PORTE_AMSEL_R &= ~0x0F;;   // disable analog functionality on PF
}

//*******************Initial TEST**********
// This is the simplest configuration, test this first, (Lab 1 part 1)
// run this with 
// no UART interrupts
// no SYSTICK interrupts
// no timer interrupts
// no switch interrupts
// no ADC serial port or LCD output
// no calls to semaphores
unsigned long Count1;   // number of times thread1 loops
unsigned long Count2;   // number of times thread2 loops
unsigned long Count3;   // number of times thread3 loops
unsigned long Count4;   // number of times thread4 loops
unsigned long Count5;   // number of times thread5 loops
void Thread1(void){
  Count1 = 0;          
  for(;;){
    PE0 ^= 0x01;       // heartbeat
    Count1++;
    OS_Suspend();      // cooperative multitasking
  }
}
void Thread2(void){
  Count2 = 0;          
  for(;;){
    PE1 ^= 0x02;       // heartbeat
    Count2++;
    OS_Suspend();      // cooperative multitasking
  }
}
void Thread3(void){
  Count3 = 0;          
  for(;;){
    PE2 ^= 0x04;       // heartbeat
    Count3++;
    OS_Suspend();      // cooperative multitasking
  }
}

int main(void){  // Testmain1
  OS_Init();          // initialize, disable interrupts
  PortE_Init();       // profile user threads
  NumCreated = 0 ;
  NumCreated += OS_AddThread(&Thread1,128,1); 
  NumCreated += OS_AddThread(&Thread2,128,2); 
  NumCreated += OS_AddThread(&Thread3,128,3); 
  // Count1 Count2 Count3 should be equal or off by one at all times
  OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
  return 0;            // this never executes
}

//*******************Second TEST**********
// Once the initalize test runs, test this (Lab 1 part 1)
// no UART interrupts
// SYSTICK interrupts, with or without period established by OS_Launch
// no timer interrupts
// no switch interrupts
// no ADC serial port or LCD output
// no calls to semaphores
void Thread1b(void){
  Count1 = 0;          
  for(;;){
    PE0 ^= 0x01;       // heartbeat
    Count1++;
  }
}
void Thread2b(void){
  Count2 = 0;          
  for(;;){
    PE1 ^= 0x02;       // heartbeat
    Count2++;
  }
}
void Thread3b(void){
  Count3 = 0;          
  for(;;){
    PE2 ^= 0x04;       // heartbeat
    Count3++;
  }
}
int main2(void){          // Testmain2
    OS_Init();            // initialize, disable interrupts
    PortE_Init();         // profile user threads
    NumCreated = 0 ;
    NumCreated += OS_AddThread(&Thread1b,128,1); 
    NumCreated += OS_AddThread(&Thread2b,128,2); 
    NumCreated += OS_AddThread(&Thread3b,128,3); 
    // Count1 Count2 Count3 should be equal on average
    // counts are larger than testmain1
    
    OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
    return 0;            // this never executes
}
