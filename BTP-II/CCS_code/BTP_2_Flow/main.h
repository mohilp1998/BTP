/*
 * main.h
 *
 *  Created on: Apr 17, 2020
 *      Author: Mohil
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>


#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_ints.h"

#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"


/*
 * Define block to easily change the UART Base
 */
#define UART_BASE_M 0x40013000  // UART7
//#define UART_BASE_M 0x4000C000  // UART0

/*
 * Addresses defined here for FLASH storage space, Flash is from 0x00000000 to 0x0003FFFF.
 * Check Debug/BTP_2_Flow.map file to note the unused space FLASH operations
 */
#define FLASH_BASE_ADDR         ((volatile uint32_t*)0x00020000)
#define FLASH_BASE_INIT_ADDR    ((volatile uint32_t*)0x00030000)
/*
 * Following are the global variable that are used at multiple different places
 */
extern uint32_t ADCValue_0[1];
extern uint32_t uartRXindex;
extern uint8_t uartRX[2];
extern unsigned int SSI_send_buffer[4096];
extern unsigned int head_index;

/*
 * Following are the function definitions, Timer1AInt is specifically specified as extern as it is should be
 */

// Functions of main.c
void timer_init(void);
extern void Timer1AInt(void);

//Functions of ADHOC.c
void transmit_sbox_rcon();
void receive_and_transmit_encrpytion_key();
void password_check();
void password_init();


//Functions of ADC.c
void ADC_Init(void);
void ADC_Get_Data();

//Funtions from SSI.c
void SSI_Init(void);
void SPI_send(unsigned int tx_data);
void Store_to_SSI_Buffer();

//Functions of UART.c
void UART7_Init();
void UART0_Init();
void Convert_for_UART();
void UART_Transmission();
void UARTSend(uint32_t ui32UARTBase, const uint8_t *pui8Buffer, uint32_t ui32Count);

//Functions of Flash.c
void Flash_Enable(void);
int Flash_Erase(int blockCount);
int Flash_Write(const void* data, int wordCount);
void Flash_Read(void* data, int wordCount);
int NoLongerFirstTime(void);


#endif /* MAIN_H_ */
