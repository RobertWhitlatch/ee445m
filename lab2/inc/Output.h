#include "CustomDefinitions.h"

#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <stdio.h>
extern FILE* uart;
extern FILE* uart_cmd;
extern FILE* lcd;
extern FILE* lcd_os;

// *************** Output_Init ********************
// Standard device driver initialization function for both streams
// Inputs: none
// Outputs: none
void Output_Init(void);

// Clear display
void Output_Clear(void);

// Turn off display (low power)
void Output_Off(void);

// Turn on display
void Output_On(void);

// set the color for future output
// Background color is fixed at black
// Input:  16-bit packed color
// Output: none
void Output_Color(uint32_t newColor);

#endif
