#include "Delay.h"

void Delay_us(uint32_t us)
{
  while(us--)
  {
    __NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();
  }
}
