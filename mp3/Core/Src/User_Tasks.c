#include "User_Tasks.h"

char str_buf[20];
uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0,
		Sleep_Sec = 0;

uint32_t sw_tick = 0, idle_tick = 0;
uint8_t sw_flag = 0, sw_buf = 0, sw_it = 0;
uint8_t sw_alarm_flag = 0;

uint16_t g_trackNo;
uint8_t g_numTotalTrack;
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
	OLED_Init();
	PAJ7620_Init();
	Usart1_Init();
	DFR0299Init();
	Battery_ADC_Init();
	g_numTotalTrack = DFGetTotalTrack();

	// EepromLoad();

	OLED_Show_Str(0, 0, "begin!!", Font8x13, 0);
	// 128 * 64
	OLED_Show_Picture(10, 32, 102, 5, Bar_Icon);
	OLED_Display();
}
void loop(void) {

	initApp();

	btnProcess_t btn[numBtn];

	initBtn(&btn[0], SW1_GPIO_Port, SW1_Pin, btn1CbShort, btn1CbLong);
	initBtn(&btn[1], SW2_GPIO_Port, SW2_Pin, btn2CbShort, btn2CbLong);
	initBtn(&btn[2], SW3_GPIO_Port, SW3_Pin, btn3CbShort, btn3CbLong);
	initBtn(&btn[3], SW4_GPIO_Port, SW4_Pin, btn4CbShort, btn4CbLong);

	while (1) {
		for (int i = 0; i < numBtn; ++i)
			updateBtn(&btn[i]);
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
	case ST_SHORT_PRESSED: pBtn->cbShort(); pBtn->btnState = ST_IDLE; break;
	case ST_LONG_PRESSED:  pBtn->cbLong();  pBtn->btnState = ST_IDLE; break;
	}
}

void btn1CbShort() {
	if (g_SystemState == ST_SYS_MAIN) {
		DFPlayNextTrack(); // 다음 곡을 재행하는 코드 구현
	}
}
void btn1CbLong() {
	DFPlayThisTrack(g_trackNo); // 현재 곡을 재행하는 코드 구현
}
void btn2CbShort() {
	DFPlayPreviousTrack(); // 다음 이전곡을 재행하는 코드 구현
}
void btn2CbLong() {
	DFPause(); // 일시중지하는 코드 구현
}
void btn3CbShort() {
	Volume_Up(); // 볼륨증가 코드
}
void btn3CbLong() {
	// 절전기능 실행 코드 구현
}
void btn4CbShort() {
	Volume_Down(); // 볼륨감소 코드
}
void btn4CbLong() {
	//  설정 화면으로 전환
	g_SystemState = ST_SYS_MANAGEMENT;
}
