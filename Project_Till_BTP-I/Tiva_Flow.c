/*
 * flow.c
 * This is the program that will govern the data flow between all the components i.e. Analog hardware, TIVA board and FPGA.
 *
 *  Created on: Oct 1, 2019
 *      Author: Mohil
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
//*****************************************************************************************************************************
// Global variables
// We will store the values from the ADC in 2 variable, as FIFO depth is 32 bit thus we will store in the uint32_t
uint32_t ADCValue_0[1];
uint32_t ADCValue_1[1];

// After converting the variables into 3 eight bit variables we will store them in the following variables for transmission
uint8_t uartTX[3];

// After receiving the three variables from the RX of UART they are stored in the following variables
uint8_t uartRX[3];
uint32_t uartRXindex; // Index used for making sure do SSI after we receive the three variables
//bool uartRX_receive; // This boolean variable is used to make know whether UART has received are not

// We will store the data to be send to the SSI in the following variables
unsigned int SSI_TX[2];

// Introducing a loop around buffer to SSI_send process
unsigned int SSI_send_buffer[4096];
unsigned int head_index = 0; //Position for next storage
unsigned int tail_index = 0; //Position to pick up the data from


unsigned int int_count=0,temp=0;
unsigned int timerval = 0;

uint8_t sbox[256] = {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67,
        0x2b, 0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59,
        0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7,
        0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1,
        0x71, 0xd8, 0x31, 0x15, 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05,
        0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83,
        0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29,
        0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
        0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 0xd0, 0xef, 0xaa,
        0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c,
        0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc,
        0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec,
        0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19,
        0x73, 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee,
        0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49,
        0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4,
        0xea, 0x65, 0x7a, 0xae, 0x08, 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6,
        0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 0x70,
        0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9,
        0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e,
        0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 0x8c, 0xa1,
        0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0,
        0x54, 0xbb, 0x16};

uint8_t rcon[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

//*****************************************************************************************************************************
void
ADC_Init(void){
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
void
SSI_Init(void){

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

    // Not sure why this is done ask vidur about it-- Mostly this is GPIO_PIN_6|GPIO_PIN_7
    GPIOPinWrite(GPIO_PORTA_BASE,0xC0,0xC0);


}

//*****************************************************************************************************************************
void
UART_Init(){

    // Enabling UART 7 for our data transmission
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);

    // Enabling the GPIO port E for UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Configuring the pins as UART pins
    GPIOPinConfigure(GPIO_PE0_U7RX); // E0 is RX
    GPIOPinConfigure(GPIO_PE1_U7TX); // E1 is TX
    GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configuring the UART as 8 bit, stop bit 1 and parity none
    UARTConfigSetExpClk(UART7_BASE, SysCtlClockGet(), 230400, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Enabling UART FIFOs
    UARTFIFOEnable(UART7_BASE);

    // Enabling the UART itself
    UARTEnable(UART7_BASE);
}

//*****************************************************************************************************************************
void
SPI_send(unsigned int tx_data){

    //unsigned int i=0;
    //GPIOPinWrite(GPIO_PORTA_BASE,0xC0,0x80);
    //GPIOPinWrite(GPIO_PORTA_BASE,0xC0,0xC0);
    GPIOPinWrite(GPIO_PORTA_BASE,0xC0,1<<7); // Mostly for GPIO_PIN_6|GPIO_PIN_7
    GPIOPinWrite(GPIO_PORTA_BASE,0xC0,0);  // Mostly for GPIO_PIN_6|GPIO_PIN_7
    SSIDataPut(SSI0_BASE,(tx_data>>8));
    SSIDataPut(SSI0_BASE,(tx_data%256));
        while(SSIBusy(SSI0_BASE));
    //SSIIntClear(SSI0_BASE,SSI_TXEOT);
    GPIOPinWrite(GPIO_PORTA_BASE,0xC0,0x40); // Mostly for GPIO_PIN_6|GPIO_PIN_7
    GPIOPinWrite(GPIO_PORTA_BASE,0xC0,0xC0); // Mostly for GPIO_PIN_6|GPIO_PIN_7
}

//****************************************************************************************************************************
void timer_init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))
            { }
                                            //    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
                                            //    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
   IntMasterEnable();
   TimerConfigure(TIMER1_BASE,TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
   TimerLoadSet(TIMER1_BASE,TIMER_A,10000); //10000 for 8kHz cycle - reducing it to check affinity with the code
   //Currently clock is 40Mhz so 5000
   IntEnable(INT_TIMER1A);
   TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);

   TimerEnable(TIMER1_BASE,TIMER_A);
}

void Timer1AInt(void)
{
    TimerDisable(TIMER1_BASE, TIMER_A);
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
//timerval = TimerValueGet(TIMER1_BASE, TIMER_A);
    int_count++;
//     temp = ~temp;
//     temp &= 0x01;
//     GPIOPinWrite(GPIO_PORTF_BASE,1<<3,temp<<3);

//timerval = TimerValueGet(TIMER1_BASE, TIMER_A);
     TimerEnable(TIMER1_BASE, TIMER_A);
// timerval = TimerValueGet(TIMER1_BASE, TIMER_A);
     //}
}

//*****************************************************************************************************************************
void
ADC_Get_Data(){
    // Asking the processor to get data from the ADC
    ADCProcessorTrigger(ADC0_BASE, 3);

    // Making sure the data is read through the ADC
    while(!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }

    // Clearing the Flag to make sure it does not cause issue in the future
    ADCIntClear(ADC0_BASE, 3);
    temp = ~temp;
    temp &= 0x01;
                                            //    GPIOPinWrite(GPIO_PORTF_BASE,1<<3,temp<<3);
    // Reading the data in ADCValue_0
    ADCSequenceDataGet(ADC0_BASE, 3, ADCValue_0);

    // Asking the processor to get data from the ADC
//    ADCProcessorTrigger(ADC0_BASE, 3);
//
//    // Making sure the data is read through the ADC
//    while(!ADCIntStatus(ADC0_BASE, 3, false))
//    {
//    }
//
//    // Clearing the Flag to make sure it does not cause issue in the future
//    ADCIntClear(ADC0_BASE, 3);
//
//    // Reading the data in ADCValue_1
//    ADCSequenceDataGet(ADC0_BASE, 3, ADCValue_1);
}

//*****************************************************************************************************************************
void
Convert_for_UART(){
    uint32_t temp;
//temp = ((ADCValue_0[0] << 12) | ADCValue_1[0]);
    temp = ADCValue_0[0] & 0x00000FFF;
    uartTX[0] = (temp % 256);
    uartTX[1] = ((temp >> 8) % 256);
//uartTX[2] = ((temp >> 16) % 256);
}

//*****************************************************************************************************************************
void
UART_Transmission(){
    UARTCharPut(UART7_BASE, uartTX[0]);
    while(UARTBusy(UART7_BASE));
//    SysCtlDelay(20);
    UARTCharPut(UART7_BASE, uartTX[1]);
//UARTCharPut(UART7_BASE, uartTX[2]);
}

//*****************************************************************************************************************************
void
Store_to_SSI_Buffer(){
    uint32_t temp;
//temp = (uartRX[2]<<16 | uartRX[1] << 8 | uartRX[0]);
    temp = (uartRX[1] << 8 | uartRX[0]);
//SSI_TX[0] = ((temp >> 12) % 4096);
//SSI_TX[1] = (temp % 4096);
    SSI_TX[0] = (temp % 4096);
    SSI_send_buffer[head_index] = SSI_TX[0];
    head_index = ((head_index + 1) % 4096);
//    SPI_send(SSI_TX[0] | 0x2000);
//SPI_send(SSI_TX[0] | 0x2000);
//SPI_send(SSI_TX[1] | 0x2000);
}
//*****************************************************************************************************************************
void
transmit_sbox_rcon(){
    uint32_t count = 0;

    for (count = 0; count < 256; count++)
    {
        UARTCharPut(UART7_BASE, sbox[count]);
        while(UARTBusy(UART7_BASE));
    }

    for (count = 0; count < 10; count++)
    {
        UARTCharPut(UART7_BASE, rcon[count]);
        while(UARTBusy(UART7_BASE));
    }

}
//*****************************************************************************************************************************
int
main(void){
    unsigned int count_local = 0;
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
//    SysCtlClockSet( SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
//    uint32_t clockfreq;
//    clockfreq = SysCtlClockGet();

    // Following are the functions used to initialize the ADC, SSI and UART into proper configurations
                                    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
                                    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
                                    uint8_t set_led = 0x00; //Probably PF3
                                    GPIOPinWrite(GPIO_PORTF_BASE,1<<3,set_led<<3);
    ADC_Init();
    SSI_Init();
    UART_Init();
    timer_init();

    set_led = 0x01;
    GPIOPinWrite(GPIO_PORTF_BASE,1<<3,set_led<<3); //Probably PF3

    SysCtlDelay(3000000);

    transmit_sbox_rcon();
    uartRXindex = 0;
    head_index = 0;
    tail_index = 0;

    // Now we will initially read from the ADC change the values as required and then will send them through UART
//    ADC_Get_Data(); // This function will get ADC values and store them in the respective variables
//    Convert_for_UART(); // Changing the 12*2 bits into 3*8 bits for UART transmission
//    UART_Transmission(); // Transmitting the 3 bytes through UART, important the lowest will be sent first

    // Now we will wait in a while() statement till we get either UART Tx free to transmit or UART Rx receive
//    uartRX_receive = UARTCharsAvail(UART7_BASE);


    while(1){
//Waiting for the system to get either get transmitter free or the receiver gets any data
//
//while(UARTBusy(UART7_BASE) || !(UARTCharsAvail(UART7_BASE))){

//}

// Commented this out for new test basically only reading is once every 125us writing at any time
//        while (int_count == count_local){
//
//        }
//        count_local = int_count;

//while(UARTBusy(UART7_BASE) || !(UARTCharsAvail(UART7_BASE))){

//      }

//        while(UARTBusy(UART7_BASE)){
//
//                        }

//if(!UARTBusy(UART7_BASE)){
        if(int_count != count_local){
                ADC_Get_Data(); // This function will get ADC values and store them in the respective variables
                Convert_for_UART(); // Changing the 12*2 bits into 3*8 bits for UART transmission
                UART_Transmission(); // Transmitting the 3 bytes through UART, important the lowest will be sent first

                if(head_index != tail_index){
                    SPI_send(SSI_send_buffer[tail_index] | 0x2000);
                    tail_index = ((tail_index + 1) % 4096);
                }
                count_local = int_count;
        }
//}

        if(UARTCharsAvail(UART7_BASE)){
            if(uartRXindex == 0){
                uartRX[uartRXindex] = UARTCharGet(UART7_BASE);
                uartRXindex++;
            }
        }

        if(UARTCharsAvail(UART7_BASE)){
            if(uartRXindex == 1){
                uartRX[uartRXindex] = UARTCharGet(UART7_BASE);
                uartRXindex++;
            }
        }

        if(uartRXindex == 2){
            uartRXindex = 0;
            Store_to_SSI_Buffer();
        }

//  int_count=0;
//}
    }
//connect pa7 cs not pa3, 3.3 instead of 5 to dac

}
