

#include <avr/io.h>
#include "board.h"
#include "FAT.h"





uint8_t read8 (uint16_t offset, uint8_t * array_name){
	return array_name[offset];
}

//Double check that the compiler is handling the casting when doing the shifts, or do the ugly shift way + | way.
uint16_t read16 (uint16_t offset, uint8_t * array_name){
	//uint16_t ret = (array_name[offset] << 8) + array_name[offset+1];
	uint16_t ret = array_name[offset]
	ret=ret<<8;
	ret|=array_name[offset+1]
	return ret;
}


uint32_t read32 (uint16_t offset, uint8_t * array_name){
	//uint32_t ret = (array_name[offset] << 24) + (array_name[offset+1] << 16) + (array_name[offset+2] << 8) + array_name[offset+3];
	uint32_t ret = array_name[offset]
	ret=ret<<8;
	ret|=array_name[offset+1]
	ret=ret<<8;
	ret|=array_name[offset+2]
	ret=ret<<8;
	ret|=array_name[offset+3]
	return ret;
}