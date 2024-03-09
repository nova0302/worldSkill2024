#ifndef __TASKS_H
#define __TASKS_H

#include <stdio.h>
#include "main.h"
#include "Control_Hardware.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void loop(void);

#endif
