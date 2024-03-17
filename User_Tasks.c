#include "User_Tasks.h"

char str_buf[20];
uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0, Sleep_Sec = 0;

uint32_t sw_tick = 0, idle_tick = 0; // 스위치 틱, 아이들 틱 변수 선언
uint8_t sw_flag = 0, sw_buf = 0, sw_it = 0; // 스위치 플래그, 버퍼, 인터럽트 변수 선언
uint8_t sw_alarm_flag = 0; // 스위치 알람 플래그 변수 선언

uint32_t sleep_tick = 0;

uint8_t modeF = 0;

uint8_t track_N, track_A = 0;
uint8_t now_P = 0;
uint8_t PH_flag = 0;

uint8_t A_cursor = 0;

uint8_t Scursor = 0;
uint8_t S_cursor = 0;
uint8_t cursor = 10;
uint16_t gestur = 0;
uint16_t M_day = 0;

uint16_t charge_tick = 0;
uint8_t charge_flag = 0;

uint8_t PHF = 0;
uint8_t fF = 1;
uint8_t level = 0;
uint32_t adc = 0;

int A_hour = 0;
int A_min = 0;
int S_min = 0;
int S_sec = 0;

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
	OLED_Init(); // OLED 초기화
	OLED_Show_Str((128 - 4 * 8) / 2, 0, "DEMO", Font8x13, 0); // "DEMO" 문자열을 OLED에 출력
	OLED_Show_Picture((128 - 102) / 2, 22, 102, 5, Bar_Icon); // 바 아이콘을 OLED에 출력
	OLED_Show_Str((128 - 3 * 8) / 2, 29, "00%", Font8x13, 0); // "00%" 문자열을 OLED에 출력
	OLED_Display(); // OLED 출력
	HAL_Delay(100); // 100ms 지연

	PAJ7620_Init(); // PAJ7620 초기화
	OLED_Show_Str((128 - 3 * 8) / 2, 29, "2", Font8x13, 0); // "2" 문자열을 OLED에 출력
	OLED_Fill(14, 23, 14 + 19, 23 + 2, 1); // OLED에 사각형 출력
	OLED_Display(); // OLED 출력
	HAL_Delay(100); // 100ms 지연

	Usart1_Init(); // USART1 초기화
	OLED_Show_Str((128 - 3 * 8) / 2, 29, "4", Font8x13, 0); // "4" 문자열을 OLED에 출력
	OLED_Fill(14 + 20, 23, 14 + 39, 23 + 2, 1); // OLED에 사각형 출력
	OLED_Display(); // OLED 출력
	HAL_Delay(100); // 100ms 지연

	DFR0299Init(); // DFR0299 초기화
	OLED_Show_Str((128 - 3 * 8) / 2, 29, "7", Font8x13, 0); // "7" 문자열을 OLED에 출력
	OLED_Fill(14 + 40, 23, 14 + 69, 23 + 2, 1); // OLED에 사각형 출력
	OLED_Display(); // OLED 출력
	HAL_Delay(100); // 100ms 지연

	Battery_ADC_Init(); // 배터리 ADC 초기화
	OLED_Show_Str((128 - 3 * 8) / 2, 29, "8", Font8x13, 0); // "8" 문자열을 OLED에 출력
	OLED_Fill(14 + 70, 23, 14 + 79, 23 + 2, 1); // OLED에 사각형 출력
	OLED_Display(); // OLED 출력
	HAL_Delay(100); // 100ms 지연

	EEPROM_Load(); // EEPROM 로드 함수 호출
	OLED_Show_Str((128 - 4 * 8) / 2, 29, "100%", Font8x13, 0); // "100%" 문자열을 OLED에 출력
	OLED_Fill(14 + 80, 23, 14 + 99, 23 + 2, 1); // OLED에 사각형 출력
	OLED_Display(); // OLED 출력
	HAL_Delay(100); // 100ms 지연
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

void Alarm_Handler() {
	if (!HAL_GPIO_ReadPin(DS3231_INT_GPIO_Port, DS3231_INT_Pin)) { // DS3231_INT_GPIO_Port의 IDR 값이 0이면
		sw_alarm_flag = 1; // 스위치 알람 플래그를 참으로 설정
		Clean_Alarm1_Flag(); // 알람1 플래그 클리어
		Buzz_Set(60, 30, 60); // 부저 설정
	}
	if (sw_alarm_flag && ((SW1_GPIO_Port->IDR & 0x1B) != 0x1B))
		Buzz_Set(0, 0, 0); // 스위치 알람 플래그가 참이고 SW1_GPIO_Port의 IDR 값이 0x1B가 아니면 부저 설정
}

void main_M() {
	now_P = 0;
	SW_Handler(1);
	Alarm_Handler();
	//sleep_handler();
	track_A = DFGetTotalTrack();
	track_N = DFGetCurrentTrack();
	DS3231_Time_Type YMD;
	DHT11_Type DHT11;
	YMD = DS3231_Get_Time();


	adc = 4095 - Battery_ADC_Get(); // 4095에서 배터리 ADC 값을 뺀 값을 ADC에 저장
	if (adc >= 377) level = 4; // ADC 값이 377보다 크거나 같으면 레벨을 4로 설정
	else if (adc >= 306) level = 3; // ADC 값이 306보다 크거나 같으면 레벨을 3로 설정
	else if (adc >= 224) level = 2; // ADC 값이 224보다 크거나 같으면 레벨을 2로 설정
	else level = 1; // 그 외의 경우 레벨을 1로 설정


	if (YMD.Month == 1 || YMD.Month == 3 || YMD.Month == 5 || YMD.Month == 7|| YMD.Month == 8 || YMD.Month == 10 || YMD.Month == 12) {
		if (YMD.Day > 31) {
			YMD.Day = 31;
		}
	}
	if (YMD.Month == 4 || YMD.Month == 6 || YMD.Month == 9 || YMD.Month == 11) {
		if (YMD.Day > 30) {
			YMD.Day = 30;
		}
	}
	if (YMD.Month == 2) {
		if (YMD.Year == 0 || YMD.Year % 4 == 0) {
			if (YMD.Day > 29) {
				YMD.Day = 29;
			}
		} else {
			if (YMD.Day > 28) {
				YMD.Day = 28;
			}
		}
	}

	if (sw_it) {
		if (sw_it == 1)DFPlayNextTrack();
		if (sw_it == 2)DFPlayPreviousTrack();
		if (sw_it == 3)Volume_Up();
		if (sw_it == 4)Volume_Down();
		if (sw_it == 5)DFPlayThisTrack(track_N);
		if (sw_it == 6)DFPause();
		if (sw_it == 7) {
			DFPause();
			modeF = 2;                 //power saving mode
			sw_it = 0;
		}
		if (sw_it == 8) {
			DFPause();
			modeF = 1;                 //setting mode
			sw_it = 0;
		}
		sw_it = 0;
	}
/*
	if (HAL_GPIO_ReadPin(PH_CNCT_GPIO_Port, PH_CNCT_Pin) && !PHF) {
		DFR0299_Power_Off();
		PHF = 1;
	}
	if (!HAL_GPIO_ReadPin(PH_CNCT_GPIO_Port, PH_CNCT_Pin) && PHF) {
		DFR0299_Power_On();
		PHF = 0;
		DFSetVolume(Volume); // 볼륨 설정
	}*/



	gestur = PAJ7620_Get_Dir(); // 제스처 변수에 PAJ7620_Get_Dir() 함수 대입

	if (gestur) {                        // 제스처 변수가 참이면
		if (gestur == Down)Volume_Up(); // 제스처가 Down이면 볼륨 업
		if (gestur == Up)Volume_Down(); // 제스처가 Up이면 볼륨 다운
		if (gestur == Left)DFPlayNextTrack(); // 제스처가 Left이면 다음 트랙 재생
		if (gestur == Right)DFPlayPreviousTrack(); // 제스처가 Right이면 이전 트랙 재생
		gestur = 0;                      // 제스처 변수를 0으로 설정
	}
	OLED_Show_Picture(0, 0, 16, 16, Dir_Icon);
	if(Alarm_Flag) OLED_Show_Picture(104, 0, 16, 16, Alarm_Icon);

	if(HAL_GPIO_ReadPin(USB_CNCT_GPIO_Port, USB_CNCT_Pin)){
		if (HAL_GetTick() >= charge_tick){
			charge_tick = HAL_GetTick() + 200;
			charge_flag++;
			if (charge_flag > 4) charge_flag = 0;
		}
		if(charge_flag == 1) OLED_Show_Picture(120, 0, 8, 16, Battry_Icon1);
		if(charge_flag == 2) OLED_Show_Picture(120, 0, 8, 16, Battry_Icon2);
		if(charge_flag == 3) OLED_Show_Picture(120, 0, 8, 16, Battry_Icon3);
		if(charge_flag == 4) OLED_Show_Picture(120, 0, 8, 16, Battry_Icon4);
	} else {
		if(level == 1) OLED_Show_Picture(120, 0, 8, 16, Battry_Icon1);
		if(level == 2) OLED_Show_Picture(120, 0, 8, 16, Battry_Icon2);
		if(level == 3) OLED_Show_Picture(120, 0, 8, 16, Battry_Icon3);
		if(level == 4) OLED_Show_Picture(120, 0, 8, 16, Battry_Icon4);
	}



	sprintf(str_buf, "VOL:%02d", Volume);
	OLED_Show_Str((128 - 8 * 6) / 2, 16 * 0, str_buf, Font8x13, 0);

	sprintf(str_buf, "Track:%02d/%02d", track_N, track_A);
	OLED_Show_Str((128 - 8 * 11) / 2, 16 * 1, str_buf, Font8x13, 0);

	sprintf(str_buf, "20%02d-%02d-%02d", YMD.Year, YMD.Month, YMD.Day);
	OLED_Show_Str(8 * 0, 16 * 2, str_buf, Font8x13, 0);

	sprintf(str_buf, "%02d-%02d-%02d", YMD.Hour, YMD.Minute, YMD.Second);
	OLED_Show_Str(8 * 0, 16 * 3, str_buf, Font8x13, 0);

	DHT11_Read_Data(&DHT11);
	sprintf(str_buf, "T:%02dC", DHT11.Temp_Int);
	OLED_Show_Str(8 * 11, 16 * 2, str_buf, Font8x13, 0);

	sprintf(str_buf, "H:%02d%%", DHT11.Humi_Int);
	OLED_Show_Str(8 * 11, 16 * 3, str_buf, Font8x13, 0);

	OLED_Display();
}

void set_M() {
	fF=1;
	SW_Handler(0);
	//sleep_handler();
	now_P = 1;

	if (sw_it) {
		if (sw_it == 1) cursor--;
		if (sw_it == 2) cursor++;
		if (sw_it == 3)modeF = cursor;
		if (sw_it == 4)modeF = 0;
		sw_it = 0;
	}
	if (cursor == 13)
		cursor = 10;
	if (cursor == 9)
		cursor = 12;

	OLED_Show_Str((128 - 8 * 7) / 2, 0 * 16, "OPTIONS", Font8x13, 0);
	OLED_Show_Str(5, 1 * 16, "Date/Time Set ", Font8x13, cursor == 10);
	OLED_Show_Str(5, 2 * 16, "Alarm Set     ", Font8x13, cursor == 11);
	OLED_Show_Str(5, 3 * 16, "Sleep Mode Set", Font8x13, cursor == 12);
	OLED_Display();
}

void set_ymd_M() {
	SW_Handler(0);
	//sleep_handler();
	DS3231_Time_Type YMD; // DS3231 시간 타입 변수 선언
	YMD = DS3231_Get_Time();

	now_P = 10;

	if (sw_it) {
		if (sw_it == 1) {
			if (Scursor == 0) {
				YMD.Year++;
				if (YMD.Year == 100)YMD.Year = 0;
			}
			if (Scursor == 1) {YMD.Month++;
				if (YMD.Month == 13)YMD.Month = 1;
			}
			if (Scursor == 2) {
				YMD.Day++;
				if (YMD.Day == 32)YMD.Day = 1;
			}
			if (Scursor == 3) {
				YMD.Hour++;
				if (YMD.Hour == 61)YMD.Hour = 0;
			}
			if (Scursor == 4) {
				YMD.Minute++;
				if (YMD.Minute == 61)YMD.Minute = 0;
			}
			if (Scursor == 5) {
				YMD.Second++;
				if (YMD.Second == 61)YMD.Second = 0;
			}
		}
		if (sw_it == 2) {
			if (Scursor == 0) {
				YMD.Year--;
				if (YMD.Year == -1)YMD.Year = 99;
			}
			if (Scursor == 1) {
				YMD.Month--;
				if (YMD.Month == 0)YMD.Month = 12;
			}
			if (Scursor == 2) {
				YMD.Day--;
				if (YMD.Day == 0)YMD.Day = 31;
			}
			if (Scursor == 3) {
				YMD.Hour--;
				if (YMD.Hour == -1)YMD.Hour = 23;
			}
			if (Scursor == 4) {
				YMD.Minute--;
				if (YMD.Minute == -1)YMD.Minute = 59;
			}
			if (Scursor == 5) {
				YMD.Second--;
				if (YMD.Second == -1)YMD.Second = 59;
			}
		}
		if (sw_it == 3) {
			Scursor++;
			if (Scursor == 6) Scursor = 0;
		}
		if (sw_it == 4) modeF = 1;
		sw_it = 0;
	}
	DS3231_Set_Time(YMD);

	OLED_Show_Str((128 - 8 * 13) / 2, 0, "DATE/TIME SET", Font8x13, 0);

	OLED_Show_Str(8 * 3, 16 * 2 - 8, "20", Font8x13, Scursor == 0);

	sprintf(str_buf, "%02d", YMD.Year);
	OLED_Show_Str(8 * 5, 16 * 2 - 8, str_buf, Font8x13, Scursor == 0);

	OLED_Show_Str(8 * 7, 16 * 2 - 8, "-", Font8x13, 0);

	sprintf(str_buf, "%02d", YMD.Month);
	OLED_Show_Str(8 * 8, 16 * 2 - 8, str_buf, Font8x13, Scursor == 1);

	OLED_Show_Str(8 * 10, 16 * 2 - 8, "-", Font8x13, 0);

	sprintf(str_buf, "%02d", YMD.Day);
	OLED_Show_Str(8 * 11, 16 * 2 - 8, str_buf, Font8x13, Scursor == 2);

	sprintf(str_buf, "%02d", YMD.Hour);
	OLED_Show_Str(8 * 4, 16 * 3, str_buf, Font8x13, Scursor == 3);

	OLED_Show_Str(8 * 6, 16 * 3, ":", Font8x13, 0);

	sprintf(str_buf, "%02d", YMD.Minute);
	OLED_Show_Str(8 * 7, 16 * 3, str_buf, Font8x13, Scursor == 4);

	OLED_Show_Str(8 * 9, 16 * 3, ":", Font8x13, 0);

	sprintf(str_buf, "%02d", YMD.Second);
	OLED_Show_Str(8 * 10, 16 * 3, str_buf, Font8x13, Scursor == 5);

	//sprintf(str_buf,"%02d",YMD.Year);
	OLED_Display();
}

void set_alarm_M() {
	now_P = 11;
	SW_Handler(0);
	//sleep_handler();
	DS3231_Time_Type YMD;
	YMD = DS3231_Get_Alarm1();


	if (sw_it) {
		if (sw_it == 1) {
			if (A_cursor == 0)
				Alarm_Flag++;
			if (A_cursor == 1) {
				YMD.Hour++;
				if (YMD.Hour == 24)
					YMD.Hour = 0;
			}
			if (A_cursor == 2) {
				YMD.Minute++;
				if (YMD.Minute == 60)
					YMD.Minute = 0;
			}
		}
		if (sw_it == 2) {
			if (A_cursor == 0)
				Alarm_Flag++;
			if (A_cursor == 1) {
				YMD.Hour--;
				if (YMD.Hour == -1)
					YMD.Hour = 23;
			}
			if (A_cursor == 2) {
				YMD.Minute--;
				if (YMD.Minute == -1)
					YMD.Minute = 59;
			}
		}
		if (Alarm_Flag > 1)
			Alarm_Flag = 0;

		if (sw_it == 3) {
			A_cursor++;
			if (A_cursor == 3)
				A_cursor = 0;
		}
		if (sw_it == 4){
			DS3231_Set_Alarm1(YMD); // DS3231 알람 설정
			if(Alarm_Flag) DS3231_Enable_Alarm1();
			else DS3231_Disable_Alarm1();
			eeprom_8bit_write(1, Alarm_Flag);
			modeF = 1;
		}

		sw_it = 0;
	}

	OLED_Show_Str((128 - 8 * 9) / 2, 0, "ALARM SET", Font8x13, 0);

	OLED_Show_Str(8 * 0, 16 * 1, "Enable:", Font8x13, 0);
	if (Alarm_Flag)OLED_Show_Str(8 * 7, 16 * 1, "ON", Font8x13, A_cursor == 0);
	else OLED_Show_Str(8 * 7, 16 * 1, "OFF", Font8x13, A_cursor == 0);

	sprintf(str_buf, "%02d", YMD.Hour);
	OLED_Show_Str(28, 22 * 2 - 4, str_buf, Font12x22, A_cursor == 1);

	OLED_Show_Str(57, 22 * 2 - 4, ":", Font12x22, 0);

	sprintf(str_buf, "%02d", YMD.Minute);
	OLED_Show_Str(70, 22 * 2 - 4, str_buf, Font12x22, A_cursor == 2);



	OLED_Display();
}

void set_sleep_M() {
	now_P = 12;
	SW_Handler(0);
	//sleep_handler();
	if (sw_it) {
		if (sw_it == 1) {
			if (S_cursor == 0)
				Sleep_Flag++;
			if (S_cursor == 1) {
				S_min++;
				if (S_min == 24)
					S_min = 0;
			}
			if (S_cursor == 2) {
				S_sec++;
				if (S_sec == 60)
					S_sec = 0;
			}
		}
		if (sw_it == 2) {
			if (S_cursor == 0)
				Sleep_Flag++;
			if (S_cursor == 1) {
				S_min--;
				if (S_min == -1)
					S_min = 23;
			}
			if (S_cursor == 2) {
				S_sec--;
				if (S_sec == -1)
					S_sec = 59;
			}
		}
		if (Sleep_Flag > 1)
			Sleep_Flag = 0;

		if (sw_it == 3) {
			S_cursor++;
			if (S_cursor == 3)
				S_cursor = 0;
		}
		if (sw_it == 4){
			Sleep_Min = S_min;
			Sleep_Sec = S_sec;

			eeprom_8bit_write(2, Sleep_Flag);
			eeprom_8bit_write(3, Sleep_Min);
			eeprom_8bit_write(4, Sleep_Sec);
			modeF = 1;
		}

		sw_it = 0;
	}

	OLED_Show_Str((128 - 8 * 14) / 2, 0, "SLEEP MODE SET", Font8x13, 0);

	OLED_Show_Str(8 * 0, 16 * 1, "Enable:", Font8x13, 0);
	if (Sleep_Flag)
		OLED_Show_Str(8 * 7, 16 * 1, "ON", Font8x13, S_cursor == 0);
	else
		OLED_Show_Str(8 * 7, 16 * 1, "OFF", Font8x13, S_cursor == 0);

	sprintf(str_buf, "%02d", S_min);
	OLED_Show_Str(28, 22 * 2 - 4, str_buf, Font12x22, S_cursor == 1);

	OLED_Show_Str(57, 22 * 2 - 4, ":", Font12x22, 0);

	sprintf(str_buf, "%02d", S_sec);
	OLED_Show_Str(70, 22 * 2 - 4, str_buf, Font12x22, S_cursor == 2);

	OLED_Display();
}

void sleep_handler(){
	SW_Handler(0);
	if(sw_it){
		sleep_tick = HAL_GetTick();
		sw_it = 0;
	}
	if(Sleep_Flag && HAL_GetTick() - sleep_tick >= Sleep_Min * 60000 + Sleep_Sec * 1000) modeF = 2;;
}

void sleep_M() {
	SW_Handler(0);

	if (sw_it) {
		modeF = now_P;
		sw_it = 0;
	}
}

void loop(void) {
	Loading();
	OLED_Buffer_Clear();
	while (1) {
		OLED_Buffer_Clear();
		if (modeF == 0)main_M();
		if (modeF == 1)set_M();
		if (modeF == 2)	sleep_M();

		//set menu
		if (modeF == 10)set_ymd_M();
		if (modeF == 11)set_alarm_M();
		if (modeF == 12)set_sleep_M();
	}
}
