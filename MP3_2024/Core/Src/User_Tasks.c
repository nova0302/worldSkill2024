#include "User_Tasks.h"

char str_buf[20];
uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0,
		Sleep_Sec = 0;
uint32_t sw_tick = 0, idle_tick = 0; // 스위치 틱, 아이들 틱 변수 선언
uint8_t sw_flag = 0, sw_buf = 0, sw_it = 0; // 스위치 플래그, 버퍼, 인터럽트 변수 선언
uint8_t sw_alarm_flag = 0; // 스위치 알람 플래그 변수 선언
uint8_t tra_all = 3; // 전체 트랙값
uint8_t tra_now = 0; // 현재 트랙값
uint16_t modeF = 1;
uint8_t YMD = 0;
uint8_t gestur = 0;
uint8_t Opsw_C = 3;
uint8_t DATE_Curser = 1;
uint8_t Alarm_Curser = 1;
uint16_t day_Max = 0;

void EEPROM_Load() {
	for (uint8_t i = 0; i < 5; i++) {
		if (i == 0)
			Volume = eeprom_8bit_read(i);
		if (i == 1)
			Alarm_Flag = eeprom_8bit_read(i);
		if (i == 2)
			Sleep_Flag = eeprom_8bit_read(i);
		if (i == 3)
			Sleep_Min = eeprom_8bit_read(i);
		if (i == 4)
			Sleep_Sec = eeprom_8bit_read(i);
	}

	if ((Sleep_Min * 60 + Sleep_Sec) < 10)
		Sleep_Sec = 10;
	DFSetVolume(Volume);
	if (Alarm_Flag)
		DS3231_Enable_Alarm1();
	else
		DS3231_Disable_Alarm1();
}

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

void Loading() {
	//OLED initialization complete
	OLED_Init();
	OLED_Show_Picture(13, 25, 102, 5, Bar_Icon);
	OLED_Show_Str(52, 33, "00%", Font8x13, 0);
	OLED_Display();
	HAL_Delay(100);
	//Gesture sensor initialization complete
	PAJ7620_Init();
	OLED_Show_Str(52, 33, "20%", Font8x13, 0);
	OLED_Fill(13, 26, 33, 28, 1);
	OLED_Display();
	HAL_Delay(100);
	//USART initialization complete
	Usart1_Init();
	OLED_Show_Str(52, 33, "40%", Font8x13, 0);
	OLED_Fill(13, 26, 53, 28, 1);
	OLED_Display();
	HAL_Delay(100);
	//MP3 player initialization complete
	DFR0299Init();
	OLED_Show_Str(52, 33, "70%", Font8x13, 0);
	OLED_Fill(13, 26, 83, 28, 1);
	OLED_Display();
	HAL_Delay(100);
	//Battery ADC initialization complete
	Battery_ADC_Init();
	OLED_Show_Str(52, 33, "80%", Font8x13, 0);
	OLED_Fill(13, 26, 93, 28, 1);
	OLED_Display();
	HAL_Delay(100);
	//EEPROM data loading complete
	EEPROM_Load();
	OLED_Show_Str(48, 33, "100%", Font8x13, 0);
	OLED_Fill(13, 26, 113, 28, 1);
	OLED_Display();
	HAL_Delay(100);
}
void SW_Handler(uint8_t longSwEn) { // 스위치 핸들러 함수
	if ((SW1_GPIO_Port->IDR & 0x1B) != 0x1B) { // SW1_GPIO_Port의 IDR 값이 0x1B가 아니면
		if (!sw_flag) { // 스위치 플래그가 거짓이면
			if (!sw_buf && !sw_alarm_flag) { // 스위치 버퍼와 스위치 알람 플래그가 거짓이면
				if ((SW1_GPIO_Port->IDR & 0x01) == 0)
					sw_buf = 1; // SW1_GPIO_Port의 IDR 값이 0이면 스위치 버퍼를 1로 설정
				if ((SW1_GPIO_Port->IDR & 0x02) == 0)
					sw_buf = 2; // SW1_GPIO_Port의 IDR 값이 0이면 스위치 버퍼를 2로 설정
				if ((SW1_GPIO_Port->IDR & 0x08) == 0)
					sw_buf = 3; // SW1_GPIO_Port의 IDR 값이 0이면 스위치 버퍼를 3로 설정
				if ((SW1_GPIO_Port->IDR & 0x10) == 0)
					sw_buf = 4; // SW1_GPIO_Port의 IDR 값이 0이면 스위치 버퍼를 4로 설정
				sw_tick = HAL_GetTick(); // 스위치 틱을 현재 시간으로 설정
				sw_flag = 1; // 스위치 플래그를 참으로 설정
			}
		} else if (HAL_GetTick() - sw_tick >= 1000 && longSwEn) { // 스위치 틱이 1000ms 이상이고 longSwEn이 참이면
			sw_it = sw_buf + 4; // 스위치를 스위치 버퍼에 4를 더한 값으로 설정
			sw_flag = 0; // 스위치 플래그를 거짓으로 설정
			Buzz_Set(1, 5, 0); // 부저 설정
		}
	} else {
		if (sw_flag) { // 스위치 플래그가 참이면
			sw_it = sw_buf; // 스위치를 스위치 버퍼로 설정
			sw_flag = 0; // 스위치 플래그를 거짓으로 설정
			Buzz_Set(1, 5, 0); // 부저 설정
		}
		if (!sw_it)
			sw_buf = 0; // 스위치가 거짓이면 스위치 버퍼를 0으로 설정
		if (!Buzz_Cnt)
			sw_alarm_flag = 0; // 부저 카운트가 거짓이면 스위치 알람 플래그를 거짓으로 설정
	}
}
void main_P() {
	SW_Handler(1);
	tra_all = DFGetTotalTrack();
	tra_now = DFGetCurrentTrack();
	DHT11_Type DHT11;
	DS3231_Time_Type YMD;
	YMD = DS3231_Get_Time();
	if (YMD.Month == 2) {
		if (YMD.Year == 0 || YMD.Year % 4 == 0) {
			if (YMD.Day >= 30) {
				YMD.Day = 29;
			}
		} else {
			if (YMD.Day >= 29) {
				YMD.Day = 28;
			}
		}
	}
	if (YMD.Day == 31) {
		switch (YMD.Month) {
		case 1:
			YMD.Day = 31;
			break;
		case 3:
			YMD.Day = 31;
			break;
		case 4:
			YMD.Day = 30;
			break;
		case 5:
			YMD.Day = 31;
			break;
		case 6:
			YMD.Day = 30;
			break;
		case 7:
			YMD.Day = 31;
			break;
		case 8:
			YMD.Day = 31;
			break;
		case 9:
			YMD.Day = 30;
			break;
		case 10:
			YMD.Day = 31;
			break;
		case 11:
			YMD.Day = 30;
			break;
		case 12:
			YMD.Day = 31;
			break;
		default:
			break;
		}
	}
	OLED_Show_Picture(120, 0, 8, 16, Battry_Icon);
	OLED_Show_Picture(0, 0, 16, 16, Dir_Icon);

	sprintf(str_buf, "VOL:%02d", Volume);
	OLED_Show_Str((128 - 8 * 6) / 2, 0, str_buf, Font8x13, 0);

	sprintf(str_buf, "Track:%02d/%02d", tra_now, tra_all);
	OLED_Show_Str((128 - 8 * 11) / 2, 18, str_buf, Font8x13, 0);

	sprintf(str_buf, "20%02d-%02d-%02d", YMD.Year, YMD.Month, YMD.Day);
	OLED_Show_Str(0, 34, str_buf, Font8x13, 0);

	sprintf(str_buf, "%02d-%02d-%02d", YMD.Hour, YMD.Minute, YMD.Second);
	OLED_Show_Str(0, 50, str_buf, Font8x13, 0);

	DHT11_Read_Data(&DHT11);
	sprintf(str_buf, "T:%02dC", DHT11.Temp_Int);
	OLED_Show_Str(128 - 8 * 5, 34, str_buf, Font8x13, 0);

	sprintf(str_buf, "H:%02d%%", DHT11.Humi_Int);
	OLED_Show_Str(128 - 8 * 5, 50, str_buf, Font8x13, 0);

	OLED_Display();
	if (sw_it) {
		if (sw_it == 1) {
			DFPlayNextTrack();
		}
		if (sw_it == 2) {
			DFPlayPreviousTrack();
		}
		if (sw_it == 3) {
			Volume_Up();
		}
		if (sw_it == 4) {
			Volume_Down();
		}
		if (sw_it == 5) {
			DFPlayThisTrack(tra_now);
		}
		if (sw_it == 6) {
			DFPause();
		}
		if (sw_it == 7) {
			modeF = 1;
		}
		if (sw_it == 8) {
			modeF = 2;

		}
		sw_it = 0;
	}
	gestur = PAJ7620_Get_Dir();
	if (gestur) {
		if (gestur == Down)
			Volume_Up();
		if (gestur == Up)
			Volume_Down();
		if (gestur == Left)
			DFPlayNextTrack();
		if (gestur == Down)
			DFPlayPreviousTrack();
		gestur = 0;
	}

}
void option_P() {
	SW_Handler(0);
	if (sw_it) {
		if (sw_it == 1)
			Opsw_C--;
		if (sw_it == 2)
			Opsw_C++;
		if (sw_it == 3)
			modeF = Opsw_C;
		if (sw_it == 4)
			modeF = 1;
		sw_it = 0;
	}
	if (Opsw_C == 6)
		Opsw_C = 3;
	if (Opsw_C == 2)
		Opsw_C = 5;
	OLED_Show_Str(40, 0, "OPTIONS", Font8x13, 0);
	OLED_Show_Str(5, 64 - 13 * 3, "Date/Time Set", Font8x13, (Opsw_C == 3));
	OLED_Show_Str(5, 64 - 13 * 2, "Alarm Set", Font8x13, (Opsw_C == 4));
	OLED_Show_Str(5, 64 - 13 * 1, "Sleep Mode Set", Font8x13, (Opsw_C == 5));
	OLED_Display();

}
void Alarm_Handler() {
}
void DateTime_P() {
	DS3231_Time_Type YMD;
	YMD = DS3231_Get_Time();
	SW_Handler(0);
	if (sw_it) {
		if (sw_it == 1) {
			if (DATE_Curser == 1) {
				YMD.Year++;
				if (YMD.Year == 100)
					YMD.Year = 0;
			}
			if (DATE_Curser == 2) {
				YMD.Month++;
				if (YMD.Month == 13)
					YMD.Month = 1;
			}
			if (DATE_Curser == 3) {
				YMD.Day++;
				if (YMD.Day == 32)
					YMD.Day = 1;
			}
			if (DATE_Curser == 4) {
				YMD.Hour++;
				if (YMD.Hour == 24)
					YMD.Hour = 0;
			}
			if (DATE_Curser == 5) {
				YMD.Minute++;
				if (YMD.Minute == 60)
					YMD.Minute = 0;
			}
			if (DATE_Curser == 6) {
				YMD.Second++;
				if (YMD.Second == 60)
					YMD.Second = 0;
			}
		}
		if (sw_it == 2) {
			if (DATE_Curser == 1) {
				YMD.Year--;
				//if(YMD.Year == -1)YMD.Year = 99;
			}
			if (DATE_Curser == 2) {
				YMD.Month--;
				//if(YMD.Month == 0)YMD.Month = 12;
			}
			if (DATE_Curser == 3) {
				YMD.Day--;
				//if(YMD.Day == 0)YMD.Day = day_S;
			}
			if (DATE_Curser == 4) {
				YMD.Hour--;
				//if(YMD.Hour == -1)YMD.Hour = 23;
			}
			if (DATE_Curser == 5) {
				YMD.Minute--;
				//	if(YMD.Minute == -1)YMD.Minute = 59;
			}
			if (DATE_Curser == 6) {
				YMD.Second--;
				if (YMD.Second == -1)
					YMD.Second = 59;
			}
		}
		if (sw_it == 3) {
			DATE_Curser++;
			if (DATE_Curser == 7)
				DATE_Curser = 1;
		}
		if (sw_it == 4) {
			sw_it = 0;
			modeF = 2;
		}
		DS3231_Set_Time(YMD);
		sw_it = 0;
	}

	OLED_Show_Str((128 - 8 * 13) / 2, 0, "DATE/TIME SET", Font8x13, 0);

	sprintf(str_buf, "20%02d", YMD.Year);
	OLED_Show_Str(8 * 3, 16 * 1 + 8, str_buf, Font8x13, (DATE_Curser == 1));

	OLED_Show_Str(8 * 7, 16 * 1 + 8, "-", Font8x13, 0);

	sprintf(str_buf, "%02d", YMD.Month);
	OLED_Show_Str(8 * 8, 16 * 1 + 8, str_buf, Font8x13, (DATE_Curser == 2));

	OLED_Show_Str(8 * 10, 16 * 1 + 8, "-", Font8x13, 0);

	sprintf(str_buf, "%02d", YMD.Day);
	OLED_Show_Str(8 * 11, 16 * 1 + 8, str_buf, Font8x13, (DATE_Curser == 3));

	sprintf(str_buf, "%02d", YMD.Hour);
	OLED_Show_Str(8 * 4, 16 * 2 + 8, str_buf, Font8x13, (DATE_Curser == 4));

	OLED_Show_Str(8 * 6, 16 * 2 + 8, ":", Font8x13, 0);

	sprintf(str_buf, "%02d", YMD.Minute);
	OLED_Show_Str(8 * 7, 16 * 2 + 8, str_buf, Font8x13, (DATE_Curser == 5));

	OLED_Show_Str(8 * 9, 16 * 2 + 8, ":", Font8x13, 0);

	sprintf(str_buf, "%02d", YMD.Second);
	OLED_Show_Str(8 * 10, 16 * 2 + 8, str_buf, Font8x13, (DATE_Curser == 6));

	OLED_Display();

}
void Alarm_P(){
	SW_Handler(0);
	if(sw_it){
		if(sw_it == 1){

		}
		if(sw_it == 2){

		}
		if(sw_it == 3){
			Alarm_Curser++;
			if(Alarm_Curser == 4){
				Alarm_Curser = 1;
			}
		}

	}
	OLED_Show_Str((128-8*9)/2, 0, "ALARM SET", Font8x13, 0);
	OLED_Show_Str(0, 0+2*13, "Enable:", Font8x13, 0);
	OLED_Display();
}
void Sleep_P() {

}
void loop(void) {
	Loading();
	OLED_Buffer_Clear();
	while (1) {
		OLED_Buffer_Clear();

		if (modeF == 1)
			main_P();
		if (modeF == 2)
			option_P();
		if (modeF == 3)
			DateTime_P();
		if (modeF == 4)
			Alarm_P();
		if (modeF == 5)
			Sleep_P();

	}
}
