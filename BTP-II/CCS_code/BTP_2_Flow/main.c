/*
 * main.c
 *
 *  Created on: Apr 17, 2020
 *      Author: Mohil
 */

#include "main.h"

/*
 * Following are the variable local to the code
 */
unsigned int tail_index = 0; //Position to pick up the data from SSI_send_buffer
unsigned int int_count=0; //Used by timer interrupt code

/*
 * Following are the variables global to the whole project
 */
uint32_t uartRXindex;
uint8_t uartRX[2];
unsigned int SSI_send_buffer[4096];
unsigned int head_index = 0; //Position to write next data to in SSI_send_buffer

/*
 * Following are the function in the main file, here we will include Timer Interrupt specific code and main loop
 */

int main(void)
{
    unsigned int count_local = 0;
    // Setting clock to 80 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    // Setting up the green led or PIN F.3 as it is used as reset the FPGA Board
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    //Reseting the FPGA Board by setting led = 0
    uint8_t set_led = 0x00;
    GPIOPinWrite(GPIO_PORTF_BASE,1<<3,set_led<<3);

    // Following are the functions used to initialize the ADC, SSI and UART into proper configurations
    ADC_Init();
    SSI_Init();

    if (UART_BASE_M == 0x40013000) // 0x40013000 => UART7_BASE; 0x4000C000 => UART0_BASE - Refer main.h for more information
    {
        UART7_Init();
    }
    else
    {
        UART0_Init();
    }

    timer_init();
    Flash_Enable();

    // Taking the FPGA out of reset state as now the Microcontroller is properly initialized
    set_led = 0x01;
    GPIOPinWrite(GPIO_PORTF_BASE,1<<3,set_led<<3);

    //Wait here to let FPGA set in after reset so it can efficiently receive the sbox and rcon
    SysCtlDelay(60000000); // Each call gives a 3 cycle delay thus this number will give around 2.5 second delay

    // IMPORTANT: This is commented out to test directly with PC no FPGA in Between
    // We are sending sbox and rcon before password as it is a  protection against garbage value which is sent by UART before it is properly initialized
    // The idea being that sbox and rcon filling time is significantly more than the time we set the reset to FPGA as high.

    transmit_sbox_rcon();

    /*
     * Following is the part where we check the password and receive encryption key
     */
    password_init();
    password_check();
    //receive_and_transmit_encrpytion_key(); //I am cutting this function out of use as FGPA will directly receive the encryption key

    // Setting up necessary initial variables
    uartRXindex = 0;
    head_index = 0;
    tail_index = 0;

    //The main loop where we sample, transmit and receive data
    while(1){
        // Noting the system that 8KHz (125 us) has passed and we need to sample new data now
        if(int_count != count_local)
        {
                ADC_Get_Data(); // This function will get ADC values and store them in the respective variables
                Convert_for_UART(); // Changing the 12*2 bits into 3*8 bits for UART transmission
                UART_Transmission(); // Transmitting the 3 bytes through UART, important the lowest will be sent first

                if(head_index != tail_index){
                    SPI_send(SSI_send_buffer[tail_index] | 0x2000);
                    tail_index = ((tail_index + 1) % 4096);
                }
                count_local = int_count;
        }

        // Here onwards we will receive data assuming that we will not receive any garbage data this reception is ordered
        // i.e. we receive 2 bytes which are ordered as first and second
        if(UARTCharsAvail(UART_BASE_M))
        {
            if(uartRXindex == 0){
                uartRX[uartRXindex] = UARTCharGet(UART_BASE_M);
                uartRXindex++;
            }
        }

        if(UARTCharsAvail(UART_BASE_M)){
            if(uartRXindex == 1){
                uartRX[uartRXindex] = UARTCharGet(UART_BASE_M);
                uartRXindex++;
            }
        }

        if(uartRXindex == 2){
            uartRXindex = 0;
            Store_to_SSI_Buffer();
        }

    }
}

//****************************************************************************************************************************
void timer_init(void)
{
    //Initializing the Timer 1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))
            { }

    // Enabling the interrupt
    IntMasterEnable();

    //Setting the timer to be periodically called at 8kHz rate
    TimerConfigure(TIMER1_BASE,TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
    TimerLoadSet(TIMER1_BASE,TIMER_A,10000); //10000 for 8kHz cycle at clock = 80MHz

    //Enabling the interrupt and the timer
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER1_BASE,TIMER_A);
}

//****************************************************************************************************************************
void Timer1AInt(void)
{
    //Disabling the timer and clearing the interrupt, disabling is necessary otherwise it may interfere operations like Interrupt clear and skew clock value
    TimerDisable(TIMER1_BASE, TIMER_A);
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Count is used in main to distinguish between 2 instances of timer interrupt thus ensuring we sample only once 8KHz cycle
    int_count++;

    //Enabling the Timer Again
     TimerEnable(TIMER1_BASE, TIMER_A);
}
