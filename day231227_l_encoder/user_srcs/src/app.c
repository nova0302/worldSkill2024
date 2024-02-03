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

extern bool g_updateDM;
extern bool g_bBtnEvent;
extern TIM_HandleTypeDef htim6;
extern DMx g_dm[12];
extern uint8_t const g_0[8];
extern uint8_t const g_1[8];
extern uint8_t const g_2[8];
extern uint8_t const g_3[8];
extern uint8_t const g_4[8];
extern uint8_t const g_5[8];
extern uint8_t const g_6[8];
extern uint8_t const g_7[8];
extern uint8_t const g_8[8];
extern uint8_t const g_9[8];
extern uint8_t const g_a[8];// <<---이거 추가
extern uint8_t const g_b[8];// <<---이거 추가
extern uint8_t const g_d[8];// <<---이거 추가
extern uint8_t const g_e[8];// <<---이거 추가
extern uint8_t const g_t[8];// <<---이거 추가

void appSetup() {
	HAL_TIM_Base_Start_IT(&htim6);
	demux_init();
	shift_reg_init();
	shift_reg_update();
}

// 아래 함수 추가
void updateRE() {
	static uint32_t last;
	static bool b_a_last;
	uint32_t now = HAL_GetTick();
	if (now - last > 10) {
		last = now;

		bool b_a_now = HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin);
		if (b_a_last == 1 && b_a_now == 0) {
			bool b_b_now = HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin);
			if (b_b_now) { //반시계방향으로 회전
				g_bBtnEvent = true;
			}
		}
		b_a_last = b_a_now;
	}
}

void appLoop() {
	if (g_updateDM) {
		g_updateDM = false;
		demux_update();
	}
	updateRE(); // 로터리 엔코터 첵크 추가
// <<---아래 코드 추가
	if (g_bBtnEvent) {
		g_bBtnEvent = false;
		for (int i = 0; i < 8; i++) {
			g_dm[0].ch[i] = g_d[i];
			g_dm[1].ch[i] = g_a[i];
			g_dm[2].ch[i] = g_t[i];
			g_dm[3].ch[i] = g_e[i];

			g_dm[4].ch[i] = g_2[i];
			g_dm[5].ch[i] = g_0[i];
			g_dm[6].ch[i] = g_2[i];
			g_dm[7].ch[i] = g_4[i];
			g_dm[8].ch[i] = g_0[i];
			g_dm[9].ch[i] = g_2[i];
			g_dm[10].ch[i] = g_0[i];
			g_dm[11].ch[i] = g_3[i];
		}
		composeDM();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		//writeFifo(&gFifo, TIME_OUT);
		g_updateDM = true;
	}
}
