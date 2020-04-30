/*
 * SSI.c
 *
 *  Created on: Apr 17, 2020
 *      Author: Mohil
 */


#include "main.h"

/*
 * Following are the variable local to the code
 */
unsigned int SSI_TX[1];

/*
 * This code include SSI specific functions only. Following are the definitions of them.
 */
//*****************************************************************************************************************************
void SSI_Init(void)
{
    // Enabling the SSI_0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    // Enabling the GPIO A header
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configuring all the pins required in the A port as SSI
    GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_3|GPIO_PIN_2);

    // Configuring additional GPIO pins as to overcome the timing diagram mismatch
    // CS is shifted to PA6 and LDAC is set as ground
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_6|GPIO_PIN_7);

    // Configuring the SSI to send data at the speed of 1MHz and 8 bits per cycle
    SSIConfigSetExpClk(SSI0_BASE, 4000000 ,SSI_FRF_MOTO_MODE_1,SSI_MODE_MASTER,1000000,8);

    // Enabling the SSI
    SSIEnable(SSI0_BASE);

    // Not sure why this is done ask vidur about it-- Mostly this is GPIO_PIN_6|GPIO_PIN_7, Done for timing diagram additional pin proper initialization
    GPIOPinWrite(GPIO_PORTA_BASE,0xC0,0xC0);
}

//*****************************************************************************************************************************
void SPI_send(unsigned int tx_data)
{
    // Necessary initial setting for timing diagram
    GPIOPinWrite(GPIO_PORTA_BASE,0xC0,1<<7); // GPIO_PIN_6|GPIO_PIN_7 - Setting Pin 7 to high
    GPIOPinWrite(GPIO_PORTA_BASE,0xC0,0);  // GPIO_PIN_6|GPIO_PIN_7 - Setting Pin 6 & 7 to low

    // Transferring data as 8 bits each
    SSIDataPut(SSI0_BASE,(tx_data>>8));
    SSIDataPut(SSI0_BASE,(tx_data%256));

    //Waiting for SSI to complete
    while(SSIBusy(SSI0_BASE));

    // Necessary Final setting for timing diagram
    GPIOPinWrite(GPIO_PORTA_BASE,0xC0,0x40); // GPIO_PIN_6|GPIO_PIN_7 - Pin 6 to high
    GPIOPinWrite(GPIO_PORTA_BASE,0xC0,0xC0); // GPIO_PIN_6|GPIO_PIN_7 - Pin 6 & 7 to high
}

//*****************************************************************************************************************************
void Store_to_SSI_Buffer()
{
    uint32_t temp;
    temp = (uartRX[1] << 8 | uartRX[0]);

    SSI_TX[0] = (temp % 4096);
    SSI_send_buffer[head_index] = SSI_TX[0];
    head_index = ((head_index + 1) % 4096);
}
