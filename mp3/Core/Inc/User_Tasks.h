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
	,ST_SYS_DATE_TIME } SystemState_t;

typedef enum btnState { ST_IDLE, ST_DOWN, ST_SHORT_PRESSED, ST_LONG_PRESSED } StBtnState_t;

typedef enum eBtnEvent {
	 EVT_BTN1_SHORT_PRESS
	,EVT_BTN1_LONG_PRESS

	,EVT_BTN2_SHORT_PRESS
	,EVT_BTN2_LONG_PRESS

	,EVT_BTN3_SHORT_PRESS
	,EVT_BTN3_LONG_PRESS

	,EVT_BTN4_SHORT_PRESS
	,EVT_BTN4_LONG_PRESS
	,EVT_IS_ENTRY
} eBtnEvent_t;

typedef struct{
	uint8_t head;
	uint8_t tail;
	eBtnEvent_t eBtnEventBuf[256];
}StEventFifo_t;

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
void updateMainMenu();
void updateManagementMenu();
void updatePowerSave();
void start_P() ;
void showManagementEntryScreen(uint8_t curPos);
void showDateTimeEntryScreen();
void updateDateTime();


#endif
