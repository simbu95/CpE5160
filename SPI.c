/*
 * SPI.c
 *
 * Created: 10/13/2021 12:16:57 PM
 *  Author: pqmf44
 */ 
#include <avr/io.h>
#include "board.h"
#include "SPI.h"
#include "gpio.h"

#define SPI_Enable (1<<SPE_OFFSET)
#define SPI_Disable (0<<SPE_OFFSET)

#define Master_Mode (1<<MSTR_OFFSET)
#define Slave_Mode (0<<MSTR_OFFSET)

#define CPOL_VAL (CPOL_bit<<CPOL_OFFSET)

// When data is sampled
#define CPHA_VAL (CPHA_bit<<CPHA_OFFSET)

#define Base_div_2 (0)
#define Base_div_8 (1)
#define Base_div_32 (2)
#define Base_div_64 (3)

#define Double_div (0)
#define No_Double (1)


void SPI0_Init(uint32_t clock_rate){
	//Init DDR for Master out, clock, and chip select. For SPI0 these are all on the same port B.
	GPIO_Output_Init(&MOSI0_PORT, MOSI0_PIN | SCK0_PIN | CHIP_SELECT_PIN );
	GPIO_Output_Set(&MOSI0_PORT, MOSI0_PIN | CHIP_SELECT_PIN);
	if(CPOL_bit==1){  //Set clock based on Clock polarity bit
		GPIO_Output_Set(&SCK0_PORT,SCK0_PIN);
	}
	else{
		GPIO_Output_Clear(&SCK0_PORT,SCK0_PIN);
	}
	SPI_Init(&SPI0,clock_rate); //Call init for SPI 0 with specified Clock rate
}

void SPI_Init(uint8_t volatile * SPI_addr, uint32_t clock_rate)
{
	uint8_t div = (uint8_t)( (F_CPU/OSC_DIV)/(clock_rate) ); //Calculate clock rate
	uint8_t control_reg = SPI_Enable | Master_Mode | CPOL_VAL | CPHA_VAL;
	uint8_t status_reg = 0;
	
	if(div < 2) //Make sure division is high enough so  clock rate is slower then the value specified. 
	{
		control_reg |= Base_div_2;
		status_reg  |= No_Double;
	}
	else if(div < 4)
	{
		control_reg |= Base_div_2;
		status_reg |= Double_div;
	}
	else if(div < 8)
	{
		control_reg |= Base_div_8;
		status_reg |= No_Double;
	}
	else if(div < 16)
	{
		control_reg |= Base_div_8;
		status_reg |= Double_div;
	}
	else if(div < 32)
	{
		control_reg |= Base_div_32;
		status_reg |= No_Double;
	}
	else if(div < 64)
	{
		control_reg |= Base_div_32;
		status_reg |= Double_div;
	}
	else if(div < 128)
	{
		control_reg |= Base_div_64;
		status_reg |= No_Double;
	}
	else
	{
		//throw error
	}
	
	*(SPI_addr+SPCR)=control_reg; //Write to registers with completed values. 
	*(SPI_addr+SPSR)=status_reg;
	
}

uint8_t SPI_Transfer(uint8_t volatile * SPI_adr, uint8_t send_value, uint8_t *e_flag)
{
	*(SPI_adr+SPDR)=send_value; //Load send value into data register
	uint8_t timeout=1;
	uint8_t status; 
	uint8_t stat_f = (1<<WCOL_OFFSET) | (1<<SPIF_OFFSET);
	do {
		status = *(SPI_adr+SPSR); //Check status
		timeout++;
	} while ( ((status & stat_f) == 0) && (timeout != 0) ); //If status is not write collision or interrupt flag, keep checking
	
	if(timeout==0){  //If no flag set after too much time, error
		*e_flag=timeout_error;
		return 0xFF;
	}
	else if((status & (1<<WCOL_OFFSET)) != 0 ){ //If write collision, clear the flag by reading and set error flag.
		*e_flag=SPI_error;
		return *(SPI_adr+SPDR);
	}
	else{
		*e_flag=no_errors; //If no errors, return recieved value. 
		return *(SPI_adr+SPDR);
	}
	
}