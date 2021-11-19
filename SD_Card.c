/*
 * SD_Card.c
 *
 * Created: 10/13/2021 2:05:38 PM
 *  Author: pqmf44
 */ 

#include <avr/pgmspace.h>

#include "board.h"
#include "SD_Card.h"
#include "gpio.h"
#include "SPI.h"
#include <stdbool.h> 
#include "UART_Print.h"

const char CMD0Error[16] PROGMEM ="CMD 0 Error\n\r";
const char CMD8Error[16] PROGMEM ="CMD 8 Error\n\r";
const char CMD41Error[16] PROGMEM ="CMD 41 Error\n\r";
const char CMD55Error[16] PROGMEM ="CMD 55 Error\n\r";
const char CMD58Error[16] PROGMEM ="CMD 58 Error\n\r";
const char NotActive[25] PROGMEM ="SD Card Not Active\n\r";

uint8_t Send_Command (uint8_t volatile *SPI_adr, uint8_t command, uint32_t argument){
	uint8_t checksum=0x01; //End bit preset
	uint8_t error_flag;
	if(command==0){ //If CMD is 0 or 8, then add in checksum
		checksum=0x95;
	}
	else if (command==8){
		checksum=0x87;
	}
	else if (command>63){  //Error out if CMD is larger then supported
		return command_error;
	}
	
	SPI_Transfer(SPI_adr,command | 0x40,&error_flag); //Set start transmission bits.
	if(error_flag!=no_errors){return error_flag;}
	for(uint8_t i = 0; i < 4; i += 1) //Shift argument into 8 bits, then send each part, MSB first
	{
		SPI_Transfer(SPI_adr, (uint8_t) (argument>>(24 - (i*8))), &error_flag);
		if(error_flag != no_errors)
		{
			return error_flag;
		}
	}
	if(error_flag!=no_errors){return error_flag;}
	SPI_Transfer(SPI_adr,checksum,&error_flag);
	return error_flag;
	
	
}

uint8_t Receive_Response(uint8_t volatile *SPI_adr, uint8_t num_of_bytes, uint8_t * array_name ){
	uint8_t error_flag;
	uint8_t rec=SPI_Transfer(SPI_adr,0xFF,&error_flag);
	while ((rec&0x80)!=0){ //If upper bit is set, keep sending commands waiting for a response
		rec=SPI_Transfer(SPI_adr,0xFF,&error_flag);
	}
	array_name[0]=rec;
	for( uint8_t i=1; i<num_of_bytes;i++){ //Receive number of bytes specified for the command. 
		array_name[i]=SPI_Transfer(SPI_adr,0xFF,&error_flag);
		if(error_flag!=no_errors){return error_flag;}
	}
	SPI_Transfer(SPI_adr,0xFF,&error_flag); //One more transfer to finish communication 
	return error_flag;
	
}

bool SD_Card_Init(uint8_t volatile *SPI_adr){
	uint8_t Response_Array[5];
	uint8_t * Response_Pointer = &Response_Array[0];
	uint8_t error_flag;
	uint8_t n;
	char* print_buffer = Export_print_buffer();
	GPIO_Output_Set(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);  // Set Chip Select High
	
	for (int i=0; i<11;i++){ //Send transfers to generate clock cycles >74. This sends 88 cycles to be safe. 
		SPI_Transfer(SPI_adr,0xFF,&error_flag);
	}
	
	GPIO_Output_Clear(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);  // Enable SD card chip select
	Send_Command(SPI_adr,0,0x00); //CMD 0, go to IDLE
	
	Receive_Response(SPI_adr,1,Response_Pointer);
	if(Response_Array[0]!=0x01){ //If not IDLE state, return error
		GPIO_Output_Set(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
		n = Copy_String_to_Buffer(CMD0Error, 0, print_buffer);
		UART_Transmit_String(&UART1, n, print_buffer);
		return false;
	}
	GPIO_Output_Set(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
	_delay_ms(1);
	
	GPIO_Output_Clear(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
	Send_Command(SPI_adr,8,0x000001AA);  //CMD 8 - Check compatible voltage range
	
	Receive_Response(SPI_adr,5,Response_Pointer);  //Return if R1 error (version 1.x card) or if voltage not supported
	if(Response_Array[0]!=0x01 && Response_Array[1]!=0x00 && Response_Array[2]!=0x00 && Response_Array[3]!=0x01 && Response_Array[4]!=0xAA){
		GPIO_Output_Set(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
		n = Copy_String_to_Buffer(CMD8Error, 0, print_buffer);
		UART_Transmit_String(&UART1, n, print_buffer);
		return false;
	}
	
	GPIO_Output_Set(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
	_delay_ms(1);
	
	GPIO_Output_Clear(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
	Send_Command(SPI_adr,58,0x00); //CMD 58, Reads OCR
	
	Receive_Response(SPI_adr,5,Response_Pointer);  //Confirm that SD card supports our particular voltage
	if(Response_Array[0]!=0x01 && ( Response_Array[2] != 0xFF)){
		GPIO_Output_Set(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
		n = Copy_String_to_Buffer(CMD58Error, 0, print_buffer);
		UART_Transmit_String(&UART1, n, print_buffer);
		return false;
	}
	
	uint8_t R1=0x01;
	
	while(R1==0x01){  // Loop until R1 is 0x00, aka active state
		GPIO_Output_Set(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
		_delay_ms(1);
	
		GPIO_Output_Clear(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
		Send_Command(SPI_adr,55,0x00);  //Send CMD 55
	
		Receive_Response(SPI_adr,1,Response_Pointer);
		if(Response_Array[0]>0x01){ //R1 response should be 0x00 or 0x01
			GPIO_Output_Set(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
			n = Copy_String_to_Buffer(CMD55Error, 0, print_buffer);
			UART_Transmit_String(&UART1, n, print_buffer);
			return false;
		}
		Send_Command(SPI_adr,41,0x40000000);  //Send CMD 41, with argument for high capacity support
	
		Receive_Response(SPI_adr,1,Response_Pointer);
		R1=Response_Array[0];
		if (R1>0x01){  //If R1 is not 0x00 or 0x01, error
			n = Copy_String_to_Buffer(CMD41Error, 0, print_buffer);
			UART_Transmit_String(&UART1, n, print_buffer);
			return false;
		}
	}
	if (R1 != 0x00){  //R1 should be active here, this is probably redundant. 
		n = Copy_String_to_Buffer(NotActive, 0, print_buffer);
		UART_Transmit_String(&UART1, n, print_buffer);
		return false;
	}
	
	GPIO_Output_Set(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
	_delay_ms(1);
	
	GPIO_Output_Clear(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
	Send_Command(SPI_adr,58,0x00);  //cmd 58 again to read OCR
	
	Receive_Response(SPI_adr,5,Response_Pointer);  //Checking that bits 30 and 31 are set, to indicate power on and high capicity support
	if(Response_Array[0]!=0x00 && ( (Response_Array[2] & 0xC0) == 0xC0)){
		GPIO_Output_Set(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
		return false;
	}
	GPIO_Output_Set(&CHIP_SELECT_PORT,CHIP_SELECT_PIN);
	_delay_ms(1);
	
	return true; //Init done. 
	
}

uint8_t Read_Block(uint8_t volatile *SPI_adr, uint16_t num_of_bytes, uint8_t * array_name){
	uint8_t error_flag;
	uint8_t rec = SPI_Transfer(SPI_adr, 0xFF, &error_flag);
	uint8_t timeout = 1;
	while ((rec & 0x80) != 0 && timeout!=0) { //Wait until SD card responds
		rec = SPI_Transfer(SPI_adr, 0xFF, &error_flag);
		timeout++;
	}
	if (timeout == 0 ) {
		return timeout_error;
	}
	else if (rec != 0) { //If R1 response is not active state, error has occurred, return
		return 3;
	}
	timeout = 1;
	rec = SPI_Transfer(SPI_adr, 0xFF, &error_flag);
	while ((rec != 0xFE) && ((rec & 0xF0) !=0) && (timeout != 0)) { //wait until response is data start token (0xFE) or until upper 4 bits are cleared
		rec = SPI_Transfer(SPI_adr, 0xFF, &error_flag); //This indicates a Data Error Token
		//timeout++;  //Timeout was usually occuring before FE token recieved, disabled for now. 
	}
	if (timeout == 0) {
		return timeout_error;
	}
	else if (rec != 0xFE) {  //If not data start token, error
		return rec; //need to find out error from lower 4 bits. 
	}
	for (uint16_t i = 0; i < num_of_bytes; i++) {  //Otherwise, read bytes equal to block size
		array_name[i] = SPI_Transfer(SPI_adr, 0xFF, &error_flag);
		if (error_flag != no_errors) { return error_flag; }
	}
	SPI_Transfer(SPI_adr, 0xFF, &error_flag); //could checksum if wanted... 
	SPI_Transfer(SPI_adr, 0xFF, &error_flag);
	SPI_Transfer(SPI_adr, 0xFF, &error_flag);// One more transfer to finish communication. 
	return error_flag; //Read block completed
}

