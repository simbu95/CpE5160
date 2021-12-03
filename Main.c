/*
 * UART_solution_struct.c
 *
 * Created: 10/5/2021 3:06:09 PM
 * Author : ryoun
 */ 

#include <avr/io.h>
#include "board_struct.h"
#include "Control_Outputs.h"
#include "UART.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "UART_Print.h"
#include "print_memory.h"
#include "Long_Serial_In.h"
#include <stdio.h>
#include "SPI.h"
#include "SDCard.h"
#include "OLED.h"
#include "Directory_Functions_struct.h"
#include "File_System_v2.h"
#include "Read_Sector.h"


const char test_string[28] PROGMEM = {"SD Initialization Program\n\r\0"};
const char LSI_Prompt[16] PROGMEM = {"Enter block #: "};
const char Complete[9] PROGMEM = {"  OK!\n\r\0"};
const char High_Cap[15] PROGMEM = {"High Capacity\0"};
const char Stnd_Cap[19] PROGMEM = {"Standard Capacity\0"};
	
uint8_t buffer1_g[512];
uint8_t buffer2_g[512];

int main(void)
{
	//
	//
	/* Provided Code Block */
	//
	//
	
	uint8_t error_flag,type;
	uint32_t input32;
	char *string_p;
	GPIO_Output_Set(&LED0_port, LED0_pin);
	GPIO_Output_Init(&LED0_port, LED0_pin);
	UART_init(&UART1,9600);
	string_p=Export_print_buffer();
	Copy_String_to_Buffer(test_string,0,string_p);
	UART_Transmit_String(&UART1,0,string_p);
	GPIO_Output_Clear(&LED0_port, LED0_pin);
	_delay_ms(100);
	GPIO_Output_Set(&LED0_port, LED0_pin);
	error_flag=SPI_Master_Init(&SPI0,400000);
	if(error_flag!=no_errors)
	{
		while(1);
	}
	OLED_Init(&OLED_SPI_Port);
	OLED_Set_Line_0 (&OLED_SPI_Port);
	Copy_String_to_Buffer(test_string,0,string_p);
	OLED_Transmit_String(&OLED_SPI_Port,7,string_p);
	error_flag=SD_Card_Init(&SD_Card_Port);
	if(error_flag!=no_errors)
	{
		while(1);
	}
	error_flag=SPI_Master_Init(&SPI0,10000000);
	Copy_String_to_Buffer(Complete,0,string_p);
	OLED_Transmit_String(&OLED_SPI_Port,5,string_p);
	OLED_Set_Line_1 (&OLED_SPI_Port);
	type=Return_SD_Card_Type();
	if(type==Standard_Capacity)
	{
		Copy_String_to_Buffer(Stnd_Cap,0,string_p);
		OLED_Transmit_String(&OLED_SPI_Port,0,string_p);
	}
	else
	{
		Copy_String_to_Buffer(High_Cap,0,string_p);
		OLED_Transmit_String(&OLED_SPI_Port,0,string_p);
	}
	GPIO_Output_Set(&LED0_port, LED0_pin);
	
	//
	//
	/* End of Provided Code Block */
	//
	//
	
	Read_Sector(0,512,buffer1_g);
	uint32_t BPB = read8(0,buffer1_g);
	
	if(BPB == 0xEB || BPB == 0xE9){
		BPB=0;
		error_flag=Mount_Drive(buffer1_g);
	}
	else{
		BPB=read32(0x01C6,buffer1_g);
		Read_Sector(BPB,512,buffer1_g);
		error_flag=Mount_Drive(buffer1_g);
	}
	FS_values_t * FAT_Param = Export_Drive_values();
	if(error_flag==No_Disk_Error){
		
		sprintf(string_p," BPB is %lu \n\r",BPB);
		UART_Transmit_String(&UART1,0,string_p);
		
		sprintf(string_p," First Data Sector is %lu \n\r",FAT_Param->FirstDataSec);
		UART_Transmit_String(&UART1,0,string_p);
		
		sprintf(string_p," Start of FAT is %lu \n\r",FAT_Param->StartofFAT);
		UART_Transmit_String(&UART1,0,string_p);
		uint16_t t = read16(0x0E, buffer1_g);
		sprintf(string_p," Rsvd Sec Cnt is %d \n\r", t);
		UART_Transmit_String(&UART1,0,string_p);
		
		//Print_Directory(FAT_Param->FirstDataSec,buffer1_g);
		sprintf(string_p," Sectors per cluster is %d \n\r",FAT_Param->SecPerClus);
		UART_Transmit_String(&UART1,0,string_p);
		
		
	}
	else{
		sprintf(string_p,"Error Mounting Disk");
		UART_Transmit_String(&UART1,0,string_p);
	}
	
	
	uint32_t DataSec=FAT_Param->FirstDataSec;
	while (1)
	{
		/*Copy_String_to_Buffer(LSI_Prompt,0,string_p);
		UART_Transmit_String(&UART1,0,string_p);
		input32=long_serial_input(&UART1);
		sprintf(string_p," %lu \n\r",input32);
		UART_Transmit_String(&UART1,0,string_p);
		GPIO_Output_Clear(&LED0_port, LED0_pin);
		GPIO_Output_Clear(&SD_CS_Port,SD_CS_Pin);  // Clear nCS = 0
		Send_Command(&SD_Card_Port,17,input32);
		Read_Block(&SD_Card_Port,512,buffer1_g);
		GPIO_Output_Set(&LED0_port, LED0_pin);
		GPIO_Output_Set(&SD_CS_Port,SD_CS_Pin);  // Set nCS = 1
		print_memory(&UART1,512,buffer1_g);
		*/
		
		//Directory_Printing(FAT_Param->FirstDataSec,FAT_Param);
		
		Print_Directory(DataSec,buffer1_g);
		
		//Prompt for directory
		Copy_String_to_Buffer(LSI_Prompt,0,string_p);
		UART_Transmit_String(&UART1,0,string_p);
		input32=long_serial_input(&UART1);
		sprintf(string_p," %lu \n\r",input32);
		UART_Transmit_String(&UART1,0,string_p);
		
		input32=Read_Dir_Entry(DataSec, (uint16_t) input32, buffer1_g);
		
		
		//if directory, then Recursive go down
		if((input32 & 0x10000000) != 0){
			sprintf(string_p," %lu \n\r",input32 & 0x0FFFFFFF);
			UART_Transmit_String(&UART1,0,string_p);
			if((input32 & 0x0FFFFFFF) == 0){
				DataSec=FAT_Param->FirstDataSec;
			}
			else{
				DataSec=First_Sector(input32 & 0x0FFFFFFF);
			}
		}
		//if file, then call Open File Function
		else{
			Open_File(input32,buffer1_g);
		}
		
		
	}
}



