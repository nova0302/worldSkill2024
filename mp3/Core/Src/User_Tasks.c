#include "User_Tasks.h"

char str_buf[20];
uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0, Sleep_Sec = 0;

uint32_t sw_tick = 0, idle_tick = 0;
uint8_t sw_flag = 0, sw_buf = 0, sw_it = 0;
uint8_t sw_alarm_flag = 0;

void Volume_Up()
{
	if (Volume < 30) Volume += 1;
	DFSetVolume(Volume);
	eeprom_8bit_write(0, Volume);
}

void Volume_Down()
{
	if (Volume) Volume--;
	DFSetVolume(Volume);
	eeprom_8bit_write(0, Volume);
}


void Alarm_Handler()
{

}

void loop(void)
{

	while (1)
	{

	}
}
