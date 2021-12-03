#include <stdio.h>
#include "board_struct.h"
#include "UART.h"
#include "UART_Print.h"
#include "File_System_v2.h"
#include "print_memory.h"
#include "Directory_Functions_struct.h"
#include "Read_Sector.h"
#include <avr/pgmspace.h>

/******* Private Constants *************/
#define CR (0x0D)
#define LF (0x0A)



// Read Value Functions

/***********************************************************************
DESC: Reads a specified byte from a block of memory 
INPUT: Offset to the byte, Starting address of the block of memory
RETURNS: uint8_t specified byte
CAUTION: 
************************************************************************/
uint8_t read8(uint16_t offset, uint8_t * array_name)
{
	uint8_t return_val = array_name[offset];
	
	return return_val;
}

/***********************************************************************
DESC: Reads a specified integer (stored little endian) from a block of memory 
INPUT: Offset to the LSB of the integer, Starting address of the block of memory
RETURNS: uint16_t specified integer
CAUTION: 
************************************************************************/

uint16_t read16(uint16_t offset, uint8_t * array_name)
{
	uint16_t return_val = array_name[offset+1];
	return_val=return_val<<8;
	
	return_val|=array_name[offset];
	
	return return_val;
}

/***********************************************************************
DESC: Reads a specified word (stored little endian) from a block of memory 
INPUT: Offset to the LSB of the word, Starting address of the block of memory
RETURNS: uint32_t specified word
CAUTION: 
************************************************************************/

uint32_t read32(uint16_t offset, uint8_t * array_name)
{
	uint32_t return_val = array_name[offset+3];
	return_val=return_val<<8;
	
	return_val|=array_name[offset+2];
	return_val=return_val<<8;
	
	return_val|=array_name[offset+1];
	return_val=return_val<<8;
	
	return_val|=array_name[offset];
	
	return return_val;
}



/***********************************************************************
DESC: Determines FAT type, Start of the FAT, Root Directory Sector
      and Start of Data for an SD card
INPUT: Start of a block of memory in xdata that can be used 
       to read blocks from the SD card
RETURNS: uint8_t error code    Creates globals in idata for: 
idata uint32_t FATSz., RelSec., RootClus., FirstDataSec., StartofFAT.;
idata uint16_t BytesPerSec., BytesPerSecM, RootEntCnt.;
idata uint8_t SecPerClus., FATtype, FATShift;

CAUTION: 
************************************************************************/

uint8_t Mount_Drive(uint8_t * array_in)
{
	
	FS_values_t * FAT_Param = Export_Drive_values();
	
	uint8_t NumberFATs = read8(0x10, array_in);
	
	uint16_t RsvdSectorCount = read16(0x0E, array_in);
	uint16_t RootEntCnt = read16(0x11, array_in);
	
	uint32_t FATSz = read16(0x16, array_in);
	if(FATSz == 0){
		FATSz = read32(0x24, array_in);
	}
	
	uint32_t TotalSectors = read16(0x13, array_in);
	if(TotalSectors == 0){
		TotalSectors = read32(0x20, array_in);
	}
	
	uint32_t RelSec = read32(0x1C, array_in);
	uint32_t RootClus = read32(0x2C, array_in);
	
	FAT_Param->FATtype = FAT32;
	FAT_Param->FATshift = FAT32_shift;
	
	FAT_Param->SecPerClus = read8(0x0D, array_in);
	FAT_Param->BytesPerSec = read16(0x0B, array_in);
	
	uint8_t i=0;
	while ((FAT_Param->BytesPerSec >> i) > 0){
		i++;
	}
	FAT_Param->BytesPerSecShift = i-1;
	
	FAT_Param->StartofFAT =  RsvdSectorCount + RelSec;
	FAT_Param->RootDirSecs = ( (RootEntCnt * 32) + (FAT_Param->BytesPerSec - 1) ) / FAT_Param->BytesPerSec;
	FAT_Param->FirstDataSec = RsvdSectorCount + (NumberFATs * FATSz) + FAT_Param->RootDirSecs + RelSec;
	FAT_Param->FirstRootDirSec = ( (RootClus-2) * FAT_Param->SecPerClus) + FAT_Param->FirstDataSec;
 
	uint32_t DataSec = TotalSectors - ( RsvdSectorCount + (NumberFATs * FATSz) + FAT_Param->RootDirSecs);
    
	if(DataSec<65535){
		//This is fat 16 drive and we are not supporting it.
		return Disk_Error;
	}
   return No_Disk_Error;
}


/***********************************************************************
DESC: Calculates the First Sector of a given Cluster 
INPUT: uint32_t Cluster number
RETURNS: uint32_t sector number 
CAUTION: 
************************************************************************/
uint32_t First_Sector (uint32_t Cluster_num)
{
	
	FS_values_t * FAT_Param = Export_Drive_values();
	
	uint32_t Sector_num= ( (Cluster_num - 2) * FAT_Param->SecPerClus) + FAT_Param->FirstDataSec;
   
	return Sector_num;
}





/***********************************************************************
DESC: Finds the next cluster number of a file in the FAT with an input
      parameter of the current cluster number.
INPUT: Current cluster number.
RETURNS: Next Cluster number from the FAT
CAUTION: 
************************************************************************/


uint32_t Find_Next_Clus(uint32_t Cluster_num, uint8_t * array_name)
{
	
	FS_values_t * FAT_Param = Export_Drive_values();
	uint8_t buffer_array[FAT_Param->BytesPerSec];
	
	uint32_t Sector = ( ( Cluster_num*FAT_Param->FATtype) / FAT_Param->BytesPerSec) + FAT_Param->StartofFAT;
	Read_Sector(Sector,FAT_Param->BytesPerSec,buffer_array);
	
	uint16_t FAToffset = (uint16_t) (Cluster_num * FAT_Param->FATtype) & (FAT_Param->BytesPerSec - 1);
	
    uint32_t return_clus= read32(FAToffset,buffer_array) & 0x0FFFFFFF;
   
   return return_clus;
}
   



/***********************************************************************
DESC: Prints the sectors of a file until the user hits X
      Prints the Cluster Number and Sector number of each sector as well
INPUT: Starting Cluster of the file and the start of a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint8_t 0
CAUTION: 
************************************************************************/


uint8_t Open_File(uint32_t Cluster_num, uint8_t * array_in)
{
	FS_values_t * FAT_Param = Export_Drive_values();
	uint16_t first_sector;
	uint8_t user_input;
	char * string_p = Export_print_buffer();
	uint16_t sector_index=0;
	
	
	first_sector = First_Sector(Cluster_num);
	Read_Sector(first_sector, FAT_Param->BytesPerSec, array_in);
	print_memory(&UART1, FAT_Param->BytesPerSec, array_in);
	while(1){
		sprintf(string_p, "Continue to next sector? (y/n): ");
		UART_Transmit_String(&UART1, 0, string_p);
		user_input = UART_Receive(&UART1);
		UART_Transmit(&UART1, user_input);
		UART_Transmit(&UART1, '\n');
		UART_Transmit(&UART1, '\r');
		if (user_input== 'n'){
			return 0;
		}
		else if(user_input == 'y') {
			sector_index++;
			if(sector_index>=FAT_Param->SecPerClus){
				Cluster_num = Find_Next_Clus(Cluster_num, array_in);
				if(Cluster_num == 0xFFFFFFFF) {
					sprintf(string_p, "End of file reached.\r\n");
					UART_Transmit_String(&UART1, 0, string_p);
					return 0;
				}
				else if(Cluster_num == 0x00000000) {
					sprintf(string_p, "Error: Unused Cluster.\r\n");
					UART_Transmit_String(&UART1, 0, string_p);
					return 1;
				}
				first_sector = First_Sector(Cluster_num);
			}
			Read_Sector((first_sector + sector_index), FAT_Param->BytesPerSec, array_in);
			print_memory(&UART1, FAT_Param->BytesPerSec, array_in);
			
			
		}
		else {
			sprintf(string_p, "Invalid selection.\r\n");
			UART_Transmit_String(&UART1, 0, string_p);
		}
	}
	return 0;
}




