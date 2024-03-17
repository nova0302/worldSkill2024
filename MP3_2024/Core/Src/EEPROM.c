#include "eeprom.h"

void eeprom_8bit_write(uint16_t addr, uint8_t data)
{
  HAL_FLASHEx_DATAEEPROM_Unlock();
  HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_BYTE, DATA_EEPROM_BASE+(0x01*addr), data);
  HAL_FLASHEx_DATAEEPROM_Lock();
}

void eeprom_16bit_write(uint16_t addr, uint16_t data)
{
  HAL_FLASHEx_DATAEEPROM_Unlock();
  HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_HALFWORD, DATA_EEPROM_BASE+(0x02*addr), data);
  HAL_FLASHEx_DATAEEPROM_Lock();
}

void eeprom_32bit_write(uint16_t addr, uint32_t data)
{
  HAL_FLASHEx_DATAEEPROM_Unlock();
  HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_WORD, DATA_EEPROM_BASE+(0x04*addr), data);
  HAL_FLASHEx_DATAEEPROM_Lock();
}

uint8_t eeprom_8bit_read(uint16_t addr)
{
  return (*(__IO uint8_t*) (DATA_EEPROM_BASE + (0x01*addr)));
}

uint16_t eeprom_16bit_read(uint16_t addr)
{
  return (*(__IO uint16_t*) (DATA_EEPROM_BASE + (0x02*addr)));
}

uint32_t eeprom_32bit_read(uint16_t addr)
{
  return (*(__IO uint32_t*) (DATA_EEPROM_BASE + (0x04*addr)));
}
