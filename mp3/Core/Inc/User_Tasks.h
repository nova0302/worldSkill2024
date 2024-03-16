#ifndef __TASKS_H
#define __TASKS_H

#include <stdio.h>
#include "main.h"
#include "stdbool.h"
#include "Control_Hardware.h"

#define numBtn (4)
typedef enum EepromBase{
	 DATE_TIME_OFFSET = 0
	,ALARM_SET_OFFSET = 32
	,SLEEP_MODE_OFFSET = 64
}EnEepomBase_t;
typedef enum SystemState { ST_SYS_INIT
	,ST_SYS_MAIN
	,ST_SYS_MENU_SEL
	,ST_SYS_SETTING_SLEEP
	,ST_SYS_SETTING_DATE_TIME
	,ST_SYS_SETTING_ALARM
	,ST_SYS_SLEEP_MODE
} EnSystemState_t;
typedef enum btnState { ST_IDLE, ST_DOWN, ST_SHORT_PRESSED, ST_LONG_PRESSED } EnBtnState_t;
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
	,EVT_ON_POW_SAVE
} EnBtnEvent_t;

typedef enum eTimeAction{ E_INCREASE, E_DECREASE }ETimeAction_t;

typedef struct{
	uint8_t head;
	uint8_t tail;
	EnBtnEvent_t eBtnEventBuf[256];
}StEventFifo_t;

typedef struct{
	uint16_t usYear;
	uint8_t	 ucMonth;
	uint8_t	 ucDay;
}StDate_t;

typedef struct{
	int8_t cHour;
	int8_t cMinute;
	int8_t cSecond;
}StTime_t;

typedef struct{
	StTime_t stTime;
	int8_t   curPos;
	bool     bEnable;
}StAlarmTime_t;

typedef struct{
	StDate_t stDate;
	StTime_t stTime;
	uint8_t  curPos;
}StDateTime_t;

typedef struct{
	StTime_t stTime;
	bool     bSleepMode;
	int8_t   curPos;
}StSleepMode_t;

typedef void (*callBack)();

typedef struct btnProcess {
	GPIO_TypeDef *port;
	uint16_t      pin;
	EnBtnState_t  btnState;
	uint32_t      last;    // time이전값
	bool          btnLast; // btn이전값
	callBack      cbShort;
	callBack      cbLong;
} btnProcess_t;

uint8_t getLastDayOfMonth(StDateTime_t *pStDateTime);

void loop(void);
void start_P() ;
void updateBtn(btnProcess_t *pBtn);
void initBtn(btnProcess_t *pBtn, GPIO_TypeDef *port, uint16_t pin, callBack cbShort, callBack cbLong);

void handleEvtMainMenu();
void handleEvtMenuSel();
void handleEvtSettingSleepMode();
void handleEvtSettingDateTime();
void handleEvtSettingAlarmSet();
void handleEvtPowSaveMode();

void showMainMenuEntryScreen();
void showManagementEntryScreen(uint8_t curPos);
void showDateTimeEntryScreen(StDateTime_t *pStDateTime);
void showAlarmSetEntryScreen(StAlarmTime_t *pStAlarmTime);
void showSleepModeEntryScreen();

void initDateTime(StDateTime_t *pStDateTime);
void incDateTime(StDateTime_t *pStDateTime);
void decDateTime(StDateTime_t *pStDateTime);
uint8_t getLastDayOfMonth(StDateTime_t *pStDateTime);

void incAlarmTime(StAlarmTime_t *pStAlarmTime);
void decAlarmTime(StAlarmTime_t *pStAlarmTime);

void btn1CbShort();
void btn1CbLong();
void btn2CbShort();
void btn2CbLong();
void btn3CbShort();
void btn3CbLong();
void btn4CbShort();
void btn4CbLong();

void printDbgMessage(uint8_t btnNumber, bool bIsShortPress);
void saveToEeprom(uint8_t *pData, size_t size, EnEepomBase_t base);
void loadFromEeprom(uint8_t *pData, size_t size, EnEepomBase_t base);
void incSleepMode();
void decSleepMode();
void setSleepMode();
void checkForPowerSave();
void initBtns(btnProcess_t *pBtn );

#endif
