#include "stdbool.h"
#include "User_Tasks.h"

char str_buf[20];
uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0,
		Sleep_Sec = 0;

uint32_t sw_tick = 0, idle_tick = 0;
uint8_t sw_flag = 0, sw_buf = 0, sw_it = 0;
uint8_t sw_alarm_flag = 0;

void btn1CbShort();
void btn1CbLong();

void btn2CbShort();
void btn2CbLong();

void btn3CbShort();
void btn3CbLong();

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
	uint32_t last;
	bool b_a_last;
	callBack cbShort;
	callBack cbLong;
} btnProcess_t;

void updateRE(btnProcess_t *btn);
void initBtn(btnProcess_t *btn, GPIO_TypeDef *port, uint16_t pin,
		callBack cbShort, callBack cbLong);
void initApp(){
	PAJ7620_Init();
	OLED_Init();
	//OLED_Show_Str(0, 0, "begin!!", Font8x13, 0);
	// 128 * 64
	OLED_Show_Picture(10, 32, 102, 5, Bar_Icon);
	OLED_Display();
}

void loop(void) {
	initApp();
	btnProcess_t btn1, btn2, btn3;
	initBtn(&btn1, SW1_GPIO_Port, SW1_Pin, btn1CbShort, btn1CbLong);
	initBtn(&btn2, SW2_GPIO_Port, SW2_Pin, btn2CbShort, btn2CbLong);
	initBtn(&btn3, SW3_GPIO_Port, SW3_Pin, btn3CbShort, btn3CbLong);
	while (1) {
		updateRE(&btn1);
		updateRE(&btn2);
		updateRE(&btn3);
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
	DHT11_Type dht11;
	DHT11_Read_Data(&dht11);
	char msgBuff[32] = { 0, };

	sprintf(msgBuff, "%d.%dC", dht11.Temp_Int, dht11.Temp_Float);

	OLED_Show_Str(0, 0, msgBuff, Font8x13, 0);
	OLED_Display();
}
void btn1CbLong() {
	OLED_Show_Str(0, 0, "btn1 long ", Font8x13, 0);
	OLED_Display();
}

void btn2CbShort() {
	BUZZ_ON;
	OLED_Show_Str(1, 0, "btn2 short ", Font8x13, 0);
	OLED_Display();
}
void btn2CbLong() {
	BUZZ_OFF;
	OLED_Show_Str(1, 0, "btn2 long ", Font8x13, 0);
	OLED_Display();
}

void btn3CbShort() {
	OLED_Show_Str(2, 0, "btn3 short ", Font8x13, 0);
	OLED_Display();
}
void btn3CbLong() {
	OLED_Show_Str(2, 0, "btn3 long ", Font8x13, 0);
	OLED_Display();
}
