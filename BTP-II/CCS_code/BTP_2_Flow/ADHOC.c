/*
 * adhoc_fun.c
 *
 *  Created on: Apr 17, 2020
 *      Author: Mohil
 */

#include "main.h"

/*
 * Following are the locally used variables
 */

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

uint8_t encryption_key[16];

/*
 * This main file include adhoc functions, this include functions like sbox, rcon transmission, password initalization and password check.
 */
//*****************************************************************************************************************************
void transmit_sbox_rcon()
{
	// Transmitting the SBOX and RCON values to FPGA
    uint32_t count = 0;

    for (count = 0; count < 256; count++)
    {
        UARTCharPut(UART_BASE_M, sbox[count]);
        while(UARTBusy(UART_BASE_M));
    }

    for (count = 0; count < 10; count++)
    {
        UARTCharPut(UART_BASE_M, rcon[count]);
        while(UARTBusy(UART_BASE_M));
    }

}

//*****************************************************************************
void receive_and_transmit_encrpytion_key()
{
	// This function was intially used to receive the encryption key for FPGA for PC, but now it is cut out and FPGA receive the key directly from PC
    uint32_t count = 0;
    // From here on we will receive the encryption key which will be send to the FPGA board by our system similar as rcon and sbox
     encryption_key[0] = UARTCharGet(UART_BASE_M);
     encryption_key[1] = UARTCharGet(UART_BASE_M);
     encryption_key[2] = UARTCharGet(UART_BASE_M);
     encryption_key[3] = UARTCharGet(UART_BASE_M);
     encryption_key[4] = UARTCharGet(UART_BASE_M);
     encryption_key[5] = UARTCharGet(UART_BASE_M);
     encryption_key[6] = UARTCharGet(UART_BASE_M);
     encryption_key[7] = UARTCharGet(UART_BASE_M);
     encryption_key[8] = UARTCharGet(UART_BASE_M);
     encryption_key[9] = UARTCharGet(UART_BASE_M);
     encryption_key[10] = UARTCharGet(UART_BASE_M);
     encryption_key[11] = UARTCharGet(UART_BASE_M);
     encryption_key[12] = UARTCharGet(UART_BASE_M);
     encryption_key[13] = UARTCharGet(UART_BASE_M);
     encryption_key[14] = UARTCharGet(UART_BASE_M);
     encryption_key[15] = UARTCharGet(UART_BASE_M);

     for (count = 0; count < 16; count++)
     {
         UARTCharPut(UART_BASE_M, encryption_key[count]);
         while(UARTBusy(UART_BASE_M));
     }
}

//*****************************************************************************
void password_check()
{
    char current_password[8], input_password[8];

    // Reading the password stored in the FLASH memory
    Flash_Read(&current_password, sizeof(current_password)/sizeof(uint32_t));

    // Requesting user for the password message
    UARTSend(UART_BASE_M, (uint8_t *)"Please input the 8 character password:", strlen("Please input the 8 character password:"));
    
    //Reading the password send via UART.
    input_password[0] = UARTCharGet(UART_BASE_M);
    input_password[1] = UARTCharGet(UART_BASE_M);
    input_password[2] = UARTCharGet(UART_BASE_M);
    input_password[3] = UARTCharGet(UART_BASE_M);
    input_password[4] = UARTCharGet(UART_BASE_M);
    input_password[5] = UARTCharGet(UART_BASE_M);
    input_password[6] = UARTCharGet(UART_BASE_M);
    input_password[7] = UARTCharGet(UART_BASE_M);

    // Checking for correctness
    if(input_password[0] == current_password[0] && input_password[1] == current_password[1] && input_password[2] == current_password[2] && input_password[3] == current_password[3] && input_password[4] == current_password[4] && input_password[5] == current_password[5] && input_password[6] == current_password[6] && input_password[7] == current_password[7])
    {
        UARTSend(UART_BASE_M, (uint8_t *)"Password Correct  ", strlen("Password Correct  "));
    }
    else
    {
        UARTSend(UART_BASE_M, (uint8_t *)"Password Incorrect", strlen("Password Incorrect"));
        while (1) {}; // System stuck here
    }

}

//*****************************************************************************
void password_init()
{
    int first_time;
    
    // Reading the FLASH variable set to know whether its first time or a returning
    first_time = FLASH_BASE_INIT_ADDR[0];

    // If first time the -1 must be in the space else 0
    if (first_time == -1)
    {
        char password[8];
        UARTSend(UART_BASE_M, (uint8_t *)"Please set new 8 character password:", strlen("Please set new 8 character password:"));

        //Reading the password send via UART.
        password[0] = UARTCharGet(UART_BASE_M);
        password[1] = UARTCharGet(UART_BASE_M);
        password[2] = UARTCharGet(UART_BASE_M);
        password[3] = UARTCharGet(UART_BASE_M);
        password[4] = UARTCharGet(UART_BASE_M);
        password[5] = UARTCharGet(UART_BASE_M);
        password[6] = UARTCharGet(UART_BASE_M);
        password[7] = UARTCharGet(UART_BASE_M);

        //Writing the password to the FLASH_BASE_ADDR
        Flash_Write(&password, sizeof(password)/sizeof(uint32_t));

        //Changing the FLASH_BASE_INIT_ADDR to notify that no longer first time, i.e changing first_time to zero in flash
        NoLongerFirstTime();

        UARTSend(UART_BASE_M, (uint8_t *)"Password Set", strlen("Password Set"));
    }
    else
    {
    	// Else is required to send and receive useless to ensure correctness of transmission of FPGA
        UARTSend(UART_BASE_M, (uint8_t *)"000000000000000000000000000000000000", strlen("000000000000000000000000000000000000"));

        char temp_pass[8];
        //Temp read to ensure no issue with the system in whole
        temp_pass[0] = UARTCharGet(UART_BASE_M);
        temp_pass[1] = UARTCharGet(UART_BASE_M);
        temp_pass[2] = UARTCharGet(UART_BASE_M);
        temp_pass[3] = UARTCharGet(UART_BASE_M);
        temp_pass[4] = UARTCharGet(UART_BASE_M);
        temp_pass[5] = UARTCharGet(UART_BASE_M);
        temp_pass[6] = UARTCharGet(UART_BASE_M);
        temp_pass[7] = UARTCharGet(UART_BASE_M);

        UARTSend(UART_BASE_M, (uint8_t *)"000000000000", strlen("000000000000"));

    }

    return;

}


