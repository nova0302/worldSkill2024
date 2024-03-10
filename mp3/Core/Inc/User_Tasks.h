#ifndef __TASKS_H
#define __TASKS_H

#include <stdio.h>
#include "main.h"
#include "stdbool.h"
#include "Control_Hardware.h"

#define numBtn (4)

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

typedef enum SystemState { ST_SYS_INIT
	,ST_SYS_MAIN
	,ST_SYS_MANAGEMENT
	,ST_SYS_POW_SAVE
	,ST_SYS_DATE_TIME
	,ST_SYS_ALARM_SET
} SystemState_t;

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

typedef enum eTimeAction{
	E_INCREASE, E_DECREASE
}ETimeAction_t;
typedef struct{
	uint8_t head;
	uint8_t tail;
	eBtnEvent_t eBtnEventBuf[256];
}StEventFifo_t;

typedef struct{
	uint16_t usYear;
	uint8_t	 ucMonth;
	uint8_t	 ucDay;
}StDate_t;

typedef struct{
	uint8_t ucHour;
	uint8_t ucMinute;
	uint8_t ucSecond;
}StTime_t;

typedef struct{
	StDate_t stDate;
	StTime_t stTime;
	uint8_t curPos;
}StDateTime_t;

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

uint8_t getLastDayOfMonth(StDateTime_t *pStDateTime);
void incDateTime(StDateTime_t *pStDateTime);

void updateBtn(btnProcess_t *pBtn);
void initBtn(btnProcess_t *pBtn, GPIO_TypeDef *port, uint16_t pin, callBack cbShort, callBack cbLong);
void start_P() ;

void updateMainMenu();
void updateManagementMenu();
void updatePowerSave();
void updateDateTime();
void updateAlarmSet();
void updateSleepMode();

void showMainMenuEntryScreen();
void showManagementEntryScreen(uint8_t curPos);
void showDateTimeEntryScreen(StDateTime_t *pStDateTime);
void showAlarmSetEntryScreen();
void showSleepModeEntryScreen();


#endif
