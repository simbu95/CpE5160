
#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include "util/delay.h"

uint8_t TWI_Master_Transmit (uint8_t volatile *I2C_adr, uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint16_t num_bytes, uint8_t * array_name);

uint8_t TWI_Master_Receive(uint8_t volatile *I2C_adr, uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint16_t num_bytes, uint8_t * array_name );

uint8_t TWI_Master_Init(uint8_t volatile *I2C_adr, uint32_t I2C_freq);



#endif /* I2C_H_ */