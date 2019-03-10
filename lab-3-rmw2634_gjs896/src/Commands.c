#include <string.h> // strcmp()
#include "ADC.h"
#include "Commands.h"
#include "CustomDefinitions.h"
#include "OS.h"
#include "ST7735.h"
#include "String_Ops.h"
#include "Lab3.h"

static void adc_interface(void);
static void msg_interface(void);
static void time_interface(void);
static void stats_interface(void);
static void debug_interface(void);
static void count_interface(void);

char* argv[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int argc = 1;

#define NUM_CMDS 6
static const char* const commands[NUM_CMDS] = {"msg", "adc", "time", "stats", "debug", "count"};
static void (*const cmd_ptr[NUM_CMDS])(void) = {msg_interface, adc_interface,
                                   time_interface, stats_interface,
                                   debug_interface, count_interface};

// -------------prepareCommand -----------------
// Prepares command string to be processes
// Input: Command string, and its length
// Output: None
void prepareCommand(char* cmd, int strlength) {
  int flag = 0;
  argv[0] = cmd;
  for (int i = 0; i < strlength; i++) {
    if (flag) {
      flag = 0;
      while (cmd[i] != '"' && i < strlength) {
        i++;
      }
      cmd[i++] = 0;
    }
    if (cmd[i] == ' ' || cmd[i] == 0) {
      cmd[i] = 0;
      if (cmd[i + 1] == '"') {
        cmd[++i] = 0;
        flag = 1;
      }
      argv[argc++] = (cmd + i + 1);
    }
  }
}

// -------------processCommand------------------
// Processes and executes entered command
// Input: Command string, and its length
// Output: None
void processCommand(char* cmd, int strlength) {
  prepareCommand(cmd, strlength);
  for (int i = 0; i < NUM_CMDS; i++) {
    if (strcmp(cmd, commands[i]) == 0) {
      cmd_ptr[i]();
      for (int i = 0; i < 8; i++) {
        argv[i] = 0;
      }
      argc = 1;
      return;
    }
  }
  for (int i = 0; i < 8; i++) {
    argv[i] = 0;
  }
  argc = 1;
  fprintf(uart, "Command not recognized.\n");
  return;
}

//---------msg_interface---------
// Execute ST7735_message per arguments
// Inputs: argc, *argc[]
// Output: None
void msg_interface(void) {
  int device = atoi(argv[1]);
  int line = atoi(argv[2]);
  int value = atoi(argv[4]);
  int value_used = atoi(argv[5]);
  ST7735_Message(device, line, argv[3], value, value_used);
  fprintf(uart, "Message printed to ST7735.\n");
}

//---------adc_interface---------
// Execute adc conversion per arguments
// Inputs: argc, *argc[]
// Output: None
void adc_interface(void) {
  int channel = atoi(argv[1]);
  ADC_Open(channel);
  int value = ADC_In();
  fprintf(uart, "ADC Channel %d = %d\n", channel, value);
}

//---------time_interface---------
// Execute OS time read per arguments
// Inputs: argc, *argc[]
// Output: None
void time_interface(void) {
  unsigned long result = OS_Time();
  fprintf(uart, "XeroOS Time = %lu\n",result);
}

//---------stats_interface---------
// Execute status print per arguments
// Inputs: argc, *argc[]
// Output: None
// 1) print performance measures
//    time-jitter, number of data points lost, number of calculations performed
//    i.e., NumSamples, NumCreated, MaxJitter, DataLost, FilterWork, PIDwork
extern unsigned long NumSamples;  // incremented every ADC sample, in Producer
extern unsigned long NumCreated;  // number of foreground threads created
extern unsigned long DataLost;    // data sent by Producer, but not received by Consumer
extern unsigned long MaxJitter;   // largest time jitter between interrupts in usec
extern unsigned long FilterWork;  // number of digital filter calculations finished
extern unsigned long PIDWork;     // current number of PID calculations finished
void stats_interface(void) {
    fprintf(uart,"NumSamples = %lu\n",NumSamples);
    fprintf(uart,"NumCreated = %lu\n",NumCreated);
    fprintf(uart,"DataLost = %lu\n",DataLost);
    fprintf(uart,"MaxJitter = %lu\n",MaxJitter);
    fprintf(uart,"FilterWork = %lu\n",FilterWork);
    fprintf(uart,"PIDWork = %lu\n",PIDWork);
}

void count_interface(void){
    fprintf(uart, "CountA = %lu\n", CountA);
    fprintf(uart, "CountB = %lu\n", CountB);
    fprintf(uart, "Count6 = %lu\n", Count6);
}

//---------debug_interface---------
// Execute debug print per arguments
// Inputs: argc, *argc[]
// Output: None
// 2) print debugging parameters
//    i.e., x[], y[]

extern long x[64];                // input array for FFT
extern long y[64];                // output array for FFT
void debug_interface(void) {
    for(int i = 0; i < 16; i++){
        fprintf(uart, "(x[%2d],y[%2d]) = (%ld,%ld)\t",i,i,x[i],y[i]);
        fprintf(uart, "(x[%2d],y[%2d]) = (%ld,%ld)\t",i+16,i+16,x[i+16],y[i+16]);
        fprintf(uart, "(x[%2d],y[%2d]) = (%ld,%ld)\t",i+32,i+32,x[i+32],y[i+32]);
        fprintf(uart, "(x[%2d],y[%2d]) = (%ld,%ld)\n",i+48,i+48,x[i+48],y[i+48]);
    }
}
