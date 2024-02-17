#ifndef __OLED_SPI_H
#define __OLED_SPI_H

#include <string.h>
#include "main.h"
#include "Fonts.h"

enum
{
  OLED_CMD,
  OLED_DATA
};

typedef enum
{
  Font8x13,
  Font12x22
}Font_Size;

extern uint8_t Display_Buffer_Cheak[8][128];
extern uint8_t Display_Buffer[8][128];

void OLED_SPI_Write(uint8_t data, uint8_t mode);
void OLED_Clear(void);
void OLED_Init(void);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_Display(void);
void OLED_Buffer_Clear(void);
void OLED_Set_Point(uint8_t x, uint8_t y, uint8_t type);
void OLED_Line(int x_start, int y_start, int x_end, int y_end, uint8_t type);
void OLED_Fill(uint8_t X_Start,uint8_t Y_Start,uint8_t X_End,uint8_t Y_End,uint8_t Type);
void OLED_Circle(int x_start, int y_start, int r, uint8_t type);
void OLED_Show_Str(uint8_t x, uint8_t y, char *str, Font_Size size, uint8_t invert);
void OLED_Show_Picture(uint8_t Show_X, uint8_t Show_Y, uint8_t Picture_X, uint8_t Picture_Y, const uint8_t *Picture);

#endif
