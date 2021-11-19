#include <stdio.h>
#include "board.h"
#include "UART.h"
#include "UART_Print.h"
#include "print_memory.h"
#include <avr/pgmspace.h>

/******* Private Constants *************/
#define CR (0x0D)
#define LF (0x0A)

const char Prnt_bytes_prnt[56] PROGMEM = {"Addr. 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n\r"}; 

/****** Private Functions **************/

void print_16bytes(uint8_t volatile * UART_addr, char * array_in);

/***********************************************************************
DESC:    Prints 8 bytes in Hexadecimal and then ASCII
INPUT: Pointer to an array of eight bytes
RETURNS: nothing
CAUTION: 
************************************************************************/
void print_16bytes(uint8_t volatile * UART_addr, char * array_in)
{
   char * input;
   uint8_t index, dat;
   char * prnt_bffr;
   
   prnt_bffr=Export_print_buffer();
   
   input = array_in;
   sprintf(prnt_bffr, "%p ",input);
   UART_Transmit_String(UART_addr,0,prnt_bffr);
   for (index=0;index<16;index++)
   {
      dat=(uint8_t)*(input+index);
      sprintf(prnt_bffr, "%2.2X ",dat);   // Changed from %2.2bX because this printf function can correctly determine number of bytes
	  UART_Transmit_String(UART_addr,0,prnt_bffr);
   }
   for (index=0;index<16;index++)
   { 
      dat=(uint8_t)*(input+index);
      if (dat<32 || dat>127) dat=46;
      UART_Transmit(UART_addr,(char)dat);
   }
   UART_Transmit(UART_addr,CR);
   UART_Transmit(UART_addr,LF);
}


/***********************************************************************
DESC: Prints an array from memory in Hexadecimal and then ASCII
INPUT: Pointer to an array, number of bytes to print
RETURNS: nothing
CAUTION: 
************************************************************************/
void print_memory(uint8_t volatile * UART_addr, uint16_t number_of_bytes, uint8_t * array_in)
{
   uint8_t * input;
   char * prnt_bffr;
   
   prnt_bffr=Export_print_buffer();
   
   input = array_in;
   Copy_String_to_Buffer(Prnt_bytes_prnt,0,prnt_bffr);
   UART_Transmit_String(UART_addr,0,prnt_bffr);
   do
   {
      print_16bytes(UART_addr,(char *)input);
      input+=16;
   }while(input<(array_in+number_of_bytes));
}
		