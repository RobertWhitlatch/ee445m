// UART.c
// Runs on LM4F120, TM4C123
// Simple device driver for the UART.
// Daniel Valvano
// May 5, 2015

/* This example accompanies the books
  "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
  ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

"Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
 
 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include <stdio.h>
#include <stdint.h>
#include "UART.h"
#include "FiFo.h"
#include "CustomDefinitions.h"

#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable

#define FIFOSIZE   16         // size of the FIFOs (must be power of 2)
#define FIFOSUCCESS 1         // return value on success
#define FIFOFAIL    0         // return value on failure

AddIndexFifo(Rx, FIFOSIZE, char, FIFOSUCCESS, FIFOFAIL)
AddIndexFifo(Tx, FIFOSIZE, char, FIFOSUCCESS, FIFOFAIL)

//------------UART_Init------------
// Initialize the UART for 115,200 baud rate (assuming 16 MHz bus clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled, interrupting
// Input: none
// Output: none
void UART_Init(void){
    SYSCTL_RCGCUART_R |= 0x01;            // activate UART0
    SYSCTL_RCGCGPIO_R |= 0x01;            // activate port A
    RxFifo_Init();                        // initialize empty FIFOs
    TxFifo_Init();
    UART0_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
    UART0_CC_R = UART_CC_CS_PIOSC;        // Use 16Mhz PIOSC Clock;
    UART0_IBRD_R = 8;                     // IBRD = int(50,000,000 / (16 * 115,200)) = int(27.1267)
    UART0_FBRD_R = 44;                    // FBRD = int(0.1267 * 64 + 0.5) = 8
                                          // 8 bit word length (no parity bits, one stop bit, FIFOs)
    UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
    UART0_IFLS_R &= ~0x3F;                // clear TX and RX interrupt FIFO level fields
                                          // configure interrupt for TX FIFO <= 1/8 full
                                          // configure interrupt for RX FIFO >= 1/8 full
    UART0_IFLS_R += (UART_IFLS_TX1_8|UART_IFLS_RX1_8);
                                          // enable TX and RX FIFO interrupts and RX time-out interrupt
    UART0_IM_R |= (UART_IM_RXIM|UART_IM_TXIM|UART_IM_RTIM);
    UART0_CTL_R |= 0x301;                 // enable UART
    GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1-0
    GPIO_PORTA_DEN_R |= 0x03;             // enable digital I/O on PA1-0
                                          // configure PA1-0 as UART
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
    GPIO_PORTA_AMSEL_R = 0;               // disable analog functionality on PA
                                          // UART0=priority 2
    NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFF00FF)|0x00004000; // bits 13-15
    NVIC_EN0_R = NVIC_EN0_INT5;           // enable interrupt 5 in NVIC
}

// copy from hardware RX FIFO to software RX FIFO
// stop when hardware RX FIFO is empty or software RX FIFO is full
void static copyHardwareToSoftware(void){
    char letter;
    while(((UART0_FR_R&UART_FR_RXFE) == 0) && (RxFifo_Size() < (FIFOSIZE - 1))){
        letter = UART0_DR_R;
        RxFifo_Put(letter);
    }
}

// copy from software TX FIFO to hardware TX FIFO
// stop when software TX FIFO is empty or hardware TX FIFO is full
void static copySoftwareToHardware(void){
    char letter;
    while(((UART0_FR_R&UART_FR_TXFF) == 0) && (TxFifo_Size() > 0)){
        TxFifo_Get(&letter);
        UART0_DR_R = letter;
    }
}

//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
char UART_InChar(void){
    while((UART0_FR_R&UART_FR_RXFE) != 0);
    return((char)(UART0_DR_R&0xFF));
}
//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(char data){
    while((UART0_FR_R&UART_FR_TXFF) != 0);
    UART0_DR_R = data;
}

void UART0_Handler(void){
    if(UART0_RIS_R&UART_RIS_TXRIS){       // hardware TX FIFO <= 2 items
        UART0_ICR_R = UART_ICR_TXIC;        // acknowledge TX FIFO
        copySoftwareToHardware();           // copy from software TX FIFO to hardware TX FIFO
        if(TxFifo_Size() == 0){             // software TX FIFO is empty
            UART0_IM_R &= ~UART_IM_TXIM;      // disable TX FIFO interrupt
        }
    }
    if(UART0_RIS_R&UART_RIS_RXRIS){       // hardware RX FIFO >= 2 items
        UART0_ICR_R = UART_ICR_RXIC;        // acknowledge RX FIFO
        copyHardwareToSoftware();        // copy from hardware RX FIFO to software RX FIFO
    }
    if(UART0_RIS_R&UART_RIS_RTRIS){       // receiver timed out
        UART0_ICR_R = UART_ICR_RTIC;        // acknowledge receiver time out
        copyHardwareToSoftware();  // copy from hardware RX FIFO to software RX FIFO
    }
}
