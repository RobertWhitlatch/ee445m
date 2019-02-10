#include "CustomDefinitions.h"
#include <stdlib.h>
#include <string.h>
#include "Commands.h"
#include "ST7735.h"
#include "ADC0.h"

char* argv[8] = {0,0,0,0,0,0,0,0};
int argc = 1;
#define NUM_CMDS 2
const char* commands[NUM_CMDS] = {"msg", "adc"};
void (*cmd_ptr[NUM_CMDS])(void) = {msg_interface,adc_interface};

void prepareCommand(char* cmd, int strlength){
    for(int i = 0; i < strlength; i++){
        if(cmd[i] == ' '){
            cmd[i] = 0;
            argv[argc++] = (cmd+i+1);
        }
    }
}


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
    fprintf(uart,"Command not recognized");
    return;
}

void msg_interface(void){
    int device = atoi(argv[1]);
    int line = atoi(argv[2]);
    int value = atoi(argv[4]);
    int value_used = atoi(argv[5]);
    ST7735_Message(device,line,argv[2],value,value_used);
    fprintf(uart,"Message printed to ST7735\n");
}

void adc_interface(void){
    int channel = atoi(argv[1]);
    ADC0_Open(channel);
    int value = ADC0_In();
    fprintf(uart,"Value obtained from Channel %d is %d",channel, value);

}
