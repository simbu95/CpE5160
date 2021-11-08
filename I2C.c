
#include <avr/io.h>
#include "board.h"
#include "I2C.h"


#Register Offesets

#define TWB_REG (0)
#define TWS_REG (1)
#define TWA_REG (2)
#define TWD_REG (3)
#define TWC_REG (4)
#define TWAM_REG (5)

#define Prescale_1 (0)
#define Prescale_4 (1)
#define Prescale_16 (2)
#define Prescale_64 (3)

uint8_t TWI_Master_Transmit (uint8_t volatile *I2C_adr, uint32_t I2C_freq){
	uint8_t pre = ( ((F_CPU/F_DIV) / (I2C_freq)) -16UL ) / (510UL); //simplified some terms. 
	if(pre <1){ //double check the result of the integer division. If it rounds down, we can't use <=. if it doesn't round down we need <=. (though should still be fine?)
		*(I2C_adr+TWSR)=Prescale_1;
		pre=1;
	}
	else if(pre <4){
		*(I2C_adr+TWSR)=Prescale_4;
		pre=4;
	}
	else if(pre <16){
		*(I2C_adr+TWSR)=Prescale_16;
		pre=16;
	}
	else if(pre <64){
		*(I2C_adr+TWSR)=Prescale_64;
		pre=64;
	}
	else{
		return 0; //error, freq too low. 
	}
	*(I2C_adr+TWBR) = ( ((F_CPU/F_DIV) / (I2C_freq)) -16UL ) / (2UL * pre);
}

uint8_t TWI_Master_Receive(uint8_t volatile *I2C_adr, uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint16_t num_bytes, uint8_t * array_name ){
	
}

uint8_t TWI_Master_Init(uint8_t volatile *I2C_adr, uint32_t I2C_freq){
	
}