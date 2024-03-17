#include "Control_Hardware.h"

uint8_t Buzz_Flag=0;
uint8_t Buzz_Cnt=0;
uint8_t Buzz_On_10ms=0;
uint8_t Buzz_Off_10ms=0;

#define ADC_SAMPLE 50
uint16_t adc_buf[ADC_SAMPLE];

void Battery_ADC_Init(void)
{
  HAL_ADC_Start(&hadc);
  for(uint8_t i=0; i<ADC_SAMPLE; i++)
  {
    HAL_ADC_PollForConversion(&hadc, 0xFFFF);
    adc_buf[i] = HAL_ADC_GetValue(&hadc);
  }
  HAL_ADC_Stop(&hadc);
}

uint16_t Battery_ADC_Get(void)
{
  static uint8_t adc_cnt=0;
  uint32_t adc=0;

  adc_cnt++;
  if(adc_cnt>ADC_SAMPLE-1) adc_cnt=0;

  HAL_ADC_Start(&hadc);

  HAL_ADC_PollForConversion(&hadc, 0xFFFF);
  adc_buf[adc_cnt] = HAL_ADC_GetValue(&hadc);

  HAL_ADC_Stop(&hadc);

  for(uint8_t i=0; i<ADC_SAMPLE; i++) adc += adc_buf[i];

  return adc/ADC_SAMPLE;
}

void DFR0299_Power_On(void)
{
  QJ008_CTRL_GPIO_Port->BSRR = QJ008_CTRL_Pin << 16;
  HAL_Delay(1000);
}

void DFR0299_Power_Off(void)
{
  QJ008_CTRL_GPIO_Port->BSRR = QJ008_CTRL_Pin;
}

void TC8002_Power_On(void)
{
  TC8002_CTRL_GPIO_Port->BSRR = TC8002_CTRL_Pin << 16;
}

void TC8002_Power_Off(void)
{
  TC8002_CTRL_GPIO_Port->BSRR = TC8002_CTRL_Pin;
}

void Buzz_Set(uint8_t cnt, uint8_t on_10ms, uint8_t off_10ms)
{
  Buzz_Flag = 0;
  Buzz_Cnt = cnt;
  Buzz_On_10ms = on_10ms;
  Buzz_Off_10ms = off_10ms;
}

void Buzz_Handler(void)
{
  static uint32_t buzz_tick=0;

  if(Buzz_Cnt)
  {
    if(HAL_GetTick() >= buzz_tick)
    {
      if(Buzz_Flag == 0)
      {
        buzz_tick = HAL_GetTick() + Buzz_On_10ms*10;
        BUZZ_ON;
      }
      if(Buzz_Flag == 1)
      {
        buzz_tick = HAL_GetTick() + Buzz_Off_10ms*10;
        BUZZ_OFF;
        Buzz_Cnt--;
      }

      Buzz_Flag = !Buzz_Flag;
    }
  }
  else
  {
    BUZZ_OFF;
    Buzz_Flag = 0;
  }
}
