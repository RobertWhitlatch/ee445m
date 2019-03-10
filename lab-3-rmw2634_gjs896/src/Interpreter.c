// Interpreter.c

#include "Commands.h"
#include "CustomDefinitions.h"
#include "Lab3.h"
#include "OS.h"
#include "String_Ops.h"

#define BUFF_LENGTH 50
#define msg_test "msg 2 2 \"Hello, World!\" 789 1"
#define msg_strlength 29
#define adc_test "adc 4"
#define adc_strlength 5
#define time_test "time"
#define time_strlength 4
#define stats_test "stats"
#define stats_strlength 5
#define debug_test "debug"
#define debug_strlength 5
#define count_test "count"
#define count_strlength 5

void Interpreter(void){
    char in_char = (char)EOF;
    char input_buff[BUFF_LENGTH];
    int input_strlength = -1;
    fprintf(uart, "\033[2J\033[1;1H");
    fprintf(uart, "Welcome to XeroOS!\n>");
    while(1){
        if((in_char = fgetc(uart)) != (char)EOF){
            fprintf(uart, "%c\n", in_char);
            switch(in_char){
                case 'a':
                    fprintf(uart, "Executing ADC test command\n");
                    processCommand(adc_test, adc_strlength);
                    break;
                case 'b':
                    fprintf(uart, "Executing Count test command\n");
                    processCommand(count_test, count_strlength);
                    break;
                case 'c':
                case 'C':
                    fprintf(uart, "Enter Command\n");
                    fprintf(uart,">");
                    while((input_strlength = getString(input_buff,BUFF_LENGTH)) == EOF);
                    fprintf(uart,"\n");
                    processCommand(input_buff, input_strlength);
                    break;
                case 'd':
                    fprintf(uart, "Executing debug test command\n");
                    processCommand(debug_test, debug_strlength);
                    break;
                case 'h':
                case 'H':
                    fprintf(uart, "a - adc Test\n");
                    fprintf(uart, "b - count Test\n");
                    fprintf(uart, "c - Enter Command\n");
                    fprintf(uart, "d - debug Test\n");
                    fprintf(uart, "h - Help Menu\n");
                    fprintf(uart, "i - Information Page\n");
                    fprintf(uart, "m - msg Test\n");
                    fprintf(uart, "s - stats Test\n");
                    fprintf(uart, "t - time Test\n");
                    break;
                case 'i':
                    fprintf(uart, "Core Freqency - 80MHz\n");
                    if(CheckInterrupts()){
                        fprintf(uart, "Interrupts - Disabled\n");
                    }else{
                        fprintf(uart, "Interrupts - Enabled\n");
                    }
                    fprintf(uart, "FIFO Depth - %d\n", FIFO_SIZE);
                    fprintf(uart, "Time Slice - %d cycles\n", TIMESLICE);
                    break;
                case 'm':
                    fprintf(uart, "Executing msg test command\n");
                    processCommand(msg_test, msg_strlength);
                    break;
                case 's':
                    fprintf(uart, "Executing stats test command\n");
                    processCommand(stats_test, stats_strlength);
                    break;
                case 't':
                    fprintf(uart, "Executing time test command\n");
                    processCommand(time_test, time_strlength);
                    break;
                default:
                    break;
            }
            fprintf(uart, ">");
        }
    }
}
