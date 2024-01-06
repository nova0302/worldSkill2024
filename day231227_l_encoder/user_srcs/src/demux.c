/*
 * demux.c
 *
 *  Created on: Dec 27, 2023
 *      Author: sooho
 */
#include "main.h"
#include "shift_reg.h"

void driveCom(uint8_t c, uint8_t b, uint8_t a) ;
typedef struct _DMx{
	char ch[8];
	uint8_t color;
}DMx;

DMx g_dmx[12];

void updateDM();
void intiDM(DMx* dm){
}

void writeDM( DMx* dmx, char pos) {
	for(int i=0; i<8; i++){
		g_dmx[pos % 12].ch[i] = dmx->ch[i];
		g_dmx[pos % 12].color = dmx->color;
	}
	updateDM();
}
void updateDM(){

}
/*
void rotateRight(){
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			t[i] += g[j] >> i & 0x1;
			t[i] = t[i] << 1;
		}
	}
}
*/


uint16_t g_charA[3][8] = {
		/*
		{
		0b1111111111111110,
		0b0111111111111100,
		0b0011111111111000,
		0b0001111111110000,
		0b0000111111100000,
		0b0000011111000000,
		0b0000001110000000,
		0b0000000100000000 },
{
		0b0000000100000000,
		0b0000001110000000,
		0b0000011111000000,
		0b0000111111100000,
		0b0001111111110000,
		0b0011111111111000,
		0b0111111111111100,
		0b1111111111111110 },
		*/
{
		0b1111111111111111,
		0b0000110000000000,
		0b0000111111000000,
		0b0000111111100000,
		0b0000111111100000,
		0b0000111111100000,
		0b0000000000110000,
		0b0000000000110000 },
{
		0b1111111111111111,
		0b0000110000000000,
		0b0000111111000000,
		0b0000111111100000,
		0b0000111111100000,
		0b0000111111100000,
		0b0000000000110000,
		0b0000000000110000 },
{
		0b1111111111111111,
		0b0000110000000000,
		0b0000111111000000,
		0b0000111111100000,
		0b0000111111100000,
		0b0000111111100000,
		0b0000000000110000,
		0b0000000000110000
}};

uint8_t g_col;
void rowDrive();
void colDrive(uint8_t col) ;

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
			g_col++; g_col %= 3;
			colDrive(g_col);
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

void colDrive(uint8_t col) {
	switch (col) {
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
