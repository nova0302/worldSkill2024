#ifndef __CONTROL_HARDWARE_H
#define __CONTROL_HARDWARE_H

#include "main.h"
#include "Delay.h"
#include "OLED_SPI.h"
#include "dfr0299.h"
#include "DS3231.h"
#include "PAJ7620.h"
#include "DHT11.h"
#include "EEPROM.h"

#define BUZZ_ON  (BUZZ_GPIO_Port->BSRR = BUZZ_Pin)
#define BUZZ_OFF (BUZZ_GPIO_Port->BSRR = BUZZ_Pin << 16)

extern ADC_HandleTypeDef hadc;

extern uint8_t Buzz_Flag;
extern uint8_t Buzz_Cnt;
extern uint8_t Buzz_On_10ms;
extern uint8_t Buzz_Off_10ms;

void Battery_ADC_Init(void);
uint16_t Battery_ADC_Get(void);
void QJ008_Power_On(void);
void QJ008_Power_Off(void);
void TC8002_Power_On(void);
void TC8002_Power_Off(void);
void Buzz_Set(uint8_t cnt, uint8_t on_10ms, uint8_t off_10ms);
void Buzz_Handler(void);

#endif
