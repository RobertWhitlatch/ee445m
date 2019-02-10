// Significant amounts of code have been adapted from ValvanoWare

#include "CustomDefinitions.h"
#include "ST7735.h"
#include "PLL.h"
#include "UART.h"
#include "ADC0.h"
#include "OS.h"
#include "String_Ops.h"
#include "Commands.h"

#define BUFF_LENGTH 50
char input_buff[BUFF_LENGTH];
char in_char;
int strlength = 0;

void dummy(){
    // 59 cycle or 737.5 nanosecs to execute empty ISR
    static int count = 0;
    fprintf(lcd,"%d\n",count++);
}

int main(void){

    DisableInterrupts();
    PLL_Init(Bus80MHz);
    Output_Init();
    //OS_AddPeriodicThread(&dummy,1000000, 4);
    
    EnableInterrupts();
    fprintf(lcd,"Hello, World!");
    fprintf(uart_cmd,"Hello, World!\n>");

    while(1){

        if((in_char = fgetc(uart_cmd)) > -1){
            fprintf(uart_cmd, "%c\n", in_char);
            switch(in_char){
                case 'c':
                case 'C':
                    fprintf(uart_cmd, "Enter Command\n");
                    while((strlength = getString(input_buff,BUFF_LENGTH)) == -1);
                    processCommand(input_buff, strlength);
                    break;
                case 'h':
                case 'H':
                    fprintf(uart_cmd, "c - Enter Command\n");
                    fprintf(uart_cmd, "h - Help Menu\n");
                    fprintf(uart_cmd, "i - Information Page\n");
                    fprintf(uart_cmd, "I - Toggle Interrupts\n");
                    
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
                default:
                    break;
            }
            fprintf(uart_cmd, ">");
        }
    }

}
