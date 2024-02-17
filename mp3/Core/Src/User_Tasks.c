#include "stdbool.h"
#include "User_Tasks.h"


char str_buf[20];
uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0,
		Sleep_Sec = 0;

uint32_t sw_tick = 0, idle_tick = 0;
uint8_t sw_flag = 0, sw_buf = 0, sw_it = 0;
uint8_t sw_alarm_flag = 0;

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

typedef struct btnProcess{
	GPIO_TypeDef *port;
	uint16_t pin;
	StBtnState_t btnState ;
	uint32_t last;
	bool b_a_last;
}btnProcess_t;

void updateRE(btnProcess_t *btn) ;

void loop(void) {
	btnProcess_t sw1, sw2, sw3;
	while (1) {
		updateRE(&sw1); updateRE(&sw2); updateRE(&sw3);
	}
}

void doWorkForShortPress(){ }

void doWorkForLongPress(){ }

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
		doWorkForShortPress();
		btn->btnState = ST_IDLE;
		break;
	case ST_LONG_PRESSED:
		doWorkForLongPress();
		btn->btnState = ST_IDLE;
		break;
	}
}
