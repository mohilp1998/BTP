#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthreadUtils.h>
#include <Pipes.h>
#include <pipeHandler.h>
#include <stdbool.h>
#ifdef AA2C
#include "mem_aa_c_model.h"
#else
#include "vhdlCStubs.h"
#endif

#define ORDER 0
#define iteration 100

void Exit(int sig)
{
	fprintf(stderr, "## Break! ##\n");
	exit(0);
}

void receiver(){
	fprintf(stdout,"Receiver starting\n");
	uint8_t temp[40];
	uint32_t temp32[0];
	uint64_t temp64[0];
	uint32_t i = 0, j = 0;
	uint8_t send[16];
	uint8_t sbox[16][16] = {
			{0x63, 0xca, 0xb7, 0x04, 0x09, 0x53, 0xd0, 0x51, 0xcd, 0x60, 0xe0, 0xe7, 0xba, 0x70, 0xe1, 0x8c},
			{0x7c, 0x82, 0xfd, 0xc7, 0x83, 0xd1, 0xef, 0xa3, 0x0c, 0x81, 0x32, 0xc8, 0x78, 0x3e, 0xf8, 0xa1},
			{0x77, 0xc9, 0x93, 0x23, 0x2c, 0x00, 0xaa, 0x40, 0x13, 0x4f, 0x3a, 0x37, 0x25, 0xb5, 0x98, 0x89},
			{0x7b, 0x7d, 0x26, 0xc3, 0x1a, 0xed, 0xfb, 0x8f, 0xec, 0xdc, 0x0a, 0x6d, 0x2e, 0x66, 0x11, 0x0d},
			{0xf2, 0xfa, 0x36, 0x18, 0x1b, 0x20, 0x43, 0x92, 0x5f, 0x22, 0x49, 0x8d, 0x1c, 0x48, 0x69, 0xbf},
			{0x6b, 0x59, 0x3f, 0x96, 0x6e, 0xfc, 0x4d, 0x9d, 0x97, 0x2a, 0x06, 0xd5, 0xa6, 0x03, 0xd9, 0xe6},
			{0x6f, 0x47, 0xf7, 0x05, 0x5a, 0xb1, 0x33, 0x38, 0x44, 0x90, 0x24, 0x4e, 0xb4, 0xf6, 0x8e, 0x42},
			{0xc5, 0xf0, 0xcc, 0x9a, 0xa0, 0x5b, 0x85, 0xf5, 0x17, 0x88, 0x5c, 0xa9, 0xc6, 0x0e, 0x94, 0x68},
			{0x30, 0xad, 0x34, 0x07, 0x52, 0x6a, 0x45, 0xbc, 0xc4, 0x46, 0xc2, 0x6c, 0xe8, 0x61, 0x9b, 0x41},
			{0x01, 0xd4, 0xa5, 0x12, 0x3b, 0xcb, 0xf9, 0xb6, 0xa7, 0xee, 0xd3, 0x56, 0xdd, 0x35, 0x1e, 0x99},
			{0x67, 0xa2, 0xe5, 0x80, 0xd6, 0xbe, 0x02, 0xda, 0x7e, 0xb8, 0xac, 0xf4, 0x74, 0x57, 0x87, 0x2d},
			{0x2b, 0xaf, 0xf1, 0xe2, 0xb3, 0x39, 0x7f, 0x21, 0x3d, 0x14, 0x62, 0xea, 0x1f, 0xb9, 0xe9, 0x0f},
			{0xfe, 0x9c, 0x71, 0xeb, 0x29, 0x4a, 0x50, 0x10, 0x64, 0xde, 0x91, 0x65, 0x4b, 0x86, 0xce, 0xb0},
			{0xd7, 0xa4, 0xd8, 0x27, 0xe3, 0x4c, 0x3c, 0xff, 0x5d, 0x5e, 0x95, 0x7a, 0xbd, 0xc1, 0x55, 0x54},	
			{0xab, 0x72, 0x31, 0xb2, 0x2f, 0x58, 0x9f, 0xf3, 0x19, 0x0b, 0xe4, 0xae, 0x8b, 0x1d, 0x28, 0xbb},
			{0x76, 0xc0, 0x15, 0x75, 0x84, 0xcf, 0xa8, 0xd2, 0x73, 0xdb, 0x79, 0x08, 0x8a, 0x9e, 0xdf, 0x16}};
	send[0] = 0x54;
	send[1] = 0x77;
	send[2] = 0x6f;
	send[3] = 0x20;
	send[4] = 0x4f;
	send[5] = 0x6e;
	send[6] = 0x65;
	send[7] = 0x20;
	send[8] = 0x4e;
	send[9] = 0x69;
	send[10] = 0x6e;
	send[11] = 0x65;
	send[12] = 0x20;
	send[13] = 0x54;
	send[14] = 0x77;
	send[15] = 0x6f;
	
	uint8_t rcon[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};
	
	char password_new_msg[36] = "Please set new 8 character password:";
	char new_password[8] = "12345678";
	char password_set_msg[12] = "Password Set";
	char password_input_msg[38] = "Please input the 8 character password:";
	char password_input[8] = "12345678";
	char password_correct_msg[18] = "Password Correct  ";

	
	uint8_t encry_key[16];
	encry_key[0] = 0x54;
	encry_key[1] = 0x68;
	encry_key[2] = 0x61;
	encry_key[3] = 0x74;
	encry_key[4] = 0x73;
	encry_key[5] = 0x20;
	encry_key[6] = 0x6d;
	encry_key[7] = 0x79;
	encry_key[8] = 0x20;
	encry_key[9] = 0x4b;
	encry_key[10] = 0x75;
	encry_key[11] = 0x6e;
	encry_key[12] = 0x67;
	encry_key[13] = 0x20;
	encry_key[14] = 0x46;
	encry_key[15] = 0x75;
	
	/*
	* Sending SBOX and RCON
	*/
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			temp[0] = sbox[j][i];
			fprintf(stdout,"SBOX: 0x%x\n", temp[0]);
			write_uint8_n("input_e",temp,1);
		}
	}
	
	for(i=0;i<10;i++)
	{	
		temp[0] = rcon[i];
		fprintf(stdout,"RCON: 0x%x\n", temp[0]);
		write_uint8_n("input_e",temp,1);
	}

	/*
	* Following is for password sequence
	*/
	
	// New Password msg
	for(i=0;i<36;i++)
	{
		temp[0] = password_new_msg[i];
		write_uint8_n("input_e",temp,1);
	}
	
	read_uint8_n("out_e",temp,36);
	
	fprintf(stdout,"MSG:");
	for(i=0;i<36;i++)
	{
		fprintf(stdout,"%c",temp[i]);
	}
	fprintf(stdout,"\n");

	//Sending new password
	for(i=0;i<8;i++)
	{
		temp[0] =new_password[i];
		write_uint8_n("input_d",temp,1);
	}
	
	read_uint8_n("out_d",temp,8);
	
	fprintf(stdout,"NEW_PASSWORD:");
	for(i=0;i<8;i++)
	{
		fprintf(stdout,"%c",temp[i]);
	}
	fprintf(stdout,"\n");
	
	//Password Set MSG
	for(i=0;i<12;i++)
	{
		temp[0] = password_set_msg[i];
		write_uint8_n("input_e",temp,1);
	}
	
	read_uint8_n("out_e",temp,12);
	
	fprintf(stdout,"MSG:");
	for(i=0;i<12;i++)
	{
		fprintf(stdout,"%c",temp[i]);
	}
	fprintf(stdout,"\n");
	
	//Password input msg
	for(i=0;i<38;i++)
	{
		temp[0] = password_input_msg[i];
		write_uint8_n("input_e",temp,1);
	}
	
	read_uint8_n("out_e",temp,38);
	
	fprintf(stdout,"MSG:");
	for(i=0;i<38;i++)
	{
		fprintf(stdout,"%c",temp[i]);
	}
	fprintf(stdout,"\n");

	//Input Password
	for(i=0;i<8;i++)
	{
		temp[0] = password_input[i];
		write_uint8_n("input_d",temp,1);	
	}

	read_uint8_n("out_d",temp,8);

	fprintf(stdout,"INPUT_PASSWORD:");
	for(i=0;i<8;i++)
	{
		fprintf(stdout,"%c",temp[i]);
	}
	fprintf(stdout,"\n");

	// Password correctness msg
	for(i=0;i<18;i++)
	{
		temp[0] = password_correct_msg[i];
		write_uint8_n("input_e",temp,1);
	}
	
	read_uint8_n("out_e",temp,18);
	
	fprintf(stdout,"MSG:");
	for(i=0;i<18;i++)
	{
		fprintf(stdout,"%c",temp[i]);
	}
	fprintf(stdout,"\n");
	
	/*	
	* Following encrpytion key transmission
	*/
	for(i=0;i<16;i++)
	{	
		temp[0] = encry_key[i];
		fprintf(stdout,"ENCRYPTION_KEY: 0x%x\n", temp[0]);
		write_uint8_n("input_d",temp,1);
	}

	/*	
	* Following is handshaking 
	*/

	read_uint8_n("out_e",temp,1);
	fprintf(stdout,"HANDSHAKE is: 0x%x\n", temp[0]);
	write_uint8_n("input_d",temp,1);

	read_uint8_n("out_e",temp,1);
	fprintf(stdout,"HANDSHAKE is: 0x%x\n", temp[0]);
	write_uint8_n("input_d",temp,1);
	
	read_uint8_n("out_e",temp,1);
	fprintf(stdout,"HANDSHAKE is: 0x%x\n", temp[0]);
	write_uint8_n("input_d",temp,1);	

	
	/*	
	* Now onwards it is data transmission and reception 
	*/
	write_uint8_n("input_e",send,14);	
	read_uint8_n("out_e", temp, 16);
	for(i=0; i< 16; i++)
	{	
		fprintf(stdout,"ENCRYPTION is: 0x%x\n", temp[i]);
	}
	
	write_uint8_n("input_d",temp,16);
	read_uint8_n("out_d", temp, 14);
	for(i=0; i< 14; i++)
	{	
		fprintf(stdout,"DECRYPTION is: 0x%x\n", temp[i]);
	}

	write_uint8_n("input_e",send,14);
	read_uint8_n("out_e", temp, 16);
	for(i=0; i< 16; i++)
	{	
		fprintf(stdout,"ENCRYPTION is: 0x%x\n", temp[i]);
	}
	
	write_uint8_n("input_d",temp,16);
	read_uint8_n("out_d", temp, 14);
	for(i=0; i< 14; i++)
	{	
		fprintf(stdout,"DECRYPTION is: 0x%x\n", temp[i]);
	}
	fprintf(stdout,"Exiting Receiver\n");
}

DEFINE_THREAD(receiver)

int main(int argc, char* argv[])
{
	float result[ORDER];
	signal(SIGINT,  Exit);
  	signal(SIGTERM, Exit);

#ifdef AA2C
	init_pipe_handler();
	mem_start_daemons (NULL, 0);
#endif

	PTHREAD_DECL(receiver);
	PTHREAD_CREATE(receiver);
	
	fprintf(stdout,"now here \n");
    PTHREAD_JOIN(receiver);
	
#ifdef AA2C
	mem_stop_daemons();
#endif		
	return(0);

}
