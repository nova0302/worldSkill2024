#include <stdio.h>
#include "User_Tasks.h"

uint32_t Tr_all = 0;
uint32_t Tr_now = 0;

char str_buf[20];
uint32_t sw_tick = 0, idle_tick = 0;
uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0, Sleep_Sec = 0;
uint8_t sw_flag = 0,  sw_buf = 0, sw_it = 0;
uint8_t sw_alarm_flag = 0;

bool     g_bUpdateScreen = false;
uint8_t  g_CurLoc = 0; // 첫번제 칸
uint16_t g_trackNo;
uint16_t g_numTotalTrack;
SystemState_t g_SystemState;
StEventFifo_t g_StEventFifo;

void initQueue(StEventFifo_t *pFifo){ pFifo->head = pFifo->tail = 0; }
void enqueue(StEventFifo_t *pFifo, eBtnEvent_t btnEvent) {
	pFifo->eBtnEventBuf[pFifo->head++] = btnEvent;
}
bool dequeue(StEventFifo_t *pFifo, eBtnEvent_t *pBtnEvent) {
	if (pFifo->head == pFifo->tail) {
		return false;
	}else{
		*pBtnEvent = pFifo->eBtnEventBuf[pFifo->tail++];
		return true;
	}
}
void Volume_Up() {
	if (Volume < 30) Volume += 1;
	DFSetVolume(Volume);
	eeprom_8bit_write(0, Volume);
}
void Volume_Down() {
	if (Volume) Volume--;
	DFSetVolume(Volume);
	eeprom_8bit_write(0, Volume);
}
void Alarm_Handler() {

}
void initApp() {
	start_P();
	g_numTotalTrack = DFGetTotalTrack();

	// EepromLoad();
	g_SystemState = ST_SYS_MAIN;
	enqueue(&g_StEventFifo, EVT_IS_ENTRY);
}
void loop(void) {
	initApp();
	btnProcess_t btn[numBtn];
	initBtn(&btn[0], SW1_GPIO_Port, SW1_Pin, btn1CbShort, btn1CbLong);
	initBtn(&btn[1], SW2_GPIO_Port, SW2_Pin, btn2CbShort, btn2CbLong);
	initBtn(&btn[2], SW3_GPIO_Port, SW3_Pin, btn3CbShort, btn3CbLong);
	initBtn(&btn[3], SW4_GPIO_Port, SW4_Pin, btn4CbShort, btn4CbLong);
	// 윤년 && eeprom?
	while (1) {
		for(int i = 0; i < numBtn; ++i) updateBtn(&btn[i]);
		switch (g_SystemState) {
		case ST_SYS_MAIN:       updateMainMenu();       break;
		case ST_SYS_MANAGEMENT: updateManagementMenu(); break;
		case ST_SYS_POW_SAVE:   updatePowerSave();      break;
		case ST_SYS_DATE_TIME:  updateDateTime();      break;
		case ST_SYS_ALARM_SET:  updateAlarmSet();      break;
		default: break;
		}
	}
}
void updateMainMenu() {
	eBtnEvent_t BtnEvent;
	if (!dequeue(&g_StEventFifo, &BtnEvent)) return;
	switch (BtnEvent) {
		case EVT_IS_ENTRY:
			OLED_Buffer_Clear(); showMainMenuEntryScreen();  break;
		case EVT_BTN1_SHORT_PRESS: DFPlayNextTrack();          break;
		case EVT_BTN1_LONG_PRESS : DFPlayThisTrack(g_trackNo); break;
		case EVT_BTN2_SHORT_PRESS: DFPlayPreviousTrack();      break;
		case EVT_BTN2_LONG_PRESS : DFPause();                  break;
		case EVT_BTN3_SHORT_PRESS: Volume_Up();                break;
		case EVT_BTN3_LONG_PRESS : g_SystemState = ST_SYS_POW_SAVE; break;
		case EVT_BTN4_SHORT_PRESS: Volume_Down();              break;
		case EVT_BTN4_LONG_PRESS :
			enqueue(&g_StEventFifo, EVT_IS_ENTRY);
			g_SystemState = ST_SYS_MANAGEMENT; break;
		default: break;
	}
}
void updateAlarmSet() {
	eBtnEvent_t BtnEvent;
	if (!dequeue(&g_StEventFifo, &BtnEvent)) return;
	switch (BtnEvent) {
		case EVT_IS_ENTRY:
			OLED_Buffer_Clear(); showAlarmSetEntryScreen();  break;
		case EVT_BTN1_SHORT_PRESS: break;
		case EVT_BTN2_SHORT_PRESS: break;
		case EVT_BTN3_SHORT_PRESS: break;
		case EVT_BTN4_SHORT_PRESS:
			enqueue(&g_StEventFifo, EVT_IS_ENTRY);
			g_SystemState = ST_SYS_MANAGEMENT; break;
		default: break;
	}
}
void updateManagementMenu() {
	static uint8_t curPos = 0;
	eBtnEvent_t BtnEvent;
	if (!dequeue(&g_StEventFifo, &BtnEvent)) return;
	switch (BtnEvent) {
		case EVT_IS_ENTRY:
			curPos = 0;
			OLED_Buffer_Clear();
			showManagementEntryScreen(curPos);
			break;
		case EVT_BTN1_SHORT_PRESS:
			if (--curPos < 0) curPos = 2;
			showManagementEntryScreen(curPos);
			break;
		case EVT_BTN2_SHORT_PRESS:
			if (++curPos > 2) curPos = 0;
			showManagementEntryScreen(curPos);
			break;
		case EVT_BTN3_SHORT_PRESS:
			enqueue(&g_StEventFifo, EVT_IS_ENTRY);
			g_SystemState = ST_SYS_DATE_TIME; break;
			break;
		case EVT_BTN4_SHORT_PRESS:
			enqueue(&g_StEventFifo, EVT_IS_ENTRY);
			g_SystemState = ST_SYS_MAIN; break;
		default: break;
	}
}
void updateDateTime(){
	eBtnEvent_t BtnEvent;
	if (!dequeue(&g_StEventFifo, &BtnEvent)) return;
	switch (BtnEvent) {
		case EVT_IS_ENTRY:
			OLED_Buffer_Clear();
			showDateTimeEntryScreen();
			break;
		case EVT_BTN1_SHORT_PRESS: showDateTimeEntryScreen(); break;
		case EVT_BTN2_SHORT_PRESS: break;
		case EVT_BTN3_SHORT_PRESS: break;
		case EVT_BTN4_SHORT_PRESS:
			enqueue(&g_StEventFifo, EVT_IS_ENTRY);
			g_SystemState = ST_SYS_MANAGEMENT; break;
		default: break;
	}
}
void updatePowerSave(){
	eBtnEvent_t BtnEvent;
	if (!dequeue(&g_StEventFifo, &BtnEvent)) return;
	switch (BtnEvent) {
		case EVT_BTN1_SHORT_PRESS: break;
		case EVT_BTN2_SHORT_PRESS: break;
		case EVT_BTN3_SHORT_PRESS: break;
		case EVT_BTN4_SHORT_PRESS: break;
		default: break;
	}
}
void initBtn(btnProcess_t *pBtn, GPIO_TypeDef *port, uint16_t pin, callBack cbShort, callBack cbLong) {
	pBtn->port = port;
	pBtn->pin = pin;
	pBtn->btnState = ST_IDLE;
	pBtn->last = 0;
	pBtn->cbShort = cbShort;
	pBtn->cbLong = cbLong;
}
void updateBtn(btnProcess_t *pBtn) {

	uint32_t now = HAL_GetTick();

	switch (pBtn->btnState) {

	case ST_IDLE: // falling edge check
		if (now - pBtn->last > 10) {
			bool btnNow = HAL_GPIO_ReadPin(pBtn->port, pBtn->pin);
			if (pBtn->btnLast == 1 && btnNow == 0)
				pBtn->btnState = ST_DOWN;
			pBtn->last = now;
			pBtn->btnLast = btnNow;
		}
		break;
	case ST_DOWN:
		bool btnNow = HAL_GPIO_ReadPin(pBtn->port, pBtn->pin);
		uint32_t delayAmount = 3000;
		if (btnNow)
			pBtn->btnState = ST_SHORT_PRESSED;
		else if (now - pBtn->last > delayAmount)
			pBtn->btnState = ST_LONG_PRESSED;
		break;
	case ST_SHORT_PRESSED:
		pBtn->cbShort();
		pBtn->btnState = ST_IDLE;
		break;
	case ST_LONG_PRESSED:
		pBtn->cbLong();
		pBtn->btnState = ST_IDLE;
		break;
	}
}
void btn1CbShort() { enqueue(&g_StEventFifo, EVT_BTN1_SHORT_PRESS); }
void btn1CbLong() {  enqueue(&g_StEventFifo, EVT_BTN1_LONG_PRESS); }
void btn2CbShort() { enqueue(&g_StEventFifo, EVT_BTN2_SHORT_PRESS); }
void btn2CbLong() {  enqueue(&g_StEventFifo, EVT_BTN2_LONG_PRESS); }
void btn3CbShort() { enqueue(&g_StEventFifo, EVT_BTN3_SHORT_PRESS); }
void btn3CbLong() {  enqueue(&g_StEventFifo, EVT_BTN3_LONG_PRESS); }
void btn4CbShort() { enqueue(&g_StEventFifo, EVT_BTN4_SHORT_PRESS); }
void btn4CbLong() {  enqueue(&g_StEventFifo, EVT_BTN4_LONG_PRESS); }
void printDbgMessage(uint8_t btnNumber, bool bIsShortPress) {
	char msgBuf[64];
	sprintf(msgBuf, "btn%d %s pressed!!", btnNumber, bIsShortPress ? "short" : "long");
	OLED_Show_Str(0, 0, msgBuf, Font8x13, 0);
	OLED_Display();
}
void showManagementEntryScreen(uint8_t curPos){
	OLED_Show_Str(0 , 0,      "Options",       Font8x13, 0);
	OLED_Show_Str(0, 64-13*3, "Date/Time Set", Font8x13, curPos == 0);
	OLED_Show_Str(0, 64-13*2, "Alarm Set",     Font8x13, curPos == 1);
	OLED_Show_Str(0, 64-13*1, "Sleep Mode Set", Font8x13, curPos == 2);
	OLED_Display();
}
void showMainMenuEntryScreen(){
	OLED_Clear();
	OLED_Show_Picture(0, 0, 16, 16, Dir_Icon);
	OLED_Show_Picture(100, 0, 16, 16, Alarm_Icon);
	OLED_Show_Picture(120, 0, 8, 16, Battry_Icon);

	sprintf(str_buf, "VOL:%02d", Volume);
	OLED_Show_Str(40, 3, str_buf, Font8x13, 0);

	//sprintf(str_buf, "Track:%02ld/%02ld", Tr_now, Tr_all);
	sprintf(str_buf, "Track:%02ld/%02ld", Tr_now, g_numTotalTrack);
	OLED_Show_Str(20, 19, str_buf, Font8x13, 0);
	OLED_Display();
}
void showDateTimeEntryScreen(){
	char msgBuf[32];
	uint16_t year=2000, month=3, day=10;
	uint16_t h=11, m=33, s=33;
	OLED_Show_Str(0 , 0,      "DATE/TIME SET", Font8x13, 0);

	sprintf(msgBuf, "%4d - %02d - %02d", year, month, day);
	OLED_Show_Str(0, 64-13*3, msgBuf, Font8x13, 0);

	sprintf(msgBuf, "%d : %d : %d", h, m, s);
	OLED_Show_Str(0, 64-13*2, msgBuf,     Font8x13, 1);
	OLED_Display();
}
void showAlarmSetEntryScreen(){
	char msgBuf[32];
	uint16_t h=11, m=33;
	OLED_Show_Str(0 , 0,      "SLEEP MODE SET", Font8x13, 0);
	OLED_Show_Str(0, 64-13*3, "Enable: ON", Font8x13, 0);
	sprintf(msgBuf, "%d : %d", h, m);
	OLED_Show_Str(0, 64-13*2, msgBuf,     Font8x13, 1);
	OLED_Display();
}
void start_P() {
	OLED_Show_Picture(13, 25, 102, 5, Bar_Icon);

	OLED_Init();
	OLED_Show_Str(52, 33, "00%", Font8x13, 0);
	OLED_Display();
	HAL_Delay(100);

	PAJ7620_Init();
	OLED_Show_Str(52, 33, "20%", Font8x13, 0);
	OLED_Fill(13, 26, 33, 28, 1);
	OLED_Display();
	HAL_Delay(100);

	Usart1_Init();
	OLED_Show_Str(52, 33, "40%", Font8x13, 0);
	OLED_Fill(13, 26, 53, 28, 1);
	OLED_Display();
	HAL_Delay(100);

	DFR0299Init();
	OLED_Show_Str(52, 33, "70%", Font8x13, 0);
	OLED_Fill(13, 26, 83, 28, 1);
	OLED_Display();
	HAL_Delay(100);

	Battery_ADC_Init();
	OLED_Show_Str(52, 33, "80%", Font8x13, 0);
	OLED_Fill(13, 26, 93, 28, 1);
	OLED_Display();
	HAL_Delay(100);

	OLED_Show_Str(48, 33, "100%", Font8x13, 0);
	OLED_Fill(13, 26, 113, 28, 1);
	OLED_Display();
	HAL_Delay(100);
}












