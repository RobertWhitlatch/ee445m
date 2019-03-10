// Lab3.h
// Runs on LM4F120/TM4C123
// Real Time Operating System for Labs 2 and 3

// Jonathan W. Valvano 2/20/17, valvano@mail.utexas.edu
// EE445M/EE380L.12
// You may use, edit, run or distribute this file
// You are free to change the syntax/organization of this file

// GPIO outputs to logic analyzer for userspace profile
// PE0 is DAS(): toggled on entry, after ADC_In(), and exit
// PE1 is ButtonWork(): toggled on entry, after first ST7735_Message(), and exit
// PE2 is Consumer(): set before 1st OS_Fifo_Get(); cleared before starting FFT

// LED outputs to logic analyzer for OS profile
// PF1 is preemptive thread switch
// PF2 is periodic task, samples PD3
// PF3 is SW1 task (touch PF4 button)

// Button inputs
// PF0 is SW2 task (Lab3)
// PF4 is SW1 button input

// Analog inputs
// PD3 Ain3 sampled at (1/PERIOD)=2kHz by ADC_In()
// PD2 Ain5 sampled at FS=400Hz by ADC_Collect()

#ifndef __LAB3_H__
#define __LAB3_H__

#define Lab2 0
#define Lab3 (!Lab2)

// test setup
#define FIFO_SIZE 32
#define TIMESLICE (2*TIME_1MS)
#define FS 400                   // 400Hz producer/consumer sampling frequency
#define RUNLENGTH (20*FS)        // 20-sec finite time experiment duration
#define PERIOD TIME_500US        // DAS 2kHz sampling period
#if Lab2
#define JITTERSIZE 64            // number of bins in jitter histogram
#endif

// profiling data
extern unsigned long NumCreated; // number of foreground threads created
extern unsigned long PIDWork;    // number of PID calculations finished
extern unsigned long FilterWork; // number of filter calculations finished
extern unsigned long NumSamples; // incremented every ADC sample, in Producer
extern long x[64],y[64];         // input and output arrays for FFT
extern unsigned long DASoutput;  // output from the most recent iteration of DAS
extern unsigned long DataLost;   // data sent, but not received by Consumer
extern unsigned long MaxJitter;  // largest jitter between interrupts (x0.1 us)

// from STMicroelectronics
void cr4_fft_64_stm32(void *pssOUT, void *pssIN, unsigned short Nbin);
short PID_stm32(short Error, const short *Coeff);
extern short IntTerm;
extern short PrevError;

//************SW1Push*************
// Called when SW1 Button pushed
// Adds another foreground task
// background threads execute once and return
void SW1Push(void);

//************SW2Push*************
// Called when SW2 Button pushed, Lab 3 only
// Adds another foreground task
// background threads execute once and return
void SW2Push(void);

//************ Interpreter *************
// Foreground thread, I/O to/from serial port
// add the following commands, leave other commands, if they make sense
// 1) print performance measures
//     time-jitter, number of data points lost, number of calculations performed
//     i.e., NumSamples, NumCreated, MaxJitter, DataLost, FilterWork, PIDwork
// 2) print debugging parameters
//     i.e., x[], y[]
// inputs:  none
// outputs: none
void Interpreter(void);

//*******************final user main DEMONTRATE THIS TO TA**********
int realmain(void);

//******************* Lab 3 Preparation 2**********
// Modify this so it runs with your RTOS (i.e., fix the time units to match your OS)
// run this with
// UART0, 115200 baud rate, used to output results
// SYSTICK interrupts, period established by OS_Launch
// first timer interrupts, period established by first call to OS_AddPeriodicThread
// second timer interrupts, period established by second call to OS_AddPeriodicThread
// SW1 no interrupts
// SW2 no interrupts

#define counts1us 10             // OS_Time update frequency in MHz
#define workA 500                // number of PseudoWork iterations in TaskA
#define workB 250                // number of PseudoWork iterations in TaskB

extern unsigned long CountA;       // number of times Task A called
extern unsigned long CountB;       // number of times Task B called
extern unsigned long Count6;       // number of times thread6 loops

void Jitter(void);       // prints jitter information (TODO: write this)
int Testmain5(void);

//******************* Lab 3 Preparation 4**********
// Modify this so it runs with your RTOS used to test blocking semaphores
// run this with
// UART0, 115200 baud rate,  used to output results
// SYSTICK interrupts, period established by OS_Launch
// first timer interrupts, period established by first call to OS_AddPeriodicThread
// second timer interrupts, period established by second call to OS_AddPeriodicThread
// SW1 no interrupts,
// SW2 no interrupts

#define MAXCOUNT 20000           // total number of Signal() and Wait() calls

int Testmain6(void);

//******************* Lab 3 Measurement of context switch time**********
// Run this to measure the time it takes to perform a task switch
// UART0 not needed
// SYSTICK interrupts, period established by OS_Launch
// first timer not needed
// second timer not needed
// SW1 not needed,
// SW2 not needed
// logic analyzer on PF1 for systick interrupt (in your OS)
//                              on PE0 to measure context switch time

int Testmain7(void);

#endif // __LAB3_H__
