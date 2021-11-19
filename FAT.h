#ifndef FAT_H_
#define FAT_H_

#include <avr/io.h>

uint8_t read8 (uint16_t offset, uint8_t * array_name);

uint16_t read16 (uint16_t offset, uint8_t * array_name);

uint32_t read32 (uint16_t offset, uint8_t * array_name);

#endif /* FAT_H_ */