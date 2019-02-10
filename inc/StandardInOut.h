#ifndef __STD_IN_OUT__
#define __STD_IN_OUT__

#include "tm4c123gh6pm.h"
#include <stdio.h>

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F

#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable


void STDIO_Init(void){
    SYSCTL_RCGCUART_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= 0x01;
    UART0_CTL_R &= ~UART_CTL_UARTEN;
    UART0_IBRD_R = 43;
    UART0_FBRD_R = 26;
    UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
    UART0_CTL_R |= UART_CTL_UARTEN;
    GPIO_PORTA_AFSEL_R |= 0x03;
    GPIO_PORTA_DEN_R |= 0x03;
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
    GPIO_PORTA_AMSEL_R &= ~0x03;
}

char UART_getc(void){
    while((UART0_FR_R&UART_FR_RXFE) != 0);
    return((char)(UART0_DR_R&0xFF));
}

void UART_putc(char data){
    while((UART0_FR_R&UART_FR_TXFF) != 0);
    UART0_DR_R = data;
}

int fputc(int ch, FILE *f){
    if((ch == 10) || (ch == 13) || (ch == 27)){
        UART_putc(13);
        UART_putc(10);
        return 1;
    }
    UART_putc(ch);
    return 1;
}

int fgetc (FILE *f){
    char ch = UART_getc();  // receive from keyboard
    UART_putc(ch);            // echo
    return ch;
}

int ferror(FILE *f){
    /* Your implementation of ferror */
    return EOF;
}

#endif
