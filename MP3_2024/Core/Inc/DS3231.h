#ifndef __DS3231_H
#define __DS3231_H

#include "main.h"

#define DS3231_Addr 0xD0

extern I2C_HandleTypeDef hi2c1;

typedef struct
{
  uint8_t Year;
  uint8_t Month;
  uint8_t Day;
  uint8_t Hour;
  uint8_t Minute;
  uint8_t Second;
  uint8_t Week;
}DS3231_Time_Type;

DS3231_Time_Type DS3231_Get_Time(void);
void DS3231_Set_Time(DS3231_Time_Type Time);

void DS3231_Set_Alarm1(DS3231_Time_Type Time);
void DS3231_Disable_Alarm1(void);
void DS3231_Enable_Alarm1(void);
void Clean_Alarm1_Flag(void);

DS3231_Time_Type DS3231_Get_Alarm1(void);

#endif
