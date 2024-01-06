/*
 * shift_reg.c
 *
 *  Created on: Dec 27, 2023
 *      Author: sooho
 */
#include "main.h"
#include "shift_reg.h"

void shift_reg_init(){
	HAL_GPIO_WritePin(RED_SCLK_GPIO_Port, RED_SCLK_Pin, 0);
	HAL_GPIO_WritePin(RED_DATA_GPIO_Port, RED_DATA_Pin, 0);
	HAL_GPIO_WritePin(RED_LCLK_GPIO_Port, RED_LCLK_Pin, 0);
	writeChar( (uint16_t)0);
	writeCharR((uint16_t)0);
}

void shift_reg_update(){
	HAL_GPIO_WritePin(GRN_DATA_GPIO_Port, GRN_DATA_Pin, 1);
	for(int i=0; i<16; i++){
		HAL_GPIO_TogglePin(GRN_SCLK_GPIO_Port, GRN_SCLK_Pin);
		HAL_Delay(1);
		HAL_GPIO_TogglePin(GRN_SCLK_GPIO_Port, GRN_SCLK_Pin);
		HAL_Delay(1);
	}
	HAL_GPIO_TogglePin(GRN_LCLK_GPIO_Port, GRN_LCLK_Pin);
	HAL_Delay(1);
	HAL_GPIO_TogglePin(GRN_LCLK_GPIO_Port, GRN_LCLK_Pin);
	HAL_Delay(1);
}

void writeChar(uint16_t c){
	uint16_t ch = c;
	for(int i=0; i<16; i++){
		HAL_GPIO_WritePin( GRN_DATA_GPIO_Port, GRN_DATA_Pin, ch & 0x1);
		HAL_GPIO_TogglePin(GRN_SCLK_GPIO_Port, GRN_SCLK_Pin);
		HAL_GPIO_TogglePin(GRN_SCLK_GPIO_Port, GRN_SCLK_Pin);
		ch = ch >> 1;
	}
	HAL_GPIO_TogglePin(GRN_LCLK_GPIO_Port, GRN_LCLK_Pin);
	HAL_GPIO_TogglePin(GRN_LCLK_GPIO_Port, GRN_LCLK_Pin);
}

void writeCharR(uint16_t c){
	uint16_t ch = c;
	for(int i=0; i<16; i++){
		HAL_GPIO_WritePin( RED_DATA_GPIO_Port, RED_DATA_Pin, ch & 0x1);
		HAL_GPIO_TogglePin(RED_SCLK_GPIO_Port, RED_SCLK_Pin);
		HAL_GPIO_TogglePin(RED_SCLK_GPIO_Port, RED_SCLK_Pin);
		ch = ch >> 1;
	}
	HAL_GPIO_TogglePin(RED_LCLK_GPIO_Port, RED_LCLK_Pin);
	HAL_GPIO_TogglePin(RED_LCLK_GPIO_Port, RED_LCLK_Pin);
}
