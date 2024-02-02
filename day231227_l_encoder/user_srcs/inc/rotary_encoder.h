/*
 * rotary_encoder.h
 *
 *  Created on: Jan 14, 2024
 *      Author: SanglaeKim
 */

#ifndef INC_ROTARY_ENCODER_H_
#define INC_ROTARY_ENCODER_H_

#include "main.h"


typedef void (*RotaryEncoderCallback)(void);

typedef struct myGpio{
	GPIO_TypeDef* port;
	uint16_t pin;
}MyGpio_t;

typedef struct rotaryEncoder{
	MyGpio_t* pA;
	MyGpio_t* pB;
	uint32_t lastTime;
	uint32_t Delay;
	bool a_last;
	RotaryEncoderCallback cb;
}RotaryEncoder_t;

void updateRE();

void initRE(RotaryEncoder_t* re,
		GPIO_TypeDef* aPort, uint16_t aPin,
		GPIO_TypeDef* bPort, uint16_t bPin,
		uint32_t delay,
		RotaryEncoderCallback cb);

#endif /* INC_ROTARY_ENCODER_H_ */
