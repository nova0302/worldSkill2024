#ifndef __TASKS_H
#define __TASKS_H

#include <stdio.h>
#include "main.h"
#include "stdbool.h"
#include "Control_Hardware.h"

void loop(void);

void btn1CbShort();
void btn1CbLong();
void btn2CbShort();
void btn2CbLong();
void btn3CbShort();
void btn3CbLong();
void btn4CbShort();
void btn4CbLong();
void printDbgMessage(uint8_t btnNumber, bool bIsShortPress);

#define numBtn (4)

typedef enum SystemState { ST_SYS_INIT
	,ST_SYS_MAIN
	,ST_SYS_MANAGEMENT
	,ST_SYS_POW_SAVE
	,ST_SYS_INIT3 } SystemState_t;

typedef enum btnState { ST_IDLE, ST_DOWN, ST_SHORT_PRESSED, ST_LONG_PRESSED } StBtnState_t;


typedef void (*callBack)();

typedef struct btnProcess {
	GPIO_TypeDef *port;
	uint16_t      pin;
	StBtnState_t  btnState;
	uint32_t      last;    // time이전값
	bool          btnLast; // btn이전값
	callBack      cbShort;
	callBack      cbLong;
} btnProcess_t;

void updateBtn(btnProcess_t *pBtn);
void initBtn(btnProcess_t *pBtn, GPIO_TypeDef *port, uint16_t pin, callBack cbShort, callBack cbLong);
void updatMainMenu();
void updatManagementMenu();
void start_P() ;


#endif
