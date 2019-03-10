#include <stdbool.h>

#include "CustomDefinitions.h"
#include "GPIO.h"
#include "OS.h"
#include "PLL.h"
#include "Timers.h"
#include "Output.h"
#include "Jitter.h"

#define MAX_THREADS 8

uint8_t cleanUp = 0;
uint32_t msTime = 0;
TCB_t threads[MAX_THREADS];
uint32_t numberThreads = 0;
TCB_t* RunPt = threads;
TCB_t* NextPt;
uint32_t active[MAX_THREADS];
uint32_t blocked[MAX_THREADS];
uint32_t sleep[MAX_THREADS];

// defined in osasm.s
void AtomicCompareAdd(long* semaPt, long value);
long AtomicCompareAddNonBlocking(long* semaPt, long value);
void StartOS(void);

void SysTick_Init(uint32_t period) {
  NVIC_ST_CTRL_R = 0x00;          // Disable SysTick during Initialization
  NVIC_ST_RELOAD_R = period - 1;  // Load period into SysTick Reload Register
  NVIC_ST_CURRENT_R = 0x00;       // Reset SysTick Current Register
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) |
                    0xC0000000;  // Set SysTick Interrupt Priority to 6 {
                                 // priority 2 is 0x40000000 }
  NVIC_ST_CTRL_R = 0x07;         // Enable SysTick concluding Initialization
}

// ******** OS_Init ************
// initialize operating system, disable interrupts until OS_Launch
// initialize OS controlled I/O: serial, ADC, systick, LaunchPad I/O and timers
// input:  none
// output: none
void OS_Init(void) {
  NVIC_SYS_PRI3_R |= 0x00E00000;
  DisableInterrupts();
  PLL_Init(Bus80MHz);
  SystemTime_Init();
  Output_Init();
  SW_Init();
  for (int i = 0; i < MAX_THREADS; i++) {
    threads[i].living = 0;
  }
}

void Stack_Init(TCB_t* thread, void (*task)(void)) {
  *(--(thread->sp)) = 0x01000000;  // thumb bit
  *(--(thread->sp)) = (int32_t)(task);
  *(--(thread->sp)) = 0x14141414;  // LR
  *(--(thread->sp)) = 0x12121212;  // R12
  *(--(thread->sp)) = 0x03030303;  // R3
  *(--(thread->sp)) = 0x02020202;  // R2
  *(--(thread->sp)) = 0x01010101;  // R1
  *(--(thread->sp)) = 0x00000000;  // R0
  *(--(thread->sp)) = 0x11111111;  // R11
  *(--(thread->sp)) = 0x10101010;  // R10
  *(--(thread->sp)) = 0x09090909;  // R9
  *(--(thread->sp)) = 0x08080808;  // R8
  *(--(thread->sp)) = 0x07070707;  // R7
  *(--(thread->sp)) = 0x06060606;  // R6
  *(--(thread->sp)) = 0x05050505;  // R5
  *(--(thread->sp)) = 0x04040404;  // R4
}

//******** OS_AddThread ***************
// add a foregound thread to the scheduler
// Inputs: pointer to a void/void foreground task
//         number of bytes allocated for its stack
//         priority, 0 is highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// stack size must be divisable by 8 (aligned to double word boundary)
// In Lab 2, you can ignore both the stackSize and priority fields
// In Lab 3, you can ignore the stackSize fields
int OS_AddThread(void (*task)(void), unsigned long stackSize,
                 unsigned long priority) {
  int threadIndex;
  for (threadIndex = 0; threadIndex < MAX_THREADS; threadIndex++) {
    if (threads[threadIndex].living == 0) {
      break;
    }
  }
  if (threadIndex == MAX_THREADS) {
    return (0);
  }
  threads[threadIndex].sp = &threads[threadIndex].pid;
  Stack_Init(&threads[threadIndex], task);
  if (numberThreads == 0) {
    threads[threadIndex].next = &threads[threadIndex];
  } else {
    TCB_t* ptr = RunPt;
    for (unsigned int i = 0; i < numberThreads; i++) {
      if (ptr->next == RunPt) {
        threads[threadIndex].next = ptr->next;
        ptr->next = &threads[threadIndex];
        break;
      } else {
        ptr = ptr->next;
      }
    }
  }
  threads[threadIndex].pid = threadIndex;
  threads[threadIndex].priority = priority;
  threads[threadIndex].blocked = 0;
  threads[threadIndex].living = 1;
  active[threadIndex] = 1;
  numberThreads++;
  return (1);
}

//******** OS_Id ***************
// returns the thread ID for the currently running thread
// Inputs: none
// Outputs: Thread ID, number greater than zero
unsigned long OS_Id(void) { return (RunPt->pid); }

//******** OS_AddPeriodicThread ***************
// add a background periodic task
// typically this function receives the highest priority
// Inputs: pointer to a void/void background function
//         period given in system time units (12.5ns)
//         priority 0 is the highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// You are free to select the time resolution for this function
// It is assumed that the user task will run to completion and return
// This task can not spin, block, loop, sleep, or kill
// This task can call OS_Signal  OS_bSignal     OS_AddThread
// This task does not have a Thread ID
// In lab 2, this command will be called 0 or 1 times
// In lab 2, the priority field can be ignored
// In lab 3, this command will be called 0 1 or 2 times
// In lab 3, there will be up to four background threads, and this priority
// field determines the relative priority of these four threads

#ifndef PRIORITY_SCHEDULER
void (*PeriodicTask1)(void);
void (*PeriodicTask2)(void);
uint16_t periodicStatus[2] = {1,1};
int OS_AddPeriodicThread(void (*task)(void), unsigned long period, unsigned long priority) {
    if (periodicStatus[0]) {
        JitterReset(periodic_jitter);
        periodic_jitter[0].period = period;
        PeriodicTask1 = task;
        PeriodicTimer1_Init(period, priority);
        periodicStatus[0] = 0;
        return (1);
    } else if(periodicStatus[1]){
        JitterReset(periodic_jitter+1);
        periodic_jitter[1].period = period;
        PeriodicTask2 = task;
        PeriodicTimer2_Init(period, priority);
        periodicStatus[1] = 0;
        return(1);
    }
    return (0);
}

void Timer4A_Handler(void) {
  TIMER4_ICR_R = TIMER_ICR_TATOCINT;  // acknowledge TIMER4A timeout
  JitterUpdate(periodic_jitter);
  (*PeriodicTask1)();                  // execute user task
}


void Timer5A_Handler(void) {
  TIMER5_ICR_R = TIMER_ICR_TATOCINT;  // acknowledge TIMER5A timeout
  JitterUpdate(periodic_jitter+1);
  (*PeriodicTask2)();                  // execute user task
}

int OS_RemovePeriodicThread(void (*task)(void)){
    if(task == PeriodicTask1){
        periodicStatus[0] = 1;
        SYSCTL_RCGCTIMER_R ^= 0x10;
        return(1);
    }else if(task == PeriodicTask2){
        periodicStatus[1] = 1;
        SYSCTL_RCGCTIMER_R ^= 0x10;
        return(1);
    }
    return(0);
}
#endif // PRIORITY_SCHEDULER

//******** OS_AddSW1Task ***************
// add a background task to run whenever the SW1 (PF4) button is pushed
// Inputs: pointer to a void/void background function
//         priority 0 is the highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// It is assumed that the user task will run to completion and return
// This task can not spin, block, loop, sleep, or kill
// This task can call OS_Signal  OS_bSignal     OS_AddThread
// This task does not have a Thread ID
// In labs 2 and 3, this command will be called 0 or 1 times
// In lab 2, the priority field can be ignored
// In lab 3, there will be up to four background threads, and this priority
// field
//           determines the relative priority of these four threads
extern void (*job1)(void);
int OS_AddSW1Task(void (*task)(void), unsigned long priority) {
  SW1_Arm(priority);
  job1 = task;
  return (1);
}

//******** OS_AddSW2Task ***************
// add a background task to run whenever the SW2 (PF0) button is pushed
// Inputs: pointer to a void/void background function
//         priority 0 is highest, 5 is lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// It is assumed user task will run to completion and return
// This task can not spin block loop sleep or kill
// This task can call issue OS_Signal, it can call OS_AddThread
// This task does not have a Thread ID
// In lab 2, this function can be ignored
// In lab 3, this command will be called will be called 0 or 1 times
// In lab 3, there will be up to four background threads, and this priority
// field
//           determines the relative priority of these four threads
extern void (*job2)(void);
int OS_AddSW2Task(void (*task)(void), unsigned long priority) {
  SW2_Arm(priority);
  job2 = task;
  return (1);
}

// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of slices to sleep
// output: none
// You are free to select the time resolution for this function
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(unsigned long sleepTime) {
  if (sleepTime == 0) {
    OS_Suspend();
  } else {
    RunPt->sleep = sleepTime;
    OS_Suspend();
  }
}

// ******** OS_Kill ************
// kill the currently running thread, release its TCB and stack
// input:  none
// output: none
void OS_Kill(void) {
  RunPt->dying = 1;
  cleanUp = 1;
  OS_Suspend();
}

// ******** OS_Suspend ************
// suspend execution of currently running thread
// scheduler will choose another thread to execute
// Can be used to implement cooperative multitasking
// Same function as OS_Sleep(0)
// input:  none
// output: none
void OS_Suspend(void) {
  // FIXME: OS_Suspend() is non-reentrant
  NextPt = RunPt->next;
  while (NextPt->sleep > 0 || NextPt->dying == 1 || NextPt->living == 0 ||
         NextPt->blocked) {
    NextPt = NextPt->next;
  }
  NVIC_INT_CTRL_R = NVIC_INT_CTRL_PEND_SV;
}

#define OS_FIFO_SIZE 128
static struct FIFO_s {
  uint32_t data[OS_FIFO_SIZE];
  uint32_t length;     // the maximum depth of the FIFO
  uint32_t head;       // the index of oldest value in the FIFO
  uint32_t tail;       // the index of next empty space in the FIFO
  Sema4Type canRead;   // the number of values stored
  Sema4Type canWrite;  // the number of spaces remaining
  Sema4Type readLock;
  Sema4Type writeLock;
} fifo;

// ******** OS_Fifo_Init ************
// Initialize the Fifo to be empty
// Inputs: size
// Outputs: none
// In Lab 2, you can ignore the size field
// In Lab 3, you should implement the user-defined fifo size
// In Lab 3, you can put whatever restrictions you want on size
//    e.g., 4 to 64 elements
//    e.g., must be a power of 2,4,8,16,32,64,128
void OS_Fifo_Init(unsigned long size) {
  fifo.length = size;
  fifo.head = 0;
  fifo.tail = 0;
  OS_InitSemaphore(&fifo.canRead, 0);
  OS_InitSemaphore(&fifo.canWrite, size);
  OS_InitSemaphore(&fifo.readLock, 1);
  OS_InitSemaphore(&fifo.writeLock, 1);
}

// ******** OS_Fifo_Put ************
// Enter one data sample into the Fifo
// Called from the background, so no waiting
// Inputs:  data
// Outputs: true if data is properly saved,
//          false if data not saved, because it was full
// Since this is called by interrupt handlers
//  this function can not disable or enable interrupts
int OS_Fifo_Put(unsigned long data) {
  if (!OS_Acquire(&fifo.writeLock)) {
    return 0;
  }
  if (!OS_Acquire(&fifo.canWrite)) {
    OS_Signal(&fifo.writeLock);
    return 0;
  }
  fifo.data[fifo.tail++] = data;
  fifo.tail %= fifo.length;
  OS_Signal(&fifo.canRead);
  OS_Signal(&fifo.writeLock);
  return 1;
}

// ******** OS_Fifo_Get ************
// Remove one data sample from the Fifo
// Called in foreground, will spin/block if empty
// Inputs:  none
// Outputs: data
unsigned long OS_Fifo_Get(void) {
  OS_Wait(&fifo.readLock);
  OS_Wait(&fifo.canRead);
  unsigned long data = fifo.data[fifo.head++];
  fifo.head %= fifo.length;
  OS_Signal(&fifo.canWrite);
  OS_Signal(&fifo.readLock);
  return data;
}

// ******** OS_Fifo_Size ************
// Check the status of the Fifo
// Inputs: none
// Outputs: returns the number of elements in the Fifo
//          greater than zero if a call to OS_Fifo_Get will return right away
//          zero or less than zero if the Fifo is empty
//          zero or less than zero if a call to OS_Fifo_Get will spin or block
long OS_Fifo_Size(void) { return fifo.canRead.Value; }

static struct Mailbox_s {
  unsigned long data;
  Sema4Type empty;
  Sema4Type full;
} mailbox;

// ******** OS_MailBox_Init ************
// Initialize communication channel
// Inputs:  none
// Outputs: none
void OS_MailBox_Init(void) {
  OS_InitSemaphore(&mailbox.empty, 1);
  OS_InitSemaphore(&mailbox.full, 0);
}

// ******** OS_MailBox_Send ************
// Enter mail into the MailBox
// Inputs:  data to be sent
// Outputs: none
// This function will be called from a foreground thread
// It will spin/block if the MailBox contains data not yet received
void OS_MailBox_Send(unsigned long data) {
  OS_bWait(&mailbox.empty);
  mailbox.data = data;
  OS_bSignal(&mailbox.full);
}

// ******** OS_MailBox_Recv ************
// remove mail from the MailBox
// Inputs:  none
// Outputs: data received
// This function will be called from a foreground thread
// It will spin/block if the MailBox is empty
unsigned long OS_MailBox_Recv(void) {
  OS_bWait(&mailbox.full);
  unsigned long data = mailbox.data;
  OS_bSignal(&mailbox.empty);
  return data;
}

// ******** OS_Time ************
// Returns the system time
// Inputs:  none
// Outputs: time in 12.5ns units, 0 to 4294967295
unsigned long OS_Time(void) { return (WTIMER5_TAV_R); }

// ******** OS_TimeDifference ************
// Calculates difference between two times
// Inputs:  two times measured with OS_Time
// Outputs: time difference in 12.5ns units
unsigned long OS_TimeDifference(unsigned long start, unsigned long stop) {
  if (stop < start) {
    return ((0xFFFFFFFF - start) + stop);
  }
  return (stop - start);
}

// ******** OS_ClearMsTime ************
// sets the system time to zero (from Lab 1)
// Inputs:  none
// Outputs: none
// You are free to change how this works
void OS_ClearMsTime(void) { msTime = 0; }

// ******** OS_MsTime ************
// reads the current time in msec (from Lab 1)
// Inputs:  none
// Outputs: time in ms units
// You are free to select the time resolution for this function
// It is ok to make the resolution to match the first call to
// OS_AddPeriodicThread
unsigned long OS_MsTime(void) { return (msTime); }

//******** OS_Launch ***************
// start the scheduler, enable interrupts
// Inputs: number of 12.5ns clock cycles for each time slice
//         you may select the units of this parameter
// Outputs: none (does not return)
// In Lab 2, you can ignore the theTimeSlice field
// In Lab 3, you should implement the user-defined TimeSlice field
// It is ok to limit the range of theTimeSlice to match the 24-bit SysTick
void OS_Launch(unsigned long theTimeSlice) {
  SysTick_Init(theTimeSlice);
  StartOS();
}

void SysTick_Handler(void) {
  msTime += 2;
  if (cleanUp) {
    cleanUp = 0;
    TCB_t* ptr = RunPt;
    while (!ptr->next->dying) {
      ptr = ptr->next;
    }
    ptr->next->living = 0;
    ptr->next->dying = 0;
    ptr->next = ptr->next->next;
  }
  for (int i = 0; i < MAX_THREADS; i++) {
    if (threads[i].sleep > 0) {
      threads[i].sleep--;
    }
  }
  OS_Suspend();
}
