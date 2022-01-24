//  ADC.c: File for functions related to ADC14 function of MSP432
//  Author: Jimmy Bates
//  Set: T
//  Date: October 21, 2020


#include "adc.h"
#include "msp.h"


/////////////////////////////////////////////////////////////////////////////////////////////
//  Function adcInit - initializes ADC for single conversions on the selected input channel
//  Arguments: analogChannel - the analog channel to be put into MEM[0]
//  Return values: none
/////////////////////////////////////////////////////////////////////////////////////////////
void adcInit(int analogChannel) {

    //Clear ADC Enable Conv bit
    ADC14->CTL0 &= ~ADC14_CTL0_ENC;

    //Select analog input to sample to Mem[0]
    ADC14->MCTL[0] = analogChannel;

    //Set ADC Enable Conv bit (enable conversions)
    ADC14->CTL0 |= ADC14_CTL0_ENC;

}


/////////////////////////////////////////////////////////////////////////////////////////////
//  Function adcSample - performs a conversion and returns digital result
//  Arguments: none
//  Return values: int of digital result MEM[0]
/////////////////////////////////////////////////////////////////////////////////////////////
int adcSample() {

    //Start the conversion
    ADC14->CTL0 |= ADC14_CTL0_SC;

    //Wait for conversion to complete
    while (!(ADC14->IFGR0 & ADC14_IFGR0_IFG0)) {
    }

    //Get the ADC result from the Mem[0] channel
    return ADC14->MEM[0];

}



