/*
 * demux.c
 *
 *  Created on: Dec 27, 2023
 *      Author: sooho
 */
#include "main.h"
#include "shift_reg.h"

void driveCom(uint8_t c, uint8_t b, uint8_t a) ;

typedef struct _char2{
	uint8_t l;
	uint8_t r;
}char2;

char2 g_char2[8] = {
		{0b11111111, 0b11111110},
		{0b01111111, 0b11111100},
		{0b00111111, 0b11111000},
		{0b00011111, 0b11110000},
		{0b00001111, 0b11100000},
		{0b00000111, 0b11000000},
		{0b00000011, 0b10000000},
		{0b00000001, 0b00000000}};

uint16_t g_charA[3][8] = {{
		0b1111111111111110,
		0b0111111111111100,
		0b0011111111111000,
		0b0001111111110000,

		0b0000111111100000,
		0b0000011111000000,
		0b0000001110000000,
		0b0000000100000000 },
{
		0b1111111111110000,
		0b0111111111111000,
		0b0011111111111100,
		0b0001111111111110,
		0b0000111111111111,
		0b0000011111111111,
		0b0000001111111111,
		0b0000000111111111 },
{
		0b1111111100000000,
		0b0111111110000000,
		0b0011111111000000,
		0b0001111111100000,
		0b0000111111110000,
		0b0000011111111000,
		0b0000001111111100,
		0b0000000111111110
}};
uint8_t g_col;
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
		case 0: colDrive();
			    driveCom(0, 0, 0); writeChar(g_charA[g_col][0]); break;
		case 1: driveCom(0, 0, 1); writeChar(g_charA[g_col][1]); break;
		case 2: driveCom(0, 1, 0); writeChar(g_charA[g_col][2]); break;
		case 3: driveCom(0, 1, 1); writeChar(g_charA[g_col][3]); break;
		case 4: driveCom(1, 0, 0); writeChar(g_charA[g_col][4]); break;
		case 5: driveCom(1, 0, 1); writeChar(g_charA[g_col][5]); break;
		case 6: driveCom(1, 1, 0); writeChar(g_charA[g_col][6]); break;
		case 7: driveCom(1, 1, 1); writeChar(g_charA[g_col][7]); break;
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
	switch (g_col++) {
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
	g_col %= 3;
}
