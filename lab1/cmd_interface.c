#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ST7735.h"
#include "PLL.h"
#include "UART.h"
#include "Output.h"
#include "CustomDefinitions.h"
#include "ADC0.h"
#include "OS.h"

int CMD_Message(int argc, char* argv[]){

    int device = *argv[0]-0x30;
    argc--;
    int line = *argv[1]-0x30;
    argc--;
    char* string = argv[2];
    argc--;
    int num, use;
    if(argc == 0){
        num = 0;
        use = 0;
    }else{
        num = atoi(argv[3]);
        use = 1;
    }
    ST7735_Message(device,line,string,num,use);
    return(0);
}
