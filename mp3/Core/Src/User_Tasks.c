#include <stdio.h>
#include "User_Tasks.h"

uint32_t Tr_all = 0;
uint32_t Tr_now = 0;

char str_buf[20];
uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0,
		Sleep_Sec = 0;

uint32_t sw_tick = 0, idle_tick = 0;
uint8_t sw_flag = 0, sw_buf = 0, sw_it = 0;
uint8_t sw_alarm_flag = 0;

bool g_bUpdateScreen = false;
uint8_t g_CurLoc = 0; // 첫번제 칸
uint16_t g_trackNo;
uint32_t g_numTotalTrack;
SystemState_t g_SystemState;

void Volume_Up() {
	if (Volume < 30)
		Volume += 1;
	DFSetVolume(Volume);
	eeprom_8bit_write(0, Volume);
}

void Volume_Down() {
	if (Volume)
		Volume--;
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
}
void loop(void) {

	initApp();

	btnProcess_t btn[numBtn];

	initBtn(&btn[0], SW1_GPIO_Port, SW1_Pin, btn1CbShort, btn1CbLong);
	initBtn(&btn[1], SW2_GPIO_Port, SW2_Pin, btn2CbShort, btn2CbLong);
	initBtn(&btn[2], SW3_GPIO_Port, SW3_Pin, btn3CbShort, btn3CbLong);
	initBtn(&btn[3], SW4_GPIO_Port, SW4_Pin, btn4CbShort, btn4CbLong);


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



	// 윤년 && eeprom?

	while (1) {

		for (int i = 0; i < numBtn; ++i)
			updateBtn(&btn[i]);

		if (g_bUpdateScreen) {
			switch (g_SystemState) {
			case ST_SYS_MAIN:
				updatMainMenu();
				break;
			case ST_SYS_MANAGEMENT:
				updatManagementMenu();
				break;
			default:
				break;
			}
			//
			g_bUpdateScreen = false;
		}
	}
}

void updatManagementMenu() {
	switch (g_CurLoc) {
	case 0: // 1번째줄 반전
		break;
	case 1: // 1번째줄 반전
		break;
	default:
		break;
	}
}


void updatMainMenu() {

	switch (g_CurLoc) {
	case 0: // 1번째줄 반전
		break;
	case 1: // 1번째줄 반전
		break;
	default:
		break;
	}
}



void initBtn(btnProcess_t *pBtn, GPIO_TypeDef *port, uint16_t pin,
		callBack cbShort, callBack cbLong) {
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

		uint32_t delayAmount = 2000;
		if (g_SystemState == ST_SYS_INIT)
			delayAmount = 2000;
		else if (g_SystemState == ST_SYS_MAIN)
			delayAmount = 5000;

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

void btn1CbShort() {
#ifdef DEBUG_
	printDbgMessage(1, true);
#endif
	switch (g_SystemState) {
	case ST_SYS_MAIN:
		DFPlayNextTrack();
		break; // 다음 곡을 재행하는 코드 구현
	case ST_SYS_MANAGEMENT:
		if (--g_CurLoc < 0)
			g_CurLoc = 3;
		g_bUpdateScreen = true;
		break; //
	default:
		break;
	}
}
void btn1CbLong() {
#ifdef DEBUG_
	printDbgMessage(1, false);
#endif
	switch (g_SystemState) {
	case ST_SYS_MAIN:
		DFPlayThisTrack(g_trackNo);
		break; // 현재곡 재행
	case ST_SYS_MANAGEMENT:
		break; //
	default:
		break;
	}
}
void btn2CbShort() {
#ifdef DEBUG_
	printDbgMessage(2, true);
#endif
	switch (g_SystemState) {
	case ST_SYS_MAIN:
		DFPlayPreviousTrack();
		break; // 이전곡 재행
	case ST_SYS_MANAGEMENT:
		if (++g_CurLoc > 3)
			g_CurLoc = 0;
		g_bUpdateScreen = true;
		break; //
	default:
		break;
	}
}
void btn2CbLong() {
#ifdef DEBUG_
	printDbgMessage(2, false);
#endif
	switch (g_SystemState) {
	case ST_SYS_MAIN:
		DFPause();
		break; // 일시중지하는 코드 구현
	case ST_SYS_MANAGEMENT:

		break; //
	default:
		break;
	}
}
void btn3CbShort() {
#ifdef DEBUG_
	printDbgMessage(3, true);
#endif
	switch (g_SystemState) {
	case ST_SYS_MAIN:
		Volume_Up();
		break; // 볼륨증가 코드
	case ST_SYS_MANAGEMENT:
		break; // 현재 곡을 재행하는 코드 구현
	default:
		break;
	}
}
void btn3CbLong() {
#ifdef DEBUG_
	printDbgMessage(3, false);
#endif
	switch (g_SystemState) { // 절전기능 실행 코드 구현
	case ST_SYS_MAIN:
		g_SystemState = ST_SYS_POW_SAVE;
		break; //  설정 화면으로 전환
	case ST_SYS_MANAGEMENT:
		break; //
	default:
		break;
	}
}
void btn4CbShort() {
#ifdef DEBUG_
	printDbgMessage(4, true);
#endif
	switch (g_SystemState) {
	case ST_SYS_MAIN:
		Volume_Down();
		break; // 볼륨감소 코드
	case ST_SYS_MANAGEMENT:
		break; //
	default:
		break;
	}
}

void btn4CbLong() {
#ifdef DEBUG_
	printDbgMessage(4, false);
#endif
	switch (g_SystemState) {
	case ST_SYS_MAIN:
		g_SystemState = ST_SYS_MANAGEMENT;
		g_bUpdateScreen = true;
		break; //  설정 화면으로 전환
	case ST_SYS_MANAGEMENT:
		break; //
	default:
		break;
	}
}

void printDbgMessage(uint8_t btnNumber, bool bIsShortPress) {
	char msgBuf[64];
	sprintf(msgBuf, "btn%d %s pressed!!", btnNumber,
			bIsShortPress ? "short" : "long");
	OLED_Show_Str(0, 0, msgBuf, Font8x13, 0);
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
