/*
 * UART.c
 *
 *  Author: dt7c9
 */

//UART Register Offsets. 
#define UCSRA (0) //Control and Status Register A
#define UCSRB (1) //B
#define UCSRC (2) //C
#define UCSRD (3) //D
#define UBRRL (4) //Baud Rate Reload Low Byte
#define UBRRH (5) //High Byte
#define UDR   (6) //Data Register

#define UDRE (5) //Offset for Data Register Empty
#define RXC (7)  //Offset for Receive Complete

//Offsets for various settings for UART 
#define Synchronous (1<<6)
#define Asynchronous (0<<6)
#define No_Parity  (0<<4)
#define Even_Parity (2<<4)
#define Odd_Parity (3<<4)
#define Five_Bits (0<<1)
#define Six_Bits (1<<1)
#define Seven_Bits (2<<1)
#define Eight_Bits (3<<1)
#define Nine_Bits (1<<2)  //Nine Bits requires eight bits to be written to UCSRC, and then another bit set in USCRB
#define One_Stop (0<<3)
#define Two_Stop (1<<3)
#define Transmit_Enable (1<<3)
#define Receive_Enable (1<<4)




#include <avr/io.h>
#include "board.h"
#include "UART.h"

//Initialize UART. Takes the UART Address, Baud rate, number of bits (5-9), parity bit (0 for none, 1 for odd, 2 for even), and number of stop bits.
// U2X bit is assumed to be set to 0.  
void UART_Init(volatile uint8_t  * uart_add, uint32_t  Baud_Rate, uint8_t bits, uint8_t parity, uint8_t stops){
	//Init Baud Rate
	*(uart_add + UCSRA) = U2X_bit << 1;
	uint16_t UBRR = (uint16_t) ( ( ( F_CPU/OSC_DIV) / (8UL * (2-U2X_bit) * Baud_Rate) ) - 1);
	*(uart_add + UBRRH) = (uint8_t) UBRR/256;
	*(uart_add + UBRRL) = (uint8_t) UBRR%256;

	//Construct flags for control registers. 
	uint8_t stat_c = Asynchronous;
	uint8_t stat_b = Transmit_Enable | Receive_Enable;

	switch(bits){
		case 5:
			stat_c |= Five_Bits;
			break;
		case 6:
			stat_c |= Six_Bits;
			break;
		case 7:
			stat_c |= Seven_Bits;
			break;
		case 8:
			stat_c |= Eight_Bits;
			break;
		case 9:
			//In the case of 9 bits, set the bits in status c to same as 8 bits, and also set correct bit in B register
			stat_c |= Eight_Bits;
			stat_b |= Nine_Bits;
			break;
		default:
			//error
			break;
		
	}

	switch(parity){
		case 0:
			stat_c |= No_Parity;
			break;
		case 1:
			stat_c |= Odd_Parity;
			break;
		case 2:
			stat_c |= Even_Parity;
			break;
		default:
			//error
			break;
	}

	switch(stops){
		case 1:
			stat_c |= One_Stop;
			break;
		case 2:
			stat_c |= Two_Stop;
			break;
		default:
			//error
			break;
	}

	//Set the control registers
	*(uart_add + UCSRC) = stat_c;
	*(uart_add + UCSRB) = stat_b;
}

void UART_Transmit(volatile uint8_t  * uart_add, char c_to_trans){
	//Loop until Data Register is empty. 
	while( ( *(uart_add + UCSRA) & (1<<UDRE)) !=( 1<<UDRE) ){
		//loop
	}
	//Transmit.
	*(uart_add + UDR) = c_to_trans;
}

char UART_Receive(volatile uint8_t  * uart_add){
	//Loop until a character is received. 
	while( ( (*(uart_add + UCSRA) & (1<<RXC)) != (1<<RXC))){
		//loop
	}
	//Return received character
	return *(uart_add + UDR);
}