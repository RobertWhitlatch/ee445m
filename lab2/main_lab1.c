// Significant amounts of code have been adapted from ValvanoWare

#include "CustomDefinitions.h"
#include "ST7735.h"
#include "PLL.h"
#include "UART.h"
#include "ADC.h"
#include "OS.h"
#include "String_Ops.h"
#include "Commands.h"

#define BUFF_LENGTH 50
char msg_test[BUFF_LENGTH] = "msg 0 0 \"Hello, World!\" 789 1";
char adc_test[BUFF_LENGTH] = "adc 0";
char time_test[BUFF_LENGTH] = "time";
char input_buff[BUFF_LENGTH];
int msg_strlength = 29;
int adc_strlength = 5;
int time_strlength = 4;
int strlength;
char in_char;


void dummy(){
    // 59 cycle or 737.5 nanosecs to execute empty ISR
    //static int count = 0;
    //fprintf(lcd,"%d\n",count++);
}

int main(void){

    DisableInterrupts();
    PLL_Init(Bus80MHz);
    Output_Init();
    //OS_AddPeriodicThread(&dummy, 1000000, 4);
    EnableInterrupts();
    fprintf(uart_cmd, "\033[2J\033[1;1H");
    fprintf(uart_cmd, "Welcome to XeroOS!\n>");
    fprintf(lcd_os, "Welcome to XeroOS!");

    while(1){
        if((in_char = fgetc(uart_cmd)) > -1){
            fprintf(uart_cmd, "%c\n", in_char);
            switch(in_char){
                case 'a':
                    fprintf(uart_cmd, "Executing ADC test command.\n");
                    processCommand(adc_test, adc_strlength);
                    break;
                case 'c':
                case 'C':
                    fprintf(uart_cmd, "Enter Command.\n>");
                    while((strlength = getString(input_buff,BUFF_LENGTH)) == -1);
                    fprintf(uart_cmd, "\n");
                    processCommand(input_buff, strlength);
                    break;
                case 'h':
                case 'H':
                    fprintf(uart_cmd, "a - adc Test\n");
                    fprintf(uart_cmd, "c - Enter Command\n");
                    fprintf(uart_cmd, "h - Help Menu\n");
                    fprintf(uart_cmd, "i - Information Page\n");
                    fprintf(uart_cmd, "I - Toggle Interrupts\n");
                    fprintf(uart_cmd, "m - msg Test\n");
                    fprintf(uart_cmd, "t - time Test\n");
                    break;
                case 'i':
                    fprintf(uart_cmd, "Core Freqency - 80MHz\n");
                    if(CheckInterrupts()){
                        fprintf(uart_cmd, "Interrupts - Disabled\n");
                    }else{
                        fprintf(uart_cmd, "Interrupts - Enabled\n");
                    }
                    break;
                case 'I':
                    if(CheckInterrupts()){
                        EnableInterrupts();
                        fprintf(uart_cmd, "Interrupts - Enabled\n");
                    }else{
                        DisableInterrupts();
                        fprintf(uart_cmd, "Interrupts - Disabled\n");
                    }
                    break;
                case 'm':
                    fprintf(uart_cmd, "Executing msg test command.\n");
                    processCommand(msg_test, msg_strlength);
                    break;
                case 't':
                    fprintf(uart_cmd, "Executing time test command.\n");
                    processCommand(time_test, time_strlength);
                    break;
                default:
                    break;
            }
            fprintf(uart_cmd, ">");
        }
    }

}
