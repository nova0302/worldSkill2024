#include "User_Tasks.h"
#include "stm32l0xx_hal.h"

char str_buf[20];
uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0, Sleep_Sec = 0;

uint32_t sw_tick = 0, idle_tick = 0;
uint8_t sw_flag = 0, sw_buf = 0, sw_it = 0;
uint8_t sw_alarm_flag = 0;

uint32_t Tr_all = 0;
uint32_t Tr_now = 0;
int VOL = 10;

uint8_t sw1 = 0;
uint8_t sw2 = 0;
uint8_t sw3 = 0;
uint8_t sw4 = 0;
uint8_t SW1 = 0;
uint8_t SW2 = 0;
uint8_t SW3 = 0;
uint8_t SW4 = 0;
uint8_t modeF = 0;
uint8_t fF = 1;
uint8_t now = 0;
uint8_t sw3_s = 0;
uint8_t last = 0;
uint32_t tim = 0;
uint32_t tim1 = 0;

char Track[100];
char volume[100];
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) {
    tim1++;
    tim++;
  }

}
void Volume_Up() {
  if (Volume < 30)
    Volume++;
  DFSetVolume(Volume);
  eeprom_8bit_write(0, Volume);
}
void Volume_Down() {
  if (Volume)
    Volume--;
  DFSetVolume(Volume);
  eeprom_8bit_write(0, Volume);
}
void Alarm_Handler() {

}
void start_P() {

  OLED_Show_Picture(13, 25, 102, 5, Bar_Icon);

  OLED_Show_Str(52, 33, "00%", Font8x13, 0);
  OLED_Display();
  HAL_Delay(100);

  OLED_Show_Str(52, 33, "20%", Font8x13, 0);
  OLED_Fill(13, 26, 33, 28, 1);
  OLED_Display();
  HAL_Delay(100);

  OLED_Show_Str(52, 33, "40%", Font8x13, 0);
  OLED_Fill(13, 26, 53, 28, 1);
  OLED_Display();
  HAL_Delay(100);

  OLED_Show_Str(52, 33, "70%", Font8x13, 0);
  OLED_Fill(13, 26, 83, 28, 1);
  OLED_Display();
  HAL_Delay(100);

  OLED_Show_Str(52, 33, "80%", Font8x13, 0);
  OLED_Fill(13, 26, 93, 28, 1);
  OLED_Display();
  HAL_Delay(100);

  OLED_Show_Str(48, 33, "100%", Font8x13, 0);
  OLED_Fill(13, 26, 113, 28, 1);
  OLED_Display();
  HAL_Delay(100);

}
void main_P() {

  sw1 = HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);
  sw2 = HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
  sw3 = HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin);
  sw4 = HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);

  OLED_Show_Picture(0, 0, 16, 16, Dir_Icon);
  OLED_Show_Picture(100, 0, 16, 16, Alarm_Icon);
  OLED_Show_Picture(120, 0, 8, 16, Battry_Icon);

  sprintf(str_buf, "VOL:%02d", Volume);
  OLED_Show_Str(40, 3, str_buf, Font8x13, 0);

  sprintf(str_buf, "Track:%02ld/%02ld", Tr_now, Tr_all);
  OLED_Show_Str(20, 19, str_buf, Font8x13, 0);
  OLED_Display();

  if (!sw4) {
    if (fF) {
      tim = 0;
      fF = 0;
    }
    if (tim >= 100) {
      Buzz_Set(1, 5, 1);
      modeF = 1;
    }
    SW4 = 1;

  } else {
    fF = 1;

    if (SW4 && tim < 100) {
      Buzz_Set(1, 5, 1);
      Volume_Down();

      SW4 = 0;
    }
  }

}
void Option_P() {
  OLED_Clear();
}
void loop(void) {
  OLED_Init();
  OLED_Clear();
  OLED_Display();
  start_P();
  OLED_Clear();

  while (1) {
    if (modeF == 0)
      main_P();
    if (modeF == 1)
      Option_P();

  }
}
