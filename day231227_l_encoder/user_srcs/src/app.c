/*
 * app.c
 *
 *  Created on: Dec 27, 2023
 *      Author: sooho
 */
#include "main.h"
#include "demux.h"
#include "shift_reg.h"

void appSetup() {
	demux_init();
	shift_reg_init();
	shift_reg_update();
}

void appLoop() {
	demux_update();

}

