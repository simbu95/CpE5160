
#ifndef _GPIO_H
#define _GPIO_H

#include <avr/io.h>
//#public

void GPIO_Output_Init(volatile uint8_t  * IO_add, char mask);
void GPIO_Output_Set(volatile uint8_t  * IO_add, char mask);
void GPIO_Output_Clear(volatile uint8_t  * IO_add, char mask);

#endif