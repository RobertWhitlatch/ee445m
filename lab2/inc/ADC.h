#include "CustomDefinitions.h"

#ifndef __ADC_H__
#define __ADC_H__ 1

// -----------ADC_Open-----------
// Prepares an ADC0 channel to execute conversions, in a busy-wait fashion
// Input: Channel to be opened
// Output: None
void ADC_Open(int channelNum);

//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint16_t ADC_In(void);

//------------ADC0_Collect--------
// Collect a specificed amount of ADC data
// Input: Channel to be collected from, frequency to collect at,
//        Buffer to store data in, and number of samples to take
// Output: None
void ADC_Collect(uint32_t channelNum, uint32_t freq, uint16_t* buffer, uint32_t numberOfSamples);

//-----------ADC0_Status-----------
// Return status of the previous ADC0_Collect task.
// Input: None
// Output: Number of samples remaining, 0 indicates completion
int ADC_Status(void);

#endif
