#include "board_struct.h"
#include "SDCard.h"
#include "Control_Outputs.h"
#include "Read_Sector.h"




uint8_t Read_Sector(uint32_t sector_number, uint16_t sector_size, uint8_t * array_for_data)
{
	uint8_t SDtype,error_flag=No_Disk_Error;   

	//SDtype=Return_SD_Card_Type();  // Used for Standard Capacity SD Card Support
	
	GPIO_Output_Clear(&LED0_port, LED0_pin);
	GPIO_Output_Clear(&SD_CS_Port,SD_CS_Pin);  // Clear nCS = 0
	Send_Command(&SD_Card_Port,17,sector_number);
	Read_Block(&SD_Card_Port,sector_size,array_for_data);
	GPIO_Output_Set(&LED0_port, LED0_pin);
	GPIO_Output_Set(&SD_CS_Port,SD_CS_Pin);  // Set nCS = 1
	
    return error_flag;
}
