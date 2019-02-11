#include "CustomDefinitions.h"
#include <stdlib.h>
#include <string.h>
#include "Commands.h"
#include "ST7735.h"
#include "ADC0.h"
#include "OS.h"

void adc_interface(void);
void msg_interface(void);
void time_interface(void);

char* argv[8] = {0,0,0,0,0,0,0,0};
int argc = 1;
#define NUM_CMDS 3
const char* commands[NUM_CMDS] = {"msg", "adc","time"};
void (*cmd_ptr[NUM_CMDS])(void) = {msg_interface,adc_interface,time_interface};


// -------------prepareCommand -----------------
// Prepares command string to be processes
// Input: Command string, and its length
// Output: None
void prepareCommand(char* cmd, int strlength){
    int flag = 0;
    argv[0] = cmd;
    for(int i = 0; i < strlength; i++){
        if(flag){
            flag = 0;
            while(cmd[i] != '"' && i < strlength){
                i++;
            }
            cmd[i++] = 0;
        }
        if(cmd[i] == ' '){
            cmd[i] = 0;
            if(cmd[i+1] == '"'){
                cmd[++i] = 0;
                flag = 1;
            }
            argv[argc++] = (cmd+i+1);
        }
    }
}

// -------------processCommand------------------
// Processes and executes entered command
// Input: Command string, and its length
// Output: None
void processCommand(char* cmd, int strlength){
    prepareCommand(cmd, strlength);
    for(int i = 0; i < NUM_CMDS; i++){
        if(strcmp(cmd, commands[i]) == 0){
            cmd_ptr[i]();
            for(int i = 0; i < 8; i++){
                argv[i] = 0;
            }
            argc = 1;
            return;
        }
    }
    for(int i = 0; i < 8; i++){
        argv[i] = 0;
    }
    argc = 1;
    fprintf(uart,"Command not recognized.\n");
    return;
}

//---------msg_interface---------
// Translate cmd string in the appropriate arguments to ST7735_message
// Inputs: argc, *argc[]
// Output: None
void msg_interface(void){
    int device = atoi(argv[1]);
    int line = atoi(argv[2]);
    int value = atoi(argv[4]);
    int value_used = atoi(argv[5]);
    ST7735_Message(device,line,argv[3],value,value_used);
    fprintf(uart,"Message printed to ST7735.\n");
}

//---------adc_interface---------
// Translate cmd string in the appropriate arguments to execute adc conversion
// Inputs: argc, *argc[]
// Output: None
void adc_interface(void){
    int channel = atoi(argv[1]);
    ADC0_Open(channel);
    int value = ADC0_In();
    ST7735_Message(0, 0, "ADC_Channel", channel, 1);
    fprintf(lcd_os, " = %d", value);
    fprintf(uart,"ADC Value printed to ST7735.\n");
}

//---------time_interface---------
// Translate cmd string in the appropriate arguments to execute OS time read
// Inputs: argc, *argc[]
// Output: None
void time_interface(void){
    int result = OS_ReadPeriodicTime();
    ST7735_Message(0, 0, "OS Time =", result, 1);
    fprintf(uart,"XeroOS time printed to ST7735.\n");
}
