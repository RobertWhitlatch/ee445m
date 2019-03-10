#include "Output.h"
#include "CustomDefinitions.h"
#include "OS.h"
#include "ST7735.h"
#include <stdint.h>
#include "FiFo.h"

#define NVIC_EN0_INT5           0x00000020  // Interrupt 5 enable
#define UART_FR_RXFF            0x00000040  // UART Receive FIFO Full
#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable
#define UART_IFLS_RX1_8         0x00000000  // RX FIFO >= 1/8 full
#define UART_IFLS_TX1_8         0x00000000  // TX FIFO <= 1/8 full
#define UART_IM_RTIM            0x00000040  // UART Receive Time-Out Interrupt Mask
#define UART_IM_TXIM            0x00000020  // UART Transmit Interrupt Mask
#define UART_IM_RXIM            0x00000010  // UART Receive Interrupt Mask
#define UART_RIS_RTRIS          0x00000040  // UART Receive Time-Out Raw Interrupt Status
#define UART_RIS_TXRIS          0x00000020  // UART Transmit Raw Interrupt Status
#define UART_RIS_RXRIS          0x00000010  // UART Receive Raw Interrupt Status
#define UART_ICR_RTIC           0x00000040  // Receive Time-Out Interrupt Clear
#define UART_ICR_TXIC           0x00000020  // Transmit Interrupt Clear
#define UART_ICR_RXIC           0x00000010  // Receive Interrupt Clear

FILE* const uart = (FILE*)3; // making this 0 --> null pointer
FILE* const lcd = (FILE*)1;
FILE* const lcd_os = (FILE*)2;

#define UART_FIFO_SIZE 64
#define UART_SUCCESS 1
#define UART_FAILURE 0
AddIndexFifo(Tx, UART_FIFO_SIZE, char, UART_SUCCESS, UART_FAILURE)
AddIndexFifo(Rx, UART_FIFO_SIZE, char, UART_SUCCESS, UART_FAILURE)

//------------UART_Init------------
// Initialize the UART for 115,200 baud rate (assuming 16 MHz bus clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled, interrupting
// Input: none
// Output: none
void UART_Init(void) {

  SYSCTL_RCGCUART_R |= 0x01;  // activate UART0
  SYSCTL_RCGCGPIO_R |= 0x01;  // activate port A

  TxFifo_Init();
  RxFifo_Init();

  UART0_CTL_R &= ~UART_CTL_UARTEN;
  UART0_CC_R = UART_CC_CS_PIOSC;    // Use 16Mhz PIOSC Clock;
  UART0_IBRD_R = 8;
  UART0_FBRD_R = 44;
  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART0_IFLS_R = (UART_IFLS_TX1_8|UART_IFLS_RX1_8);
  UART0_IM_R = (UART_IM_RXIM|UART_IM_TXIM|UART_IM_RTIM);
  UART0_CTL_R |= 0x301;

  GPIO_PORTA_AFSEL_R |= 0x03;
  GPIO_PORTA_DEN_R |= 0x03;
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) + 0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03;
  NVIC_PRI1_R = (NVIC_PRI1_R & 0xFFFF00FF) | 0x00000000;
  NVIC_EN0_R |= NVIC_EN0_INT5;

}

// copy from hardware RX FIFO to software RX FIFO
// stop when hardware RX FIFO is empty or software RX FIFO is full
void static copyHardwareToSoftware(void){
  char letter;
  while(((UART0_FR_R&UART_FR_RXFE) == 0) && (RxFifo_Size() < (UART_FIFO_SIZE - 1))){
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

// input ASCII character from UART
// spin if RxFifo is empty
char UART_InChar(void){
  char letter;
  while(RxFifo_Get(&letter) == UART_FAILURE){};
  return(letter);
}

// output ASCII character to UART
// spin if TxFifo is full
void UART_OutChar(char data){
  while(TxFifo_Put(data) == UART_FAILURE){};
  UART0_IM_R &= ~UART_IM_TXIM;          // disable TX FIFO interrupt
  copySoftwareToHardware();
  UART0_IM_R |= UART_IM_TXIM;           // enable TX FIFO interrupt
}

// at least one of three things has happened:
// hardware TX FIFO goes from 3 to 2 or less items
// hardware RX FIFO goes from 1 to 2 or more items
// UART receiver has timed out
void UART0_Handler(void){
  if(UART0_RIS_R&UART_RIS_TXRIS){       // hardware TX FIFO <= 2 items
    UART0_ICR_R = UART_ICR_TXIC;        // acknowledge TX FIFO
    // copy from software TX FIFO to hardware TX FIFO
    copySoftwareToHardware();
    if(TxFifo_Size() == 0){             // software TX FIFO is empty
      UART0_IM_R &= ~UART_IM_TXIM;      // disable TX FIFO interrupt
    }
  }
  if(UART0_RIS_R&UART_RIS_RXRIS){       // hardware RX FIFO >= 2 items
    UART0_ICR_R = UART_ICR_RXIC;        // acknowledge RX FIFO
    // copy from hardware RX FIFO to software RX FIFO
    copyHardwareToSoftware();
  }
  if(UART0_RIS_R&UART_RIS_RTRIS){       // receiver timed out
    UART0_ICR_R = UART_ICR_RTIC;        // acknowledge receiver time out
    // copy from hardware RX FIFO to software RX FIFO
    copyHardwareToSoftware();
  }
}

int lcd_count = 0;

static Sema4Type uart_mutex = {.Value = 1};
static Sema4Type lcd_mutex = {.Value = 1};

// Print a character to a display
int fputc(int ch, FILE* f) {
  if (f == uart) {
    OS_bWait(&uart_mutex);
    UART_OutChar(ch);
    if (ch == 10) {
      UART_OutChar(13);
    }
    OS_bSignal(&uart_mutex);
    return 1;
  } else if (f == lcd) {
    OS_bWait(&lcd_mutex);
    if (lcd_count >= 20) {
      ST7735_OutChar('\n');
      lcd_count = 0;
    }
    ST7735_OutChar(ch);
    if ((ch == 10) || (ch == 13) || (ch == 27)) {
      lcd_count = 0;
    } else {
      lcd_count++;
    }
    OS_bSignal(&lcd_mutex);
    return (1);
  } else if (f == lcd_os) {
    OS_bWait(&lcd_mutex);
    ST7735_OutChar(ch);
    OS_bSignal(&lcd_mutex);
    return (1);
  }
  return (EOF);
}

// Get input from UART, echo
int fgetc(FILE* f) {
  char ch = EOF;
  if (f == uart) {
    OS_bWait(&uart_mutex);
    if(RxFifo_Size() > 0){
        ch = UART_InChar();
    }
    OS_bSignal(&uart_mutex);
    return(ch);
  }else if (f == lcd) {
    return (ch);
  } else if (f == lcd_os) {
    return (ch);
  }
  return (ch);
}

/*
// Function called when file error occurs.
int ferror(FILE *f){

    if(f == uart){
        return(EOF);
    }else if(f == lcd){
        return(EOF);
    }else if(f == lcd_os){
        return(EOF);
    }
    return EOF;

}
*/

// Clear display
void Output_Clear(void) {}

// Turn off display (low power)
void Output_Off(void) {}

// Turn on display
void Output_On(void) {}

// set the color for future output
void Output_Color(uint32_t newColor) { ST7735_SetTextColor(newColor); }

//------------Output_Init------------
// Initialize the UART for 115,200 baud rate (assuming 16 MHz bus clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void Output_Init(void) {
  UART_Init();
  ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(0);
  ST7735_SetRotation(2);
}
