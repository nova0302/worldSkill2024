/*
 * app.c
 *
 *  Created on: Dec 27, 2023
 *      Author: sooho
 */
#include <stdbool.h>
#include "main.h"
#include "demux.h"
#include "shift_reg.h"
#include "ds3231.h"
#include "rotary_encoder.h"

//https://m.blog.naver.com/eziya76/221491571100

extern I2C_HandleTypeDef hi2c1;

_RTC rtc = { .Year = 19,
		.Month = 12,
		.Date  = 12,
		.DaysOfWeek = SUNDAY,
		.Hour = 12,
		.Min  = 12,
		.Sec  = 12 };

uint8_t regVal;
float rtcTemp;

extern bool g_updateDM;
extern TIM_HandleTypeDef htim6;

void appSetup() {
	HAL_TIM_Base_Start_IT(&htim6);
	demux_init();
	shift_reg_init();
	shift_reg_update();

	//DS3231_Init(&hi2c1);
	//DS3231_SetTime(&rtc);

	/* Configure Alarm1 */
	//DS3231_ClearAlarm1();
	//DS3231_SetAlarm1(ALARM_MODE_ONCE_PER_SECOND, 0, 0, 0, 0);
	//DS3231_SetAlarm1(ALARM_MODE_SEC_MATCHED, 0, 0, 0, 30);

}

void appLoop() {
	if (g_updateDM) {
		g_updateDM = false;
		demux_update();
	}
	updateRE();

	/*
	static uint32_t last;
	uint32_t now = HAL_GetTick();
	if (now - last > 1000) {
		last = now;

		DS3231_GetTime(&rtc);
		DS3231_ReadTemperature(&rtcTemp);

		ReadRegister(DS3231_REG_STATUS, &regVal);
		if (regVal & DS3231_STA_A1F) {
			regVal &= ~DS3231_STA_A1F;
			WriteRegister(DS3231_REG_STATUS, regVal);
		}
	}
	*/
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		//writeFifo(&gFifo, TIME_OUT);
		g_updateDM = true;
	}
}
