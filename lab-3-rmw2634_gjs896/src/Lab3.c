// Lab3.c

#include <string.h>

#include "inc/tm4c123gh6pm.h"

#include "OS.h"
#include "ST7735.h"
#include "ADC.h"
#include "GPIO.h"
#include "Lab3.h"

// // // // // realmain: DAS workload // // // // //

unsigned long NumCreated;   // number of foreground threads created
unsigned long PIDWork;      // current number of PID calculations finished
unsigned long FilterWork;   // number of digital filter calculations finished
unsigned long NumSamples;   // incremented every ADC sample, in Producer
long x[64],y[64];           // input and output arrays for FFT
unsigned long DataLost;     // data sent, but not received by Consumer
unsigned long MaxJitter;    // largest time jitter between interrupts

#if Lab2
static unsigned long JitterHistogram[JITTERSIZE]={0,};
#endif

//------------------Task 1--------------------------------
// 2 kHz sampling ADC channel 1, using software start trigger
// background thread executed at 2 kHz

//*********** Filter *************
// 60-Hz notch high-Q, IIR filter, assuming fs=2000 Hz
// y(n) = (256x(n) -503x(n-1) + 256x(n-2) + 498y(n-1)-251y(n-2))/256 (2k sampling)
// y(n) = (256x(n) -476x(n-1) + 256x(n-2) + 471y(n-1)-251y(n-2))/256 (1k sampling)
static long Filter(long data){
static long x[6]; // this MACQ needs twice
static long y[6];
static unsigned long n=3;       // 3, 4, or 5
    n++;
    if(n==6) n=3;
    x[n] = x[n-3] = data;    // two copies of new data
    y[n] = (256*(x[n]+x[n-2])-503*x[n-1]+498*y[n-1]-251*y[n-2]+128)/256;
    y[n-3] = y[n];               // two copies of filter outputs too
    return y[n];
}

//******** DAS ***************
// background thread, calculates 60Hz notch filter
// runs 2000 times/sec
// samples channel 4, PD3,
// inputs:  none
// outputs: none
static void DAS(void){
    unsigned long input;
#if Lab2
    unsigned static long LastTime;  // time at previous ADC sample
    unsigned long thisTime;                 // time at current ADC sample
    long jitter;                                        // time between measured and expected, in us
#endif
    static unsigned long DASoutput;
    if(NumSamples < RUNLENGTH){     // finite time run
        PE0 ^= 0x01;
        input = ADC_In();                       // channel set when calling ADC_Init
        PE0 ^= 0x01;
        DASoutput = Filter(input);
        FilterWork++;                // calculation finished
#if Lab2
        thisTime = OS_Time();               // current time, 12.5 ns
        if(FilterWork>1){        // ignore timing of first interrupt
            unsigned long diff = OS_TimeDifference(LastTime,thisTime);
            if(diff>PERIOD){
                jitter = (diff-PERIOD+4)/8;  // in 0.1 usec
            }else{
                jitter = (PERIOD-diff+4)/8;  // in 0.1 usec
            }
            if(jitter > MaxJitter){
                MaxJitter = jitter; // in usec
            }               // jitter should be 0
            if(jitter >= JITTERSIZE){
                jitter = JITTERSIZE-1;
            }
            JitterHistogram[jitter]++;
        }
        LastTime = thisTime;
#endif
        PE0 ^= 0x01;
    }
}

//------------------Task 2--------------------------------
// background thread executes with SW1 button
// one foreground task created with button push
// foreground treads run for 2 sec and die

//***********ButtonWork*************
static void ButtonWork(void){
    unsigned long myId = OS_Id();
    PE1 ^= 0x02;
    ST7735_Message(1,0,"NumCreated =",NumCreated,1);
    PE1 ^= 0x02;
    OS_Sleep(50);           // set this to sleep for 50msec
    ST7735_Message(1,1,"PIDWork         =",PIDWork,1);
    ST7735_Message(1,2,"DataLost        =",DataLost,1);
    ST7735_Message(1,3,"Jitter 0.1us=",MaxJitter,1);
    PE1 ^= 0x02;
    OS_Kill();  // done, OS does not return from a Kill
}

void SW1Push(void){
    if(OS_MsTime() > 20){ // debounce
        if(OS_AddThread(&ButtonWork,100,2)){
            NumCreated++;
        }
        OS_ClearMsTime();    // at least 20ms between touches
    }
}

void SW2Push(void){
    if(OS_MsTime() > 20){ // debounce
        if(OS_AddThread(&ButtonWork,100,2)){
            NumCreated++;
        }
        OS_ClearMsTime();    // at least 20ms between touches
    }
}

//------------------Task 3--------------------------------
// hardware timer-triggered ADC sampling at 400Hz
// Producer runs as part of ADC ISR
// Producer uses fifo to transmit 400 samples/sec to Consumer
// every 64 samples, Consumer calculates FFT
// every 2.5ms*64 = 160 ms (6.25 Hz), consumer sends data to Display via mailbox
// Display thread updates LCD with measurement

//******** Producer ***************
// The Producer in this lab will be called from your ADC ISR
// A timer runs at 400Hz, started by your ADC_Collect
// The timer triggers the ADC, creating the 400Hz sampling
// Your ADC ISR runs when ADC data is ready
// Your ADC ISR calls this function with a 12-bit sample
// sends data to the consumer, runs periodically at 400Hz
// inputs:  none
// outputs: none
static void Producer(unsigned long data){
    if(NumSamples < RUNLENGTH){     // finite time run
        NumSamples++;                               // number of samples
        if(OS_Fifo_Put(data) == 0){ // send to consumer
            DataLost++;
        }
    }
}

//******** Display ***************
// foreground thread, accepts data from consumer
// displays calculated results on the LCD
// inputs:  none
// outputs: none
static void Display(void){
    unsigned long data,voltage;
    ST7735_Message(0,1,"Run length = ",(RUNLENGTH)/FS,1);     // top half used for Display
    while(NumSamples < RUNLENGTH) {
        data = OS_MailBox_Recv();
        voltage = 3000*data/4095;                               // calibrate your device so voltage is in mV
        PE3 = 0x08;
        ST7735_Message(0,2,"v(mV) =",voltage,1);
        PE3 = 0x00;
    }
    OS_Kill();  // done
}

//******** Consumer ***************
// foreground thread, accepts data from producer
// calculates FFT, sends DC component to Display
// inputs:  none
// outputs: none
static void Consumer(void){
    unsigned long data,DCcomponent;     // 12-bit raw ADC sample, 0 to 4095
    unsigned long t;                                    // time in 2.5 ms
    unsigned long myId = OS_Id();
    ADC_Collect(5, FS, &Producer); // start ADC sampling, channel 5, PD2, 400 Hz
    NumCreated += OS_AddThread(&Display,128,0);
    while(NumSamples < RUNLENGTH) {
        PE2 = 0x04;
        for(t = 0; t < 64; t++){     // collect 64 ADC samples
            data = OS_Fifo_Get();        // get from producer
            x[t] = data;                         // real part is 0 to 4095, imaginary part is 0
        }
        PE2 = 0x00;
        cr4_fft_64_stm32(y,x,64);    // complex FFT of last 64 ADC values
        DCcomponent = y[0]&0xFFFF; // Real part at frequency 0, imaginary part should be zero
        OS_MailBox_Send(DCcomponent); // called every 2.5ms*64 = 160ms
    }
    OS_Kill();  // done
}

//------------------Task 4--------------------------------
// foreground thread that runs without waiting or sleeping
// it executes a digital controller

//******** PID ***************
// foreground thread, runs a PID controller
// never blocks, never sleeps, never dies
// inputs:  none
// outputs: none
short IntTerm;            // accumulated error, RPM-sec
short PrevError;          // previous error, RPM
const short Coeff[3] = {384, 128, 64};

static void PID(void){
    static short Actuator;
    short err;  // speed error, range -100 to 100 RPM
    unsigned long myId = OS_Id();
    PIDWork = 0;
    IntTerm = 0;
    PrevError = 0;
    while(NumSamples < RUNLENGTH) {
        for(err = -1000; err <= 1000; err++){        // made-up data
            Actuator = PID_stm32(err,Coeff)/256;
        }
        PIDWork++;              // calculation finished
    }
    for(;;){ }                  // done
}

//------------------Task 5--------------------------------
// UART background ISR performs serial input/output
// Two software fifos are used to pass I/O data to foreground
// The interpreter runs as a foreground thread

//*******************final user main DEMONTRATE THIS TO TA**********
int realmain(void){
    OS_Init();                   // initialize, disable interrupts
    PortE_Init();
    DataLost = 0;                // lost data between producer and consumer
    NumSamples = 0;
    MaxJitter = 0;           // in 1us units

//********initialize communication channels
    OS_MailBox_Init();
    OS_Fifo_Init(FIFO_SIZE);      // ***note*** 4 is not big enough*****

//*******attach background tasks***********
    OS_AddSW1Task(&SW1Push,2);
#if Lab3
    OS_AddSW2Task(&SW2Push,2);  // add this line in Lab 3
#endif
    ADC_Open(4);    // sequencer 3, channel 4, PD3, sampling in DAS()
    OS_AddPeriodicThread(&DAS,PERIOD,1); // 2 kHz real time sampling of PD3

    NumCreated = 0 ;
// create initial foreground threads
    NumCreated += OS_AddThread(&Interpreter,128,2);
    NumCreated += OS_AddThread(&Consumer,128,1);
    NumCreated += OS_AddThread(&PID,128,3);  // Lab 3, make this lowest priority

    OS_Launch(TIMESLICE); // doesn't return, interrupts enabled in here
    return 0;                        // this never executes
}

// // // // // Testmain5: basic scheduler test // // // // //

unsigned long CountA;       // number of times Task A called
unsigned long CountB;       // number of times Task B called
unsigned long Count6;       // number of times thread6 loops

//*******PseudoWork*************
// simple time delay, simulates user program doing real work
// Input: amount of work in 100ns units (free free to change units
// Output: none
static void PseudoWork(unsigned long work){
unsigned long startTime;
    startTime = OS_Time();      // time in 100ns units
    while(OS_TimeDifference(startTime,OS_Time()) <= work){}
}

static void Thread6(void){  // foreground thread
    Count6 = 0;
    for(;;){
        Count6++;
        PE0 ^= 0x01;                // debugging toggle bit 0
    }
}

static void Thread7(void){  // foreground thread
    fprintf(uart,"\nEE345M/EE380L, Lab 3 Preparation 2\n");
    OS_Sleep(5000);     // 10 seconds
    Jitter();                   // print jitter information
    fprintf(uart,"\n\nCountA=%lu\nCountB=%lu\nCount6=%lu\n",CountA,CountB,Count6);
    OS_Kill();
}

static void TaskA(void){               // called every {1000, 2990us} in background
    PE1 = 0x02;          // debugging profile
    CountA++;
    PseudoWork(workA*counts1us); //  do work (100ns time resolution)
    PE1 = 0x00;          // debugging profile
}

static void TaskB(void){               // called every pB in background
    PE2 = 0x04;          // debugging profile
    CountB++;
    PseudoWork(workB*counts1us); //  do work (100ns time resolution)
    PE2 = 0x00;          // debugging profile
}

int Testmain5(void){             // Testmain5 Lab 3
    PortE_Init();
    OS_Init();                   // initialize, disable interrupts
    NumCreated = 0 ;
    NumCreated += OS_AddThread(&Thread6,128,2);
    NumCreated += OS_AddThread(&Thread7,128,1);
    OS_AddPeriodicThread(&TaskA,TIME_1MS,0);                     // 1 ms, higher priority
    OS_AddPeriodicThread(&TaskB,2*TIME_1MS,1);               // 2 ms, lower priority

    OS_Launch(TIME_2MS); // 2ms, doesn't return, interrupts enabled in here
    return 0;                           // this never executes
}

// // // // // Testmain6: blocking semaphore test // // // // //

static Sema4Type s;
static unsigned long SignalCount1; // number of times s is signaled
static unsigned long SignalCount2; // number of times s is signaled
static unsigned long SignalCount3; // number of times s is signaled
static unsigned long WaitCount1; // number of times s is successfully waited on
static unsigned long WaitCount2; // number of times s is successfully waited on
static unsigned long WaitCount3; // number of times s is successfully waited on

static void OutputThread(void){    // foreground thread
    fprintf(uart,"\n\rEE445M/EE380L, Lab 3 Preparation 4\n\r");
    while(SignalCount1+SignalCount2+SignalCount3<100*MAXCOUNT){
        OS_Sleep(1000);     // 1 second
        fprintf(uart,".");
    }
    fprintf(uart," done\nSignalled=%lu, Waited=%lu\n",SignalCount1+SignalCount2+SignalCount3,WaitCount1+WaitCount2+WaitCount3);
    OS_Kill();
}

static void Wait1(void){    // foreground thread
    for(;;){
        OS_Wait(&s);        // three threads waiting
        WaitCount1++;
    }
}

static void Wait2(void){    // foreground thread
    for(;;){
        OS_Wait(&s);        // three threads waiting
        WaitCount2++;
    }
}

static void Wait3(void){       // foreground thread
    for(;;){
        OS_Wait(&s);        // three threads waiting
        WaitCount3++;
    }
}

static void Signal1(void){          // called every 799us in background
    if(SignalCount1<MAXCOUNT){
        OS_Signal(&s);
        SignalCount1++;
    }
}

static void Signal2(void){             // called every 1111us in background
    if(SignalCount2<MAXCOUNT){
        // TODO: edit this so it changes the periodic rate
        OS_Signal(&s);
        SignalCount2++;
    }
}

static void Signal3(void){             // foreground
    while(SignalCount3<98*MAXCOUNT){
        OS_Signal(&s);
        SignalCount3++;
    }
    OS_Kill();
}

static long add(const long n, const long m){
static long result;
    result = m+n;
    return result;
}

int Testmain6(void){            // Testmain6    Lab 3
    volatile unsigned long delay;
    OS_Init();                   // initialize, disable interrupts
    delay = add(3,4);
    PortE_Init();
    SignalCount1 = 0;       // number of times s is signaled
    SignalCount2 = 0;       // number of times s is signaled
    SignalCount3 = 0;       // number of times s is signaled
    WaitCount1 = 0;         // number of times s is successfully waited on
    WaitCount2 = 0;         // number of times s is successfully waited on
    WaitCount3 = 0;     // number of times s is successfully waited on
    OS_InitSemaphore(&s,0);  // this is the test semaphore
    OS_AddPeriodicThread(&Signal1,(799*TIME_1MS)/1000,0);       // 0.799 ms, higher priority
    OS_AddPeriodicThread(&Signal2,(1111*TIME_1MS)/1000,1);  // 1.111 ms, lower priority
    NumCreated = 0 ;
    NumCreated += OS_AddThread(&Thread6,128,6);         // idle thread to keep from crashing
    NumCreated += OS_AddThread(&OutputThread,128,2);    // results output thread
    NumCreated += OS_AddThread(&Signal3,128,2);     // signalling thread
    NumCreated += OS_AddThread(&Wait1,128,2);       // waiting thread
    NumCreated += OS_AddThread(&Wait2,128,2);       // waiting thread
    NumCreated += OS_AddThread(&Wait3,128,2);       // waiting thread

    OS_Launch(TIME_1MS);    // 1ms, doesn't return, interrupts enabled in here
    return 0;                           // this never executes
}

// // // // // Testmain7: context switch time measurement // // // // //

static void Thread8(void){             // only thread running
    while(1){
        PE0 ^= 0x01;            // debugging profile
    }
}

int Testmain7(void){             // Testmain7
    PortE_Init();
    OS_Init();                   // initialize, disable interrupts
    NumCreated = 0 ;
    NumCreated += OS_AddThread(&Thread8,128,2);
    OS_Launch(TIME_1MS/10); // 100us, doesn't return, interrupts enabled in here
    return 0;                           // this never executes
}

#if Lab2
// // // // // Testmain8: FIFO test // // // // //
// Count1 should exactly equal Count2
// Count3 should be very large
// Timer interrupts, with period established by OS_AddPeriodicThread

unsigned long OtherCount1;
unsigned long Expected8; // last data read+1
unsigned long Error8;
void ConsumerThread8(void){
  Count2 = 0;
  for(;;){
    OtherCount1 = OS_Fifo_Get();
    if(OtherCount1 != Expected8){
      Error8++;
    }
    Expected8 = OtherCount1+1; // should be sequential
    Count2++;
  }
}
void FillerThread8(void){
  Count3 = 0;
  for(;;){
    Count3++;
  }
}
void BackgroundThread8Producer(void){   // called periodically
  if(OS_Fifo_Put(Count1) == 0){ // send to consumer
    DataLost++;
  }
  Count1++;
}
int Testmain8(void){   // Testmain8
  Count1 = 0;     DataLost = 0;
  Expected8 = 0;  Error8 = 0;
  OS_Init();           // initialize, disable interrupts
  NumCreated = 0 ;
  OS_AddPeriodicThread(&BackgroundThread8Producer,PERIOD,0);
  OS_Fifo_Init(16);
  NumCreated += OS_AddThread(&ConsumerThread8,128,2);
  NumCreated += OS_AddThread(&FillerThread8,128,3);
  OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
  return 0;            // this never executes
}
#endif // Lab2
