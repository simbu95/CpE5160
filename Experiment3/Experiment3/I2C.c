
#include <avr/io.h>
#include "board.h"
#include "I2C.h"


//Register Offsets

#define TWB_REG (0)
#define TWS_REG (1)
#define TWA_REG (2)
#define TWD_REG (3)
#define TWC_REG (4)
#define TWAM_REG (5)

//Bit Offsets
#define TWIE_offset (0)  //interrupt enable (should be 0)
#define TWEN_offset (2) //TWI enable. Must be 1 during entire TWI communication
#define TWWC_offset (3)  //write collision
#define TWSTO_offset (4) //Stop Condition bit
#define TWSTA_offset (5) //Start Condition bit
#define TWEA_offset (6)  //Enable Acknowledge bit
#define TWINT_offset (7) //indicates that it is clear to send next command, write to TWCR then write 1 to this bit, next command ready when this bit is set again. 

#define Prescale_1 (0)
#define Prescale_4 (1)
#define Prescale_16 (2)
#define Prescale_64 (3)

uint8_t TWI_Master_Transmit (uint8_t volatile *I2C_adr, uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint16_t num_bytes, uint8_t * array_name){
	uint8_t send_val = device_addr<<1;
	uint8_t temp_status;
	uint8_t index;
	uint8_t int_addr_index;
	uint8_t return_val = 0;
	
	//create start condition
	*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWSTA_offset)|(1<<TWEN_offset));
	
	//wait for start condition to be transmitted
	do{
		temp_status = *(I2C_adr+TWC_REG);
	}while(temp_status & (1<<TWINT_offset) == 0);
	
	if(temp_status == 0x08){ //start bit sent
		//send the device address
		*(I2C_adr+TWD_REG) = (device_addr<<1);
		*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWEN_offset));
		
		//wait for device address to be sent
		do{
			temp_status = *(I2C_adr+TWC_REG);
		}while(temp_status & (1<<TWINT_offset) == 0);
		
		//check for ACK from sending device address
		temp_status = *(I2C_adr+TWS_REG) & 0xF8;
		if(temp_status != 0x18){
			return 1;
		}
		
		int_addr_index = 0;
		while(int_addr_sz != 0 && int_addr_index < 5)
		{
			//send the internal address
			*(I2C_adr+TWD_REG) = ((uint8_t)(int_addr>>(8*int_addr_index)));
			*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWEN_offset));
			
			//wait for internal address index to transmit
			do{
				temp_status = *(I2C_adr+TWC_REG);
			}while(temp_status & (1<<TWINT_offset) == 0);
		}
		
		*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWEA_offset)|(1<<TWEN_offset));
		//check for ACK from sending internal address
		temp_status = *(I2C_adr+TWS_REG) & 0xF8;
		if(temp_status != 0x18){
			return 1;
		}
		
		index=0;
		while(num_bytes != 0 && return_val == 0){
			if(temp_status == 0x28){
				send_val = array_name[index];
				*(I2C_adr+TWD_REG) = send_val;
				*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWEN_offset));
				//finish send code
			}
		}
	}
	else{ //start bit not sent
		return 1;
	}
}

uint8_t TWI_Master_Receive(uint8_t volatile *I2C_adr, uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint16_t num_bytes, uint8_t * array_name ){
	uint8_t temp_status;
	uint8_t int_addr_index;
	uint8_t index;
	uint8_t return_val = 0;
	
	if(int_addr_sz > 0){
		//Can this just be TWI Master Transmit? I think it should be the same, as we are writing the internal address, then doing a read, right?
		//create start condition
		*(I2C_adr+TWC_REG)= (1<<TWINT_offset)|(1<<TWSTA_offset)|(1<<TWEN_offset);
		
		//wait for start condition to be transmitted
		do{
			temp_status = *(I2C_adr+TWC_REG);
		}while(temp_status & (1<<TWINT_offset) == 0);
		
		//confirm status start has been sent
		temp_status = *(I2C_adr+TWS_REG) & 0xF8;
		if((temp_status != 0x08) && (temp_status != 0x10)){
			return 1; //return error
		}
		
		//send the device address
		*(I2C_adr+TWD_REG) = (device_addr<<1);
		*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWEN_offset));
		
		//wait for device address to be sent
		do{
			temp_status = *(I2C_adr+TWC_REG);
		}while(temp_status & (1<<TWINT_offset) == 0);
		
		//check for ACK from sending device address
		temp_status = *(I2C_adr+TWS_REG) & 0xF8;
		if(temp_status != 0x18){
			return 1;
		}
		
		int_addr_index = 0;
		while(int_addr_sz != 0 && int_addr_index < 5)
		{
			//send the internal address
			*(I2C_adr+TWD_REG) = ((uint8_t)(int_addr>>(8*int_addr_index)));
			*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWEN_offset));
			
			//wait for internal address index to transmit
			do{
				temp_status = *(I2C_adr+TWC_REG);
			}while(temp_status & (1<<TWINT_offset) == 0);
		}
		
		*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWEA_offset)|(1<<TWEN_offset));
		do{
			temp_status = *(I2C_adr+TWC_REG);
		}while(temp_status & (1<<TWINT_offset) == 0);
		
		//check for ACK from sending internal address
		temp_status = *(I2C_adr+TWS_REG) & 0xF8;
		if(temp_status != 0x50){
			return 1;
		}
	}
	
	//create start condition
	*(I2C_adr+TWC_REG)= (1<<TWINT_offset)|(1<<TWSTA_offset)|(1<<TWEN_offset);
	
	//wait for start condition to be transmitted
	do{
		temp_status = *(I2C_adr+TWC_REG);
	}while(temp_status & (1<<TWINT_offset) == 0);
	
	//confirm status start has been sent
	temp_status = *(I2C_adr+TWS_REG) & 0xF8;
	if((temp_status != 0x08) && (temp_status != 0x10)){
		return 1; //return error
	}
	
	//send the device address + R
	*(I2C_adr+TWD_REG) = ((device_addr<<1) | 0x01);
	*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWEN_offset));
	
	//wait for device address to be sent
	do{
		temp_status = *(I2C_adr+TWC_REG);
	}while(temp_status & (1<<TWINT_offset) == 0);
	
	//check status register
	temp_status = *(I2C_adr+TWS_REG) & 0xF8;
	
	// I think this could flow better.... something like num=num bytes
	//while num_bytes != 1 do no stop
	//num bytes ==1 do stop byte and end. 
	if(temp_status = 0x40){ //slave address + R sent, ACK received
		if(num_bytes == 1){
			*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(0<<TWEA_offset)|(1<<TWEN_offset));
		}
		else {
			*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWEA_offset)|(1<<TWEN_offset));
		}
		
		index=0;
		while((num_bytes != 0) && (return_val == 0)){
			do{
				temp_status = *(I2C_adr+TWC_REG);
			} while ((temp_status & 0x80) = 0);
		
			temp_status = *(I2C_adr+TWS_REG) & 0xF8;
			
			if(temp_status = 0x50){ //data byte received, ACK sent
				num_bytes--;
				array_name[index] = *(I2C_adr+TWD_REG);
				index++;
				if(num_bytes == 1){
					*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(0<<TWEA_offset)|(1<<TWEN_offset));
				}
				else {
					*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWEA_offset)|(1<<TWEN_offset));
				}
			}
			else if(temp_status = 0x58){ //data byte received, NACK sent
				num_bytes--;
				array_name[index] = *(I2C_adr+TWD_REG);
				*(I2C_adr+TWC_REG) = ((1<<TWINT_offset)|(1<<TWSTO_offset)|(1<<TWEN_offset));
				
				//stop condition
				do{
					temp_status = *(I2C_adr+TWC_REG);
				}while(temp_status & (1<<TWSTO_offset) == 1);
				
			}
			else{ //data not received
				return_val = 1;
			}
		}
	}
	else{
		return 1;
	}
	
	return return_val;
}

uint8_t TWI_Master_Init(uint8_t volatile *I2C_adr, uint32_t I2C_freq){
	//changed variables
	uint8_t pre = ( ((F_CPU/OSC_DIV) / (I2C_freq)) -16UL ) / (510UL); //simplified some terms. 
	if(pre < 1){ //double check the result of the integer division. If it rounds down, we can't use <=. if it doesn't round down we need <=. (though should still be fine?)
		*(I2C_adr+TWS_REG)=Prescale_1;
		pre=1;
	}
	else if(pre < 4){
		*(I2C_adr+TWS_REG)=Prescale_4;
		pre=4;
	}
	else if(pre < 16){
		*(I2C_adr+TWS_REG)=Prescale_16;
		pre=16;
	}
	else if(pre < 64){
		*(I2C_adr+TWS_REG)=Prescale_64;
		pre=64;
	}
	else{
		return 0; //error, freq too low. 
	}
	*(I2C_adr+TWB_REG) = ( ((F_CPU/OSC_DIV) / (I2C_freq)) -16UL ) / (2UL * pre);
	return 1;
}
