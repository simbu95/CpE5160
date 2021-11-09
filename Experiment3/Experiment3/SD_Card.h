/*
 * SD_Card.h
 *
 * Created: 10/13/2021 2:05:23 PM
 *  Author: pqmf44
 */ 


#ifndef SD_CARD_H_
#define SD_CARD_H_

#include <avr/io.h>
#include "util/delay.h"
#include <stdbool.h> 

uint8_t Send_Command (uint8_t volatile *SPI_adr, uint8_t command, uint32_t argument);

uint8_t Receive_Response(uint8_t volatile *SPI_adr, uint8_t num_of_bytes, uint8_t * array_name );

bool SD_Card_Init(uint8_t volatile *SPI_adr);

uint8_t Read_Block(uint8_t volatile *SPI_adr, uint16_t num_of_bytes, uint8_t * array_name);



#endif /* SD_CARD_H_ */