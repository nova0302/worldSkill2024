#include "OLED_SPI.h"

uint8_t Display_Buffer_Cheak[8][128] = {0x00};  // [y][x]
uint8_t Display_Buffer[8][128] = {0x00};        // [y][x]

void OLED_SPI_Write(uint8_t data, uint8_t mode)
{
  OLED_DC_GPIO_Port->BSRR = OLED_DC_Pin << (mode ? 0 : 16);

  for(uint8_t i=0; i<8; i++)
  {
    OLED_D0_GPIO_Port->BRR = OLED_D0_Pin;
    OLED_D1_GPIO_Port->BSRR = OLED_D1_Pin << ((data & (0x80 >> i)) ? 0 : 16);
    OLED_D0_GPIO_Port->BSRR = OLED_D0_Pin;
  }
}

void OLED_Clear(void)
{
  for(uint8_t y=0; y<8; y++)
  {
    OLED_SPI_Write(0xB0 + y, OLED_CMD);
    OLED_SPI_Write(0x00, OLED_CMD);
    OLED_SPI_Write(0x10, OLED_CMD);
    for(uint8_t x=0; x<128; x++) OLED_SPI_Write(0x00, OLED_DATA);
  }
}

void OLED_Init(void)
{
  OLED_RES_GPIO_Port->BRR = OLED_RES_Pin;
  HAL_Delay(10);
  OLED_RES_GPIO_Port->BSRR = OLED_RES_Pin;

  //OLED_SPI_Write(0xAE, OLED_CMD); // Set Display Off, Default: 0xAE
  //OLED_SPI_Write(0xD5, OLED_CMD); // Set Display Clock Divide Ratio/Oscillator Frequency
  //OLED_SPI_Write(0x80, OLED_CMD); // Default: 0x80
  //OLED_SPI_Write(0xA8, OLED_CMD); // Set Multiplex Ratio
  //OLED_SPI_Write(0x3F, OLED_CMD); // Default: 0x3F
  //OLED_SPI_Write(0xD3, OLED_CMD); // Set Display Offset
  //OLED_SPI_Write(0x00, OLED_CMD); // Default: 0x00
  //OLED_SPI_Write(0x40, OLED_CMD); // Set Display Start Line, Default: 0x40
  OLED_SPI_Write(0x8D, OLED_CMD); // Set Charge Pump
  OLED_SPI_Write(0x14, OLED_CMD); // Default: 0x10
  OLED_SPI_Write(0xA1, OLED_CMD); // Set Segment Re-Map, Default: 0xA0
  OLED_SPI_Write(0xC8, OLED_CMD); // Set COM Output Scan Direction, Default: 0xC0
  //OLED_SPI_Write(0xDA, OLED_CMD); // Set COM Pins Hardware Configuration
  //OLED_SPI_Write(0x12, OLED_CMD); // Default: 0x12
  OLED_SPI_Write(0x81, OLED_CMD); // Set Contrast Control
  OLED_SPI_Write(0xCF, OLED_CMD); // Default: 0x7F
  OLED_SPI_Write(0xD9, OLED_CMD); // Set Pre-Charge Period
  OLED_SPI_Write(0xF1, OLED_CMD); // Default: 0x22
  OLED_SPI_Write(0xDB, OLED_CMD); // Set VCOMH Deselect Level
  OLED_SPI_Write(0x40, OLED_CMD); // Default: 0x20
  //OLED_SPI_Write(0xA4, OLED_CMD); // Set Entire Display On/Off, Default: 0xA4
  //OLED_SPI_Write(0xA6, OLED_CMD); // Set Normal/Inverse Display, Default: 0xA6
  OLED_SPI_Write(0xAF, OLED_CMD); // Set Display On, Default: 0xAE

  OLED_Clear();
  HAL_Delay(100);
}

void OLED_Set_Pos(uint8_t x, uint8_t y)
{
  OLED_SPI_Write(0xB0 + y, OLED_CMD);
  OLED_SPI_Write((x & 0x0F), OLED_CMD);
  OLED_SPI_Write(((x & 0xF0) >> 4) | 0x10, OLED_CMD);
}

void OLED_Display(void)
{
  for(uint8_t x=0; x<128; x++)
  {
    for(uint8_t y=0; y<8; y++)
    {
      if(Display_Buffer_Cheak[y][x] != Display_Buffer[y][x])
      {
        Display_Buffer_Cheak[y][x] = Display_Buffer[y][x];
        OLED_Set_Pos(x, y);
        OLED_SPI_Write(Display_Buffer[y][x], OLED_DATA);
      }
    }
  }
}

void OLED_Buffer_Clear(void)
{
  memset(Display_Buffer, 0x00, sizeof(Display_Buffer));
}

// type: 0 - Erase, 1 - Write, 2 - Toggle
void OLED_Set_Point(uint8_t x, uint8_t y, uint8_t type)
{
  if(x<128 && y<64)
  {
    if(type == 0) Display_Buffer[y/8][x] &= ~(0x01 << (y%8));
    if(type == 1) Display_Buffer[y/8][x] |= 0x01 << (y%8);
    if(type == 2) Display_Buffer[y/8][x] ^= 0x01 << (y%8);
  }
}

void OLED_Line(int x_start, int y_start, int x_end, int y_end, uint8_t type)
{
  int t;
  int xerr = 0;
  int yerr = 0;
  int deltax, deltay, distance;
  int incx, incy, uRow, uCol;

  deltax = x_end - x_start;
  deltay = y_end - y_start;
  uRow = x_start;
  uCol = y_start;
  if (deltax > 0){
    incx = 1;
  }else if (deltax == 0){
    incx = 0;
  }else {
    incx = -1;
    deltax = -deltax;
  }
  if (deltay > 0){
    incy = 1;
  }else if (deltay == 0){
    incy = 0;
  }else {
    incy = -1;
    deltay = -deltay;
  }
  if (deltax > deltay){
    distance = deltax;
  }else {
    distance = deltay;
  }
  for (t = 0; t <= distance+1; t++){
    OLED_Set_Point(uRow, uCol, type);
    xerr += deltax;
    yerr += deltay;
    if (xerr > distance){
      xerr -= distance;
      uRow += incx;
    }
    if (yerr > distance){
      yerr -= distance;
      uCol += incy;
    }
  }
}

// type: 0 - Erase, 1 - Write, 2 - Toggle
void OLED_Fill(uint8_t X_Start,uint8_t Y_Start,uint8_t X_End,uint8_t Y_End,uint8_t Type)
{
  for (uint8_t x = X_Start; x <= X_End; x++)
    for (uint8_t y = Y_Start; y <= Y_End; y++)
      OLED_Set_Point(x, y, Type);
}

// type: 0 - Erase, 1 - Write, 2 - Toggle
void OLED_Circle(int x_start, int y_start, int r, uint8_t type)
{
  int x = r;
  int y = 0;
  int F = -2*r+3;
  while(x >= y)
  {
    OLED_Set_Point(x_start+x, y_start+y, type);
    OLED_Set_Point(x_start-x, y_start+y, type);
    OLED_Set_Point(x_start+x, y_start-y, type);
    OLED_Set_Point(x_start-x, y_start-y, type);
    OLED_Set_Point(x_start+y, y_start+x, type);
    OLED_Set_Point(x_start-y, y_start+x, type);
    OLED_Set_Point(x_start+y, y_start-x, type);
    OLED_Set_Point(x_start-y, y_start-x, type);
    if(F >= 0)
    {
      x--;
      F -= 4*x;
    }
    y++;
    F += 4*y+2;
  }
}

// invert: 0 - None, 1 - Invert
void OLED_Show_Str(uint8_t x, uint8_t y, char *str, Font_Size size, uint8_t invert)
{
  uint8_t ch, i, j;
  uint8_t size_x, size_y, space_x;
  uint16_t data, shift;

  if(size == Font8x13)
  {
    size_x = 8;
    size_y = 13;
    space_x = 0;
    shift = 0x80;
  }
  else if(size == Font12x22)
  {
    size_x = 12;
    size_y = 22;
    space_x = 3;
    shift = 0x8000;
  }
  else return;

  while (*str != '\0')
  {
    if(x > 128-size_x) break;
    if(y > 64-size_y) break;

    if(size == Font8x13) ch = (*str - ' ')<95 ? (*str - ' ') : 0;
    if(size == Font12x22) ch = (*str - '0')<11 ? (*str - '0') : 0;
    str++;

    for(i=0; i<size_y; i++)
    {
      for(j=0; j<size_x; j++)
      {
        if(size == Font8x13) data = F8X13[ch][i];
        if(size == Font12x22) data = F12X22[ch][i];
        if(invert) data = ~data;
        OLED_Set_Point(x+j, y+i, (data & (shift>>j)) ? 1 : 0);
      }
    }

    x += size_x+space_x;
  }
}

void OLED_Show_Picture(uint8_t Show_X, uint8_t Show_Y, uint8_t Picture_X, uint8_t Picture_Y, const uint8_t *Picture)
{
  Picture_X = (Picture_X / 8) + ((Picture_X % 8) ? 1 : 0);
  for(uint8_t y=0; y<Picture_Y; y++)
  {
    for(uint8_t x=0; x<Picture_X; x++)
    {
      for(uint8_t i=0; i<8; i++)
      {
        if(Picture[(y * Picture_X) + x] & (0x80 >> i)) OLED_Set_Point(Show_X + x * 8 + i,y + Show_Y, 1);
        else OLED_Set_Point(Show_X + x * 8 + i,y + Show_Y, 0);
      }
    }
  }
}
