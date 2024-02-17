#ifndef __EEPROM_H
#define __EEPROM_H

#include "main.h"

void eeprom_8bit_write(uint16_t addr, uint8_t data);
void eeprom_16bit_write(uint16_t addr, uint16_t data);
void eeprom_32bit_write(uint16_t addr, uint32_t data);
uint8_t eeprom_8bit_read(uint16_t addr);
uint16_t eeprom_16bit_read(uint16_t addr);
uint32_t eeprom_32bit_read(uint16_t addr);

#endif
