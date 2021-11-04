/*
 * main.c
 *
 * Created: 9/23/2021 3:35:07 PM
 *  Author: dt7c9
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <xc.h>

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
#include "Long_Serial_In.h"


const char myString[66] PROGMEM = "This program was created by Dustin Tanksley and Preston Misemer\n\r";
const char Init[23] PROGMEM = "Initializing SD Card\n\r";
const char Failed[20] PROGMEM = "Failed\n\r";
const char Success[34] PROGMEM = "Init Succeeded, Reading Block 0\n\r";
const char Enter[33] PROGMEM = "Enter the block number to read\n\r";

int main(void)
{
	
	char print_char=0x55;
	char* print_buffer = Export_print_buffer();
	uint8_t buffer[512];
	uint32_t block;
	bool retval;
	uint8_t n;
	
	UART_Init(&UART1,9600UL,8,0,1);
	
	n = sprintf(print_buffer, "Testing character is %c\n\r", print_char);
	UART_Transmit_String(&UART1, n, print_buffer);

	n = Copy_String_to_Buffer(myString, 0, print_buffer); // If I don't manually enter the length, it tends to grab other strings as well. 
	UART_Transmit_String(&UART1, n, print_buffer);

	n = Copy_String_to_Buffer(Init, 0, print_buffer);
	UART_Transmit_String(&UART1, n, print_buffer);

	SPI0_Init(400000UL); //Init SPI 0

	retval = SD_Card_Init(&SPI0); //Init SD CARD
	if (retval) {  //If init successful
		_delay_ms(2000);
		n = Copy_String_to_Buffer(Success, 0, print_buffer);
		UART_Transmit_String(&UART1, n, print_buffer);
		GPIO_Output_Clear(&CHIP_SELECT_PORT, CHIP_SELECT_PIN);
		Send_Command(&SPI0, 17, 0x00);  //Read Block 0, and print
		
		Read_Block(&SPI0, 512, buffer);
		GPIO_Output_Set(&CHIP_SELECT_PORT, CHIP_SELECT_PIN);

		print_memory(&UART1, 512, buffer);
	}
	else {  //Else, Indicate failure to init
		n = Copy_String_to_Buffer(Failed, 0, print_buffer);
		UART_Transmit_String(&UART1, n, print_buffer);
	}
	
	

	while(1)
    {
		n = Copy_String_to_Buffer(Enter, 0, print_buffer);
		UART_Transmit_String(&UART1, n, print_buffer);
		
		block=long_serial_input(&UART1);  //Ask User for block number
		n=sprintf(print_buffer, "%ld\n\r", block);
		UART_Transmit_String(&UART1, n, print_buffer);
		
		n = sprintf(print_buffer, "Printing block %ld\n\r", block);
		UART_Transmit_String(&UART1, n, print_buffer);
		
		GPIO_Output_Clear(&CHIP_SELECT_PORT, CHIP_SELECT_PIN);
		Send_Command(&SPI0, 17, block); //Send CMD 17, read single block, with argument for received block number
		
		n=Read_Block(&SPI0, 512, buffer);
		GPIO_Output_Set(&CHIP_SELECT_PORT, CHIP_SELECT_PIN);
		if(n!=0){
			n = Copy_String_to_Buffer(Failed, 0, print_buffer);
			UART_Transmit_String(&UART1, n, print_buffer);
		}
		else{
			print_memory(&UART1, 512, buffer); //Print out contents of block. 
		}
		

		
    }
}

