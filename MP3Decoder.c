#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "board.h"
#include "util/delay.h"
#include "UART_Print.h"
#include "I2C.h"
#include "MP3Decoder.h"


extern const uint8_t CONFIG [];
extern const uint8_t CONFIG2 [];
extern const uint8_t CONFIG3 [];

uint8_t MP3_Decoder_Config_File (uint8_t volatile *I2C_adr){
	uint16_t index=0;
	uint8_t send_array[1];
	uint8_t timeout;
	uint8_t reg_addr;
	uint8_t status;
	char* prnt_bffr = Export_print_buffer();
	do
	{
		reg_addr = pgm_read_byte(&CONFIG[index]);  // internal reg. addr
		index++;
		send_array[0] = pgm_read_byte(&CONFIG[index]);  // value for the reg.
		index++;
		timeout=50;
		do
		{
			status=TWI_Master_Transmit(&TWI1,0x43,reg_addr,1,1,send_array);
			timeout--;
		}while((status != 0) && (timeout != 0));
	} while((reg_addr != 0xFF) && (timeout != 0));
	
	if(timeout!=0) {
		status=sprintf(prnt_bffr,"Config sent\n\r");
		UART_Transmit_String(&UART1, status, prnt_bffr);
	}
	_delay_ms(1000);
	index=0;
	do
	{
		reg_addr = pgm_read_byte(&CONFIG2[index]);  // internal reg. addr
		index++;
		send_array[0] = pgm_read_byte(&CONFIG2[index]);  // value for the reg.
		index++;
		timeout=50;
		do
		{
			status=TWI_Master_Transmit(&TWI1,0x43,reg_addr,1,1,send_array);
			timeout--;
		}while((status != 0) && (timeout != 0));
	} while((reg_addr != 0xFF) && (timeout != 0));
	
	if(timeout!=0) {
		status=sprintf(prnt_bffr,"Config sent\n\r");
		UART_Transmit_String(&UART1, status, prnt_bffr);
	}
	_delay_ms(1000);
	index=0;
	do
	{
		reg_addr = pgm_read_byte(&CONFIG3[index]);  // internal reg. addr
		index++;
		send_array[0] = pgm_read_byte(&CONFIG3[index]);  // value for the reg.
		index++;
		timeout=50;
		do
		{
			status=TWI_Master_Transmit(&TWI1,0x43,reg_addr,1,1,send_array);
			timeout--;
		}while((status != 0) && (timeout != 0));
	} while((reg_addr != 0xFF) && (timeout != 0));
	
	if(timeout!=0) {
		status=sprintf(prnt_bffr,"Config sent\n\r");
		UART_Transmit_String(&UART1, status, prnt_bffr);
		return 0;
	}
	return 1;
}