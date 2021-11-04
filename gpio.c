
#include <avr/io.h>
#include "board.h"
#include "gpio.h"

//Register Offsets. 
#define PIN (0) //Register for reading the pins.
#define DDR (1) //Data Direction Register. Configures pins as output or input. 
#define PORT (2) //Register for settings the pins. 

void GPIO_Output_Init(volatile uint8_t  * IO_add, char mask){
	*(IO_add+DDR) =*(IO_add+DDR) | mask;
}
void GPIO_Output_Set(volatile uint8_t  * IO_add, char mask){
	*(IO_add+PORT) = *(IO_add+PORT) | mask;
}
void GPIO_Output_Clear(volatile uint8_t  * IO_add, char mask){
	*(IO_add+PORT) = *(IO_add+PORT) & (~ mask);
}