/*
 * demux.c
 *
 *  Created on: Dec 27, 2023
 *      Author: sooho
 *      허수호 010 8581 1012
 *      서형준 010 9297 2837
 */
#include <stdbool.h>
#include "main.h"
#include "shift_reg.h"
#include "dm.h"
#include "font.h"

extern bool g_updateDM;
extern uint8_t g_col;
extern DMx g_dm[12];
extern uint16_t g_charA[3][8];

void driveCom(uint8_t c, uint8_t b, uint8_t a);
void rowDrive();
void colDrive();
void rotateDM(uint8_t *ch);

/*
 bool g_updateDM;
 uint8_t g_col;
 DMx g_dm[12];
 uint16_t g_charA[3][8] =
 {
 {
 //01234567 89012345
 0b0000000000000000, 0b0000000000000000, 0b1110111010101110,
 0b0100010011101000, 0b0100010010101110, 0b0100010010101000,
 0b0100111010101110, 0b0000000000000000 },
 { 0b1111111111111111, 0b0000110000000000, 0b0000111111000000,
 0b0000111111100000, 0b0000111111100000, 0b0000111111100000,
 0b0000000000110000, 0b0000000000110000 },
 { 0b1111111111111111, 0b0000110000000000, 0b0000111111000000,
 0b0000111111100000, 0b0000111111100000, 0b0000111111100000,
 0b0000000000110000, 0b0000000000110000 } };
 */

void updateDM() { }

void initDM() {

	for (int i = 0; i < 8; i++){
		g_dm[0].ch[i] = g_t[i];
		g_dm[1].ch[i] = g_i[i];
		g_dm[2].ch[i] = g_m[i];
		g_dm[3].ch[i] = g_e[i];

		g_dm[4].ch[i] = g_0[i];
		g_dm[5].ch[i] = g_1[i];
		g_dm[6].ch[i] = g_2[i];
		g_dm[7].ch[i] = g_3[i];
		g_dm[8].ch[i] = g_4[i];
		g_dm[9].ch[i] = g_5[i];
		g_dm[10].ch[i] = g_6[i];
		g_dm[11].ch[i] = g_7[i];
	}
}

void rotateDM(uint8_t *ch) {
	uint8_t temp[8] = { 0, };
	for (int i = 0; i < 8; ++i) {
		uint8_t t = 0;
		for (int j = 0; j < 8; ++j) {
			t = t >> 1;
			t += (ch[j] << i) & 0x80;
		}
		temp[i] = t;
	}
	for (int i = 0; i < 8; i++)
		ch[i] = temp[i];
}

void composeDM() {
	uint8_t dm0[8], dm1[8];
	for (int i = 0; i < 8; ++i) {
		dm0[i] = g_dm[0].ch[i] + (g_dm[1].ch[i] >> 4);
		dm1[i] = g_dm[2].ch[i] + (g_dm[3].ch[i] >> 4);
	}
	rotateDM(dm0);
	rotateDM(dm1);

	for (int i = 0; i < 8; ++i)
		g_charA[0][i] = ((uint16_t) dm0[i] << 8) + (uint16_t) dm1[i];
//////////////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < 8; ++i) {
		dm0[i] = g_dm[4].ch[i] + (g_dm[5].ch[i] >> 4);
		dm1[i] = g_dm[6].ch[i] + (g_dm[7].ch[i] >> 4);
	}
	rotateDM(dm0);
	rotateDM(dm1);

	for (int i = 0; i < 8; ++i)
		g_charA[1][i] = ((uint16_t) dm0[i] << 8) + (uint16_t) dm1[i];

//////////////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < 8; ++i) {
		dm0[i] = g_dm[8].ch[i] + (g_dm[9].ch[i] >> 4);
		dm1[i] = g_dm[10].ch[i] + (g_dm[11].ch[i] >> 4);
	}
	rotateDM(dm0);
	rotateDM(dm1);

	for (int i = 0; i < 8; ++i)
		g_charA[2][i] = ((uint16_t) dm0[i] << 8) + (uint16_t) dm1[i];
}

void demux_init() {
	HAL_GPIO_WritePin(COM_D_GPIO_Port, COM_D_Pin, 0);
	HAL_GPIO_WritePin(COM_E_GPIO_Port, COM_E_Pin, 0);
	initDM();
	composeDM();
}

void demux_update() {
	rowDrive();
}

void rowDrive() {
	//static uint32_t last;

	static uint8_t sel;
	//uint32_t now = HAL_GetTick();

	//if (now - last > 0) {
	// last = now;
	switch (sel++) {
	case 0:
		colDrive();
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
	//}
}

void driveCom(uint8_t c, uint8_t b, uint8_t a) {
	HAL_GPIO_WritePin(COM_A_GPIO_Port, COM_A_Pin, a);
	HAL_GPIO_WritePin(COM_B_GPIO_Port, COM_B_Pin, b);
	HAL_GPIO_WritePin(COM_C_GPIO_Port, COM_C_Pin, c);
}

void colDrive() {
	g_col++;
	g_col %= 3;
	switch (g_col) {
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
}
