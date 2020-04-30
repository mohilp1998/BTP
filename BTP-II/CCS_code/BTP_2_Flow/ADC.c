/*
 * ADC.c
 *
 *  Created on: Apr 17, 2020
 *      Author: Mohil
 */

#include "main.h"

/*
 * Following are the variables local to the code
 */


/*
 * Following are the variables global to the project
 */
uint32_t ADCValue_0[1];

/*
 * This file includes all the functions corresponding to ADC, following are the definitions of the same
 */

//*****************************************************************************************************************************
void ADC_Init(void)
{
    // We will configure ADC in the processor Trigger Mode, i.e. it will only read when the processor is ask it to read
    // The pin corresponding to the ADC is pin E3

    // Enabling the system peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enabling the GPIO port
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Configuring the pin E3 as ADC
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

    // Enabling sequence 3 in which the processor will only read single value when asked
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    // As the pin E3 is AIN0 in the datasheet we will require to enable channel 0 to make the system work.
    // Sample channel 0 (ADC_CTL_CH0) in single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                                 ADC_CTL_END);

    // Enabling the ADC
    ADCSequenceEnable(ADC0_BASE, 3);

    // Enabling the interrupt and clearing the interrupt flag to make sure that it does not affect our process ahead
    ADCIntEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);
}

//*****************************************************************************************************************************
void ADC_Get_Data()
{
    // Asking the processor to get data from the ADC
    ADCProcessorTrigger(ADC0_BASE, 3);

    // Making sure the data is read through the ADC
    while(!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }

    // Clearing the Flag to make sure it does not cause issue in the future
    ADCIntClear(ADC0_BASE, 3);

    // Reading the data in ADCValue_0
    ADCSequenceDataGet(ADC0_BASE, 3, ADCValue_0);

}

