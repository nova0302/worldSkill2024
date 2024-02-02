/*
 * rotary_encoder.c
 *
 *  Created on: Jan 14, 2024
 *      Author: SanglaeKim
 */
#include <stdbool.h>
#include "dm.h"
#include "rotary_encoder.h"

extern uint32_t g_counter;
extern DMx g_dm[12];

extern  uint8_t const g_0[8];
extern  uint8_t const g_1[8];
extern  uint8_t const g_2[8];
extern  uint8_t const g_3[8];
extern  uint8_t const g_4[8];
extern  uint8_t const g_5[8];
extern  uint8_t const g_6[8];
extern  uint8_t const g_7[8];
extern  uint8_t const g_8[8];
extern  uint8_t const g_9[8];
extern  uint8_t const g_nums[10][8];


bool readGpioPin(MyGpio_t* pGpio){
		bool a_now = HAL_GPIO_ReadPin(pGpio->port, pGpio->pin);
		return a_now;
}

void initRE(RotaryEncoder_t* re,
		GPIO_TypeDef* aPort, uint16_t aPin,
		GPIO_TypeDef* bPort, uint16_t bPin,
		uint32_t delay,
		RotaryEncoderCallback cb){
	re->lastTime = 0;
	re->a_last = 0;
	re->Delay = delay;

	re->pA->port = aPort;
	re->pA->pin = aPin;

	re->pB->port = bPort;
	re->pB->pin = bPin;

	re->cb = cb;
}

void updateRE(RotaryEncoder_t* re) {

	uint32_t now = HAL_GetTick();
	if(now - re->lastTime > re->Delay){
		re->lastTime = now;
		bool a_now = readGpioPin(re->pA);
		if(re->a_last == true && a_now == false){
			bool b_now = readGpioPin(re->pB);
			re->cb();
		}
	}

	/*
	static uint32_t last;
	static bool a_last;
	if (now - last > 10) {
		bool a_now = HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin);
		if (a_last == true && a_now == false) { // faling edge
			bool b_now = HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin);
			if (b_now)
				g_counter++;
			else
				g_counter--;
			for (int i = 0; i < 8; i++)
				g_dm[4].ch[i] = g_nums[g_counter % 10][i];
		}
		last = now;
		a_last = a_now;
	}
	*/
}
