#include "CustomDefinitions.h"
#include "Output.h"
#include "UART.h"
#include "ST7735.h"

FILE* uart = (FILE*)0;
FILE* uart_cmd = (FILE*)1;
FILE* lcd  = (FILE*)2;
FILE* lcd_os = (FILE*)3;

int lcd_count = 0;

// Print a character to a display
int fputc(int ch, FILE *f){

    if(f == uart){
        if((ch == 10) || (ch == 13) || (ch == 27)){
            UART_OutChar(13);
            UART_OutChar(10);
            return 1;
        }
        UART_OutChar(ch);
        return 1;
    }else if(f == lcd){
        if(lcd_count >= 20){
            ST7735_OutChar('\n');
            lcd_count = 0;
        }
        ST7735_OutChar(ch);
        if((ch == 10) || (ch == 13) || (ch == 27)){
            lcd_count = 0;
        }else{
            lcd_count++;
        }
        return(1);
    }else if(f == lcd_os){
        ST7735_OutChar(ch);
        return(1);
    }else if(f == uart_cmd){
        if((ch == 10) || (ch == 13) || (ch == 27)){
            UART_OutChar(13);
            UART_OutChar(10);
            return 1;
        }
        UART_OutChar(ch);
        return 1;
    }
    return(EOF);
}

// Get input from UART, echo
int fgetc (FILE *f){

    char ch = EOF;
    if(f == uart){
        ch = UART_InChar();
        UART_OutChar(ch);
        return(ch);
    }else if(f == uart_cmd){
        if(UART0_FR_R&0x00000010){
            ch = UART_InChar();
        }
        return(ch);
    }else if(f == lcd){
        return(ch);
    }else if(f == lcd_os){
        return(ch);
    }
    return(ch);

}
// Function called when file error occurs.
int ferror(FILE *f){

    if(f == uart){
        return(EOF);
    }else if(f == uart_cmd){
        return(EOF);
    }else if(f == lcd){
        return(EOF);
    }else if(f == lcd_os){
        return(EOF);
    }
    return EOF;

}

// Clear display
void Output_Clear(void){

}

// Turn off display (low power)
void Output_Off(void){

}

// Turn on display
void Output_On(void){

}

// set the color for future output
void Output_Color(uint32_t newColor){
    ST7735_SetTextColor(newColor);
}

//------------Output_Init------------
// Initialize the UART for 115,200 baud rate (assuming 16 MHz bus clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void Output_Init(void){

    UART_Init();
    ST7735_InitR(INITR_REDTAB);
    ST7735_FillScreen(0);
    ST7735_SetRotation(2);

}
