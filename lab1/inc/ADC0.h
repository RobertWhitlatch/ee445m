#include "CustomDefinitions.h"

#ifndef __ADC0_H__
#define __ADC0_H__

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: programmable using variable 'channelNum' [0:11]
// SS3 interrupts: enabled but not promoted to controller
void ADC0_Open(int channelNum);

//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint16_t ADC0_In(void);

//------------ADC0_Collect--------
// Collect a specificed amount of ADC data
// Input: Channel to be collected from, frequency to collect at,
//        Buffer to store data in, and number of samples to take
// Output: None
void ADC0_Collect(uint32_t channelNum, uint32_t freq, uint16_t* buffer, uint32_t numberOfSamples);

//-----------ADC0_Status-----------
// Return status of the previous ADC0_Collect task.
// Input: None
// Output: Number of samples remaining, 0 indicates completion
int ADC0_Status(void);

#endif
