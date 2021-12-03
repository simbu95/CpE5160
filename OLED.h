/*
 * OLED.h
 *
 * Created: 7/1/2020 4:30:27 PM
 *  Author: youngerr
 */ 


#ifndef OLED_H_
#define OLED_H_




void OLED_Init(volatile SPI_t *OLED_port);
void OLED_Set_Line_0 (volatile SPI_t *OLED_port);
void OLED_Set_Line_1 (volatile SPI_t *OLED_port);
void OLED_Set_Line_2 (volatile SPI_t *OLED_port);
void OLED_Set_Line_3 (volatile SPI_t *OLED_port);
void OLED_Set_Cursor (volatile SPI_t *OLED_port, uint8_t position);
void OLED_Send_Char (volatile SPI_t *OLED_port, char ASCII_input);
void OLED_Clear_Line(volatile SPI_t *OLED_port);
void OLED_Clear_Display(volatile SPI_t *OLED_port);
void OLED_Transmit_String(volatile SPI_t *OLED_port, uint8_t num_bytes,char * string_name);


#endif /* OLED_H_ */