#include "DHT11.h"

// Mode 1: GPIO_MODE_OUTPUT_PP, 0: GPIO_MODE_INPUT
void DHT11_Set_IO(uint8_t Mode)
{
  GPIO_InitTypeDef GPIO_Init = {0};

  DHT11_DATA_GPIO_Port->BSRR = DHT11_DATA_Pin;

  GPIO_Init.Mode  = ((Mode) ? (GPIO_MODE_OUTPUT_PP) : (GPIO_MODE_INPUT));
  GPIO_Init.Pin   = DHT11_DATA_Pin;
  GPIO_Init.Pull  = GPIO_PULLUP;
  GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(DHT11_DATA_GPIO_Port,&GPIO_Init);
}

uint8_t DHT11_Read_Byte(void)
{
  uint8_t Data = 0x00;

  for(uint8_t i=0; i<8; i++)
  {
    while(!(DHT11_DATA_GPIO_Port->IDR & DHT11_DATA_Pin));
    Delay_us(30);

    Data <<= 1;
    if(DHT11_DATA_GPIO_Port->IDR & DHT11_DATA_Pin)
    {
      Data |= 0x01;
      while(DHT11_DATA_GPIO_Port->IDR & DHT11_DATA_Pin);
    }
  }

  return Data;
}

void DHT11_Read_Data(DHT11_Type *User_Data)
{
  DHT11_Type Data = {0};

  DHT11_DATA_GPIO_Port->BSRR = (uint32_t)DHT11_DATA_Pin << 16;
  HAL_Delay(18);

  DHT11_DATA_GPIO_Port->BSRR = DHT11_DATA_Pin;
  Delay_us(30);

  DHT11_Set_IO(0);
  Delay_us(5);

  if(!(DHT11_DATA_GPIO_Port->IDR & DHT11_DATA_Pin))
  {
    while(!(DHT11_DATA_GPIO_Port->IDR & DHT11_DATA_Pin));
    while(DHT11_DATA_GPIO_Port->IDR & DHT11_DATA_Pin);

    Data.Humi_Int = DHT11_Read_Byte();
    Data.Humi_Float = DHT11_Read_Byte();
    Data.Temp_Int = DHT11_Read_Byte();
    Data.Temp_Float = DHT11_Read_Byte();
    Data.Check_Byte = DHT11_Read_Byte();
    if(Data.Humi_Int + Data.Humi_Float + Data.Temp_Int + Data.Temp_Float == Data.Check_Byte) memcpy(User_Data,&Data,sizeof(DHT11_Type));
  }
  DHT11_Set_IO(1);
}

