#include "stdbool.h"
#include "User_Tasks.h"

char str_buf[20];
uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0,
		Sleep_Sec = 0;

uint32_t sw_tick = 0, idle_tick = 0;
uint8_t sw_flag = 0, sw_buf = 0, sw_it = 0;
uint8_t sw_alarm_flag = 0;

uint16_t g_trackNo;

void btn1CbShort();
void btn1CbLong();

void btn2CbShort();
void btn2CbLong();

void btn3CbShort();
void btn3CbLong();

void btn4CbShort();
void btn4CbLong();

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

typedef enum screenState {
	ST_SCR_IDLE, ST_SCR_1, ST_SCR_2, ST_SCR_3, ST_SCR_4, ST_SCR_5, ST_SCR_6
} ScreenState_t;

ScreenState_t gScreenState;

typedef enum btnState {
	ST_IDLE, ST_DOWN, ST_SHORT_PRESSED, ST_LONG_PRESSED
} StBtnState_t;

typedef void (*callBack)();

typedef struct btnProcess {
	GPIO_TypeDef *port;
	uint16_t pin;
	StBtnState_t btnState;
	uint32_t last; // time이전값
	bool b_a_last; // btn이전값
	callBack cbShort;
	callBack cbLong;
} btnProcess_t;

void updateRE(btnProcess_t *btn);
void initBtn(btnProcess_t *btn, GPIO_TypeDef *port, uint16_t pin,
		callBack cbShort, callBack cbLong);
void initApp() {
	PAJ7620_Init();
	OLED_Init();
	//OLED_Show_Str(0, 0, "begin!!", Font8x13, 0);
	// 128 * 64
	OLED_Show_Picture(10, 32, 102, 5, Bar_Icon);
	OLED_Display();
}

void loop(void) {

	initApp();

	btnProcess_t btn[4];

	initBtn(&btn[0], SW1_GPIO_Port, SW1_Pin, btn1CbShort, btn1CbLong);
	initBtn(&btn[1], SW2_GPIO_Port, SW2_Pin, btn2CbShort, btn2CbLong);
	initBtn(&btn[2], SW3_GPIO_Port, SW3_Pin, btn3CbShort, btn3CbLong);
	initBtn(&btn[3], SW4_GPIO_Port, SW4_Pin, btn4CbShort, btn4CbLong);

	while (1) {
		for (int i = 0; i < 3; ++i)
			updateRE(&btn[i]);
	}
}

void initBtn(btnProcess_t *btn, GPIO_TypeDef *port, uint16_t pin,
		callBack cbShort, callBack cbLong) {
	btn->port = port;
	btn->pin = pin;
	btn->btnState = ST_IDLE;
	btn->last = 0;
	btn->cbShort = cbShort;
	btn->cbLong = cbLong;
}

void updateRE(btnProcess_t *btn) {

	uint32_t now = HAL_GetTick();

	switch (btn->btnState) {

	case ST_IDLE: // falling edge check
		if (now - btn->last > 10) {
			btn->last = now;
			bool b_a_now = HAL_GPIO_ReadPin(btn->port, btn->pin);
			if (btn->b_a_last == 1 && b_a_now == 0)
				btn->btnState = ST_DOWN;
			btn->b_a_last = b_a_now;
		}
		break;
	case ST_DOWN:

		bool b_a_now = HAL_GPIO_ReadPin(btn->port, btn->pin);

		uint32_t delayAmount = 1000;
		if (gScreenState == ST_SCR_1) {
			delayAmount = 1000;
		} else if (gScreenState == ST_SCR_2) {
			delayAmount = 3000;
		}

		if (b_a_now)
			btn->btnState = ST_SHORT_PRESSED;
		else if (now - btn->last > delayAmount)
			btn->btnState = ST_LONG_PRESSED;
		break;
	case ST_SHORT_PRESSED:
		btn->cbShort();
		btn->btnState = ST_IDLE;
		break;
	case ST_LONG_PRESSED:
		btn->cbLong();
		btn->btnState = ST_IDLE;
		break;
	}
}

void btn1CbShort() {
	DFPlayNextTrack(); // 다음 곡을 재행하는 코드 구현
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
	//  설정화면으로 전환
}
