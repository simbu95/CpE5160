/*
 * UART.h
 *
 *  Author: dt7c9
 */
#ifndef _UART_H
#define _UART_H

#include <avr/io.h>

//#public

void UART_Init(volatile uint8_t  * uart_add, uint32_t  Baud_Rate, uint8_t bits, uint8_t parity, uint8_t stops);
void UART_Transmit(volatile uint8_t  * uart_add, char c_to_trans);
char UART_Receive(volatile uint8_t  * uart_add);

#endif

