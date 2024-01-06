/*
 * demux.c
 *
 *  Created on: Dec 27, 2023
 *      Author: sooho
 */
#include "main.h"
#include "shift_reg.h"

void driveCom(uint8_t c, uint8_t b, uint8_t a) ;

uint16_t g_charA[8] = {
		0b1110000011111111,
		0b0000000011111111,
		0b0000000011111111,
		0b0000000011111111,
		0b0000000011111111,
		0b0000000011111111,
		0b1110000011111111,
		0b1110000011111111
};
void rowDrive();
void colDrive();

void demux_init() {
	HAL_GPIO_WritePin(COM_D_GPIO_Port, COM_D_Pin, 0);
	HAL_GPIO_WritePin(COM_E_GPIO_Port, COM_E_Pin, 0);
}

void demux_update() {
	rowDrive();
}
void rowDrive() {
	static uint32_t last;
	static uint8_t sel;
	uint32_t now = HAL_GetTick();

	if (now - last > 0) {
		last = now;
		switch (sel++) {
		case 0:
			colDrive();
			driveCom(0, 0, 0); writeChar(g_charA[0]); break;
		case 1: driveCom(0, 0, 1); writeChar(g_charA[1]); break;
		case 2: driveCom(0, 1, 0); writeChar(g_charA[2]); break;
		case 3: driveCom(0, 1, 1); writeChar(g_charA[3]); break;
		case 4: driveCom(1, 0, 0); writeChar(g_charA[4]); break;
		case 5: driveCom(1, 0, 1); writeChar(g_charA[5]); break;
		case 6: driveCom(1, 1, 0); writeChar(g_charA[6]); break;
		case 7: driveCom(1, 1, 1); writeChar(g_charA[7]); break;
		default: break;
		}
		sel %= 8;
	}
}
void driveCom(uint8_t c, uint8_t b, uint8_t a) {
	HAL_GPIO_WritePin(COM_A_GPIO_Port, COM_A_Pin, a);
	HAL_GPIO_WritePin(COM_B_GPIO_Port, COM_B_Pin, b);
	HAL_GPIO_WritePin(COM_C_GPIO_Port, COM_C_Pin, c);
}

void colDrive() {
	static uint8_t col;
	switch (col++) {
	case 0:
		HAL_GPIO_WritePin(COM_D_GPIO_Port, COM_D_Pin, 0);
		HAL_GPIO_WritePin(COM_E_GPIO_Port, COM_E_Pin, 0);
		break;
	case 1:
		HAL_GPIO_WritePin(COM_D_GPIO_Port, COM_D_Pin, 1);
		HAL_GPIO_WritePin(COM_E_GPIO_Port, COM_E_Pin, 0);
		break;
	case 2:
		HAL_GPIO_WritePin(COM_D_GPIO_Port, COM_D_Pin, 0);
		HAL_GPIO_WritePin(COM_E_GPIO_Port, COM_E_Pin, 1);
		break;
	default:
		break;
	}
	col %= 3;
}
