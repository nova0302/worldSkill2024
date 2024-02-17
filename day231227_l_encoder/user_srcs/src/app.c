/*
 * app.c
 *
 *  Created on: Dec 27, 2023
 *      Author: sooho
 */
#include <stdbool.h>
#include "main.h"
#include "demux.h"
#include "dm.h"
#include "shift_reg.h"
#include "rotary_encoder.h"

extern TIM_HandleTypeDef htim6;

extern bool g_updateDM;

void appSetup() {
	HAL_TIM_Base_Start_IT(&htim6);
	demux_init();
	shift_reg_init();
	shift_reg_update();
}

void appLoop() {
	if (g_updateDM) {
		g_updateDM = false;
		demux_update();
	}
	updateRE(); // 로터리 엔코터 첵크 추가
	updateFramBuffer();

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		//writeFifo(&gFifo, TIME_OUT);
		g_updateDM = true;
	}
}
