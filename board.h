#ifndef _BOARD_H
#define _BOARD_H

#define F_CPU (16000000UL)
#define OSC_DIV (1)
#define U2X_bit (0)

#define UART0 _SFR_MEM8(0xC0)
#define UART1 _SFR_MEM8(0xC8)
#define UART2 _SFR_MEM8(0xD0)

#define GPIO_C _SFR_MEM8(0x26) //Pin C
#define GPIO_C_LED (1<<7)

#define CPOL_bit (0)
#define CPHA_bit (0)

#define SPI0 _SFR_MEM8(0x4C)
#define SPI1 _SFR_MEM8(0xAC)

#define MOSI0_PORT _SFR_MEM8 (0x23) //Pin B
#define MOSI0_PIN (1<<5)
#define MISO0_PORT _SFR_MEM8 (0x23)
#define MISO0_PIN  (1<<6)
#define SCK0_PORT _SFR_MEM8 (0x23)
#define SCK0_PIN  (1<<7)


#define MOSI1_PORT _SFR_MEM8 (0x2C) //Pin E
#define MOSI1_PIN (1<<3)
#define MISO1_PORT _SFR_MEM8 (0x2C)
#define MISO1_PIN  (1<<2)
#define SCK1_PORT _SFR_MEM8 (0x29) //PinD
#define SCK1_PIN  (1<<7)

#define CHIP_SELECT_PORT _SFR_MEM8 (0x23) //Pin B
#define CHIP_SELECT_PIN (1<<4)

#define TWI0 _SFR_MEM8(0xB8)
#define TWI1 _SFR_MEM8(0xD8)

#endif