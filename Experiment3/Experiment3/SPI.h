/*
 * SPI.h
 *
 * Created: 10/13/2021 12:14:10 PM
 *  Author: pqmf44
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

// SPI register offsets
#define SPCR (0)
#define SPSR (1)
#define SPDR (2)

// SPCR bit offsets
#define SPR (0)  // Controls clock divison 
#define CPHA_OFFSET (2) // edge select/ clock phase
#define CPOL_OFFSET (3) // clock polarity
#define MSTR_OFFSET (4) // 1: master 0: slave
#define DORD_OFFSET (5) // 1: lsb first 0: msb first
#define SPE_OFFSET (6) // SPI enable
#define SPIE_OFFSET (7) // Interrupt enable

// Status register bit offsets
#define SPI2X_OFFSET (0)  // Doubles SPI clock rate
#define WCOL_OFFSET (6)  //Write Collision
#define SPIF_OFFSET (7)  //Interrupt Flag, is set when transfer is complete. Unset when SPIDR is read

#define no_errors (0)
#define timeout_error (1)
#define SPI_error (2)
#define command_error (3)


void SPI0_Init( uint32_t clock_rate);
void SPI_Init(uint8_t volatile * SPI_addr, uint32_t clock_rate);
uint8_t SPI_Transfer(uint8_t volatile * SPI_adr, uint8_t send_value, uint8_t * e_flag);



#endif /* SPI_H_ */