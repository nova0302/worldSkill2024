/*
 * demux.c
 *
 *  Created on: Dec 27, 2023
 *      Author: sooho
 *      허수호 010 8581 1012
 *      서형준 010 9297 2837
 */
#include "main.h"
#include "shift_reg.h"
#include "dm.h"
#include "font.h"

void driveCom(uint8_t c, uint8_t b, uint8_t a) ;
void rowDrive();
void colDrive() ;
void rotateDM(uint8_t* ch);

uint8_t g_col;

DMx g_dm[12];

uint16_t g_charA[3][8] = {
  {
    //01234567 89012345
    0b0000000000000000,
    0b0000000000000000,
    0b1110111010101110,
    0b0100010011101000,
    0b0100010010101110,
    0b0100010010101000,
    0b0100111010101110,
    0b0000000000000000 },
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

void updateDM(){}

void initDM(){
 
  //rotateDM(t0);
  for(int i=0; i<8; i++)
    g_dm[0].ch[i] = g_t[i];

  //rotateDM(t1);
  for(int i=0; i<8; i++)
    g_dm[1].ch[i] = g_i[i];

  //rotateDM(t2);
  for(int i=0; i<8; i++)
    g_dm[2].ch[i] = g_m[i];

  //rotateDM(t3);
  for(int i=0; i<8; i++)
    g_dm[3].ch[i] = g_e[i];

}

void rotateDM(uint8_t* ch){
  uint8_t temp[8] = {0,};
  for (int i=0; i<8; ++i) {
    uint8_t t=0;
    for (int j=0; j<8; ++j) {
      t = t >> 1;
      t += (ch[j] << i) & 0x80;
    }
    temp[i] = t;
  }
  for(int i=0; i<8; i++)
    ch[i] = temp[i];
}

void composeDM(){
  uint8_t dm0[8], dm1[8];
  for (int i=0; i<8; ++i) {
    dm0[i]   = g_dm[0].ch[i]  + (g_dm[1].ch[i] >> 4);
    dm1[i]   = g_dm[2].ch[i]  + (g_dm[3].ch[i] >> 4);
  }
  rotateDM(dm0);
  rotateDM(dm1);

  for (int i=0; i<8; ++i) 
    g_charA[1][i] = ((uint16_t)dm0[i] << 8) + (uint16_t)dm1[i];
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
  static uint32_t last;
  static uint8_t sel;
  uint32_t now = HAL_GetTick();

  if (now - last > 0) {
    last = now;
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
  }
}

void driveCom(uint8_t c, uint8_t b, uint8_t a) {
  HAL_GPIO_WritePin(COM_A_GPIO_Port, COM_A_Pin, a);
  HAL_GPIO_WritePin(COM_B_GPIO_Port, COM_B_Pin, b);
  HAL_GPIO_WritePin(COM_C_GPIO_Port, COM_C_Pin, c);
}

void colDrive() {
  g_col++; g_col %= 3;
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
/*
  {
  0b0000 0000 0000 0000,
  0b0000 0000 0000 0000,
  0b1110 1110 1010 1110,
  0b0100 0100 1110 1000,
  0b0100 0100 1010 1110,
  0b0100 0100 1010 1000,
  0b0100 1110 1010 1110,
  0b0000 0000 0000 0000 },
  {
  0b0000000100000000,
  0b0000001110000000,
  0b0000011111000000,
  0b0000111111100000,
  0b0001111111110000,
  0b0011111111111000,
  0b0111111111111100,
  0b1111111111111110 },

  0b11111111 11111111,
  0b00001100 00000000,
  0b00001111 11000000,
  0b00001111 11100000,
  0b00001111 11100000,
  0b00001111 11100000,
  0b00000000 00110000,
  0b00000000 00110000
*/
/*
  void writeDM( DMx* dmx, char pos) {
  for(int i=0; i<8; i++){
  g_dmx[pos % 12].ch[i] = dmx->ch[i];
  g_dmx[pos % 12].color = dmx->color;
  }
  updateDM();
  }
*/

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

