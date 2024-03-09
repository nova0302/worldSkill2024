#include "DS3231.h"

uint8_t IIC_Read_OneByte(uint8_t Addr)
{
  uint8_t Data = 0;
  HAL_I2C_Mem_Read(&hi2c1,DS3231_Addr,Addr,1,&Data,1,0xFFFFFFFF);
  return Data;
}

void IIC_Write_OneByte(uint8_t Addr,uint8_t Data)
{
  HAL_I2C_Mem_Write(&hi2c1,DS3231_Addr,Addr,1,&Data,1,0xFFFFFFFF);
}

uint8_t HEX_To_BCD(uint8_t Hex)
{
  return (Hex >> 4) * 10 + (Hex & 0x0F);
}

uint8_t BCD_To_HEX(uint8_t BCD)
{
  return (BCD / 10) << 4 | (BCD % 10);
}

DS3231_Time_Type DS3231_Get_Time(void)
{
  DS3231_Time_Type Time = {0};

  Time.Second = HEX_To_BCD(IIC_Read_OneByte(0x00));
  Time.Minute = HEX_To_BCD(IIC_Read_OneByte(0x01));
  Time.Hour   = HEX_To_BCD(IIC_Read_OneByte(0x02) & 0x3F);
  Time.Week   = HEX_To_BCD(IIC_Read_OneByte(0x03));
  Time.Day    = HEX_To_BCD(IIC_Read_OneByte(0x04));
  Time.Month  = HEX_To_BCD(IIC_Read_OneByte(0x05) & 0x1F);
  Time.Year   = HEX_To_BCD(IIC_Read_OneByte(0x06));

  return Time;
}

void DS3231_Set_Time(DS3231_Time_Type Time)
{
  IIC_Write_OneByte(0x00,BCD_To_HEX(Time.Second));
  IIC_Write_OneByte(0x01,BCD_To_HEX(Time.Minute));
  IIC_Write_OneByte(0x02,BCD_To_HEX(Time.Hour));
  IIC_Write_OneByte(0x03,BCD_To_HEX(Time.Week));
  IIC_Write_OneByte(0x04,BCD_To_HEX(Time.Day));
  IIC_Write_OneByte(0x05,BCD_To_HEX(Time.Month));
  IIC_Write_OneByte(0x06,BCD_To_HEX(Time.Year));
}

void DS3231_Set_Alarm1(DS3231_Time_Type Time)
{
  IIC_Write_OneByte(0x07,BCD_To_HEX(Time.Second));
  IIC_Write_OneByte(0x08,BCD_To_HEX(Time.Minute));
  IIC_Write_OneByte(0x09,BCD_To_HEX(Time.Hour));
  IIC_Write_OneByte(0x0A,0x80);
}

DS3231_Time_Type DS3231_Get_Alarm1(void)
{
  DS3231_Time_Type Time = {0};

  Time.Second = HEX_To_BCD(IIC_Read_OneByte(0x07) & 0x7F);
  Time.Minute = HEX_To_BCD(IIC_Read_OneByte(0x08) & 0x7F);
  Time.Hour   = HEX_To_BCD(IIC_Read_OneByte(0x09) & 0x3F);

  return Time;
}

void DS3231_Disable_Alarm1(void)
{
  IIC_Write_OneByte(0x0E,0x84);
}

void DS3231_Enable_Alarm1(void)
{
  IIC_Write_OneByte(0x0E,0x85);
  Clean_Alarm1_Flag();
}

void Clean_Alarm1_Flag(void)
{
  IIC_Write_OneByte(0x0F,0x00);
}



