/*
 * main.c
 *
 * Created: 9/23/2021 3:35:07 PM
 *  Author: dt7c9
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
//#include <xc.h>

#include "board.h"

#include <stdio.h>
#include <stdbool.h> 
#include "util/delay.h"

#include "gpio.h"
#include "UART.h"
#include "UART_Print.h"
#include "print_memory.h"
#include "SD_Card.h"
#include "SPI.h"
#include "I2C.h"
#include "Long_Serial_In.h"
#include "MP3Decoder.h"


const char myString[66] PROGMEM = "This program was created by Dustin Tanksley and Preston Misemer\n\r";
const char Init_SD[23] PROGMEM = "Initializing SD Card\n\r";
const char Init_MP3[30] PROGMEM = "Initializing MP3 Decoder\n\r";
const char Failed[20] PROGMEM = "Failed\n\r";
const char Success[34] PROGMEM = "Init Succeeded, Reading Block 0\n\r";
const char Enter[33] PROGMEM = "Enter the block number to read\n\r";

int main(void)
{
	
	char* print_buffer = Export_print_buffer();
	uint8_t buf[3];
	uint8_t* buffer=&buf[0];
	uint8_t n;
	
	GPIO_Output_Init(&PINB,0x02);
	GPIO_Output_Clear(&PINB,0x02);
	_delay_ms(100);
	GPIO_Output_Set(&PINB,0x02);
	_delay_ms(100);
	
	UART_Init(&UART1,9600UL,8,0,1);

	n = Copy_String_to_Buffer(myString, 0, print_buffer); 
	UART_Transmit_String(&UART1, n, print_buffer);

	n = Copy_String_to_Buffer(Init_MP3, 0, print_buffer);
	UART_Transmit_String(&UART1, n, print_buffer);

	n=TWI_Master_Init(&TWI1,16000UL);
	
	n=TWI_Master_Receive(&TWI1,0x4F,0,0,2,buffer);
	
	n = sprintf(print_buffer, "Temp is %d\n\r", buf[0]);
	UART_Transmit_String(&UART1, n, print_buffer);
	
	_delay_ms(2000);
	
	n=TWI_Master_Receive(&TWI1,0x43,0x01,1,1,buffer);
	
	n = sprintf(print_buffer, "ID is %d\n\r", buf[0]);
	UART_Transmit_String(&UART1, n, print_buffer);
	
	_delay_ms(2000);
	
	MP3_Decoder_Config_File(&TWI1);
	
	while(1)
    {
		

		
    }
}

