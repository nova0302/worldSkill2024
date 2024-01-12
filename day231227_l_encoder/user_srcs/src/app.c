/*
 * app.c
 *
 *  Created on: Dec 27, 2023
 *      Author: sooho
 */
#include <stdbool.h>
#include "main.h"
#include "demux.h"
#include "shift_reg.h"

extern bool g_updateDM;
extern TIM_HandleTypeDef htim6;

void appSetup () {
  HAL_TIM_Base_Start_IT(&htim6);
  demux_init ();
  shift_reg_init ();
  shift_reg_update ();
}

void appLoop () {
  if (g_updateDM) {
    g_updateDM = false;
    demux_update ();
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){
  if(htim->Instance == TIM6){
    //writeFifo(&gFifo, TIME_OUT);
    g_updateDM = true;
  }
}
