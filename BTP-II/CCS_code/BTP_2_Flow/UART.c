/*
 * uart_code.c
 *
 *  Created on: Apr 17, 2020
 *      Author: Mohil
 */


#include "main.h"

/*
 * Following are the variables used locally by the code
 */
uint8_t uartTX[2];

/*
 * This file include all the UART specific functions. Following are the definitions
 */
//*****************************************************************************************************************************
void UART7_Init()
{
    // Enabling UART 7 for our data transmission
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);

    // Enabling the GPIO port E for UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Configuring the pins as UART pins
    GPIOPinConfigure(GPIO_PE0_U7RX); // E0 is RX
    GPIOPinConfigure(GPIO_PE1_U7TX); // E1 is TX
    GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configuring the UART as 8 bit, 230400 baud rate, stop bit 1 and parity none
    UARTConfigSetExpClk(UART7_BASE, SysCtlClockGet(), 230400, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Enabling UART FIFOs
    UARTFIFOEnable(UART7_BASE);

    // Enabling the UART itself
    UARTEnable(UART7_BASE);
}

//*****************************************************************************************************************************
void UART0_Init()
{
    // Enabling UART 0 for our data transmission
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Enabling the GPIO port A for UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configuring the pins as UART pins
    GPIOPinConfigure(GPIO_PA0_U0RX); // Pin A0 Rx
    GPIOPinConfigure(GPIO_PA1_U0TX); // Pin A1 Tx
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configuring the UART as 8 bit, 230400 baud rate, stop bit 1 and parity none
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 230400, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Enabling UART FIFOs
    UARTFIFOEnable(UART0_BASE);

    // Enabling the UART itself
    UARTEnable(UART0_BASE);
}

//*****************************************************************************************************************************
void Convert_for_UART()
{
	// Converting the received data as 2 blocks for UART Transmission done by UART_Transmission
    uint32_t temp;

    temp = ADCValue_0[0] & 0x00000FFF;
    uartTX[0] = (temp % 256);
    uartTX[1] = ((temp >> 8) % 256);
}

//*****************************************************************************************************************************
void UART_Transmission()
{
	// Transmistting the UART data, set by Convert_for_UART
    UARTCharPut(UART_BASE_M, uartTX[0]);
    while(UARTBusy(UART_BASE_M));
    UARTCharPut(UART_BASE_M, uartTX[1]);
}

//*****************************************************************************
void UARTSend(uint32_t ui32UARTBase, const uint8_t *pui8Buffer, uint32_t ui32Count)
{
	// This is mainly used to transfer the password message in between PC and microcontroller.

    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPut(ui32UARTBase, *pui8Buffer++);
    }
}

