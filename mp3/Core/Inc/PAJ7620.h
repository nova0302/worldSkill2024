#ifndef __PAJ7620_H
#define __PAJ7620_H

#include "main.h"

#define PAJ7620_ID  0xE6// DEVICE ID

// REGISTER BANK SELECT
#define PAJ7620_REGITER_BANK_SEL 0xEF
#define PAJ7620_BANK0 0x00
#define PAJ7620_BANK1 0x01

typedef enum
{
  None            = 0x00,
  Up              = 0x08,
  Down            = 0x04,
  Left            = 0x01,
  Right           = 0x02,
  Forward         = 0x10,
  Backward        = 0x20,
  Clockwise       = 0x40,
  Count_Clockwise = 0x80,
  WAVE            = 0x11
}PAJ7620_Dir_Type;

typedef enum
{
  BANK0 = 0x00,
  BANK1,
}Bank_Type;

PAJ7620_Dir_Type PAJ7620_Get_Dir(void);

void PAJ7620_Init(void);
void PAJ7620_Write_Reg(uint8_t Addr,uint8_t Cmd);
void PAJ7620_Read_Reg(uint8_t Addr,uint8_t Num,uint8_t *Data);
void PAJ7620_Select_Bank(Bank_Type Bank);

#endif
