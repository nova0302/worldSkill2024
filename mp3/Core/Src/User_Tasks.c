#include "stdbool.h"
#include "User_Tasks.h"


char str_buf[20];
uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0,
		Sleep_Sec = 0;

uint32_t sw_tick = 0, idle_tick = 0;
uint8_t sw_flag = 0, sw_buf = 0, sw_it = 0;
uint8_t sw_alarm_flag = 0;

void sw1CbShort();
void sw1CbLong();

void sw2CbShort();
void sw2CbLong();

void sw3CbShort();
void sw3CbLong();

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
typedef enum btnState {
	ST_IDLE, ST_DOWN, ST_SHORT_PRESSED, ST_LONG_PRESSED
} StBtnState_t;

typedef void (*callBack)();

typedef struct btnProcess{
	GPIO_TypeDef *port;
	uint16_t pin;
	StBtnState_t btnState ;
	uint32_t last;
	bool b_a_last;
	callBack cbShort;
	callBack cbLong;
}btnProcess_t;

void updateRE(btnProcess_t *btn) ;
void initBtn(btnProcess_t *btn, GPIO_TypeDef *port, uint16_t pin, callBack cbShort, callBack cbLong);

void loop(void) {
	OLED_Init();
	OLED_Show_Str(0,0, "begin!!", Font8x13, 0);
	OLED_Display();
	btnProcess_t sw1, sw2, sw3;
	initBtn(&sw1, SW1_GPIO_Port, SW1_Pin, sw1CbShort, sw1CbLong);
	initBtn(&sw2, SW2_GPIO_Port, SW2_Pin, sw2CbShort, sw2CbLong);
	initBtn(&sw3, SW3_GPIO_Port, SW3_Pin, sw3CbShort, sw3CbLong);
	while (1) {
		updateRE(&sw1); updateRE(&sw2); updateRE(&sw3);
	}
}

void initBtn(btnProcess_t *btn, GPIO_TypeDef *port, uint16_t pin, callBack cbShort, callBack cbLong){
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
	case ST_IDLE:
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
		if (b_a_now)
			btn->btnState = ST_SHORT_PRESSED;
		else if (now - btn->last > 3000)
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

void sw1CbShort(){
	OLED_Show_Str(0,0, "sw1 short ", Font8x13, 0);
	OLED_Display();
}
void sw1CbLong(){
	OLED_Show_Str(0,0, "sw1 long ", Font8x13, 0);
	OLED_Display();
}

void sw2CbShort(){
	OLED_Show_Str(1,0, "sw2 short ", Font8x13, 0);
	OLED_Display();
}
void sw2CbLong(){
	OLED_Show_Str(1,0, "sw2 long ", Font8x13, 0);
	OLED_Display();
}

void sw3CbShort(){
	OLED_Show_Str(2,0, "sw3 short ", Font8x13, 0);
	OLED_Display();
}
void sw3CbLong(){
	OLED_Show_Str(2,0, "sw3 long ", Font8x13, 0);
	OLED_Display();
}
