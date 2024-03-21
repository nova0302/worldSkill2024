#ifndef __DHT11_H
#define __DHT11_H

#include <string.h>
#include "main.h"
#include "Delay.h"
#include "stdbool.h"

typedef struct
{
  uint8_t Humi_Int;
  uint8_t Humi_Float;
  uint8_t Temp_Int;
  uint8_t Temp_Float;
  uint8_t Check_Byte;
}DHT11_Type;

bool DHT11_Read_Data(DHT11_Type *User_Data);

#endif
