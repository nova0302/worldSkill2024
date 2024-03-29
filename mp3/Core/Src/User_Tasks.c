//https://meet.google.com/bpn-eadc-vxy

#include <stdio.h>
#include "User_Tasks.h"

extern TIM_HandleTypeDef htim6;

uint32_t Tr_all = 0;
uint32_t Tr_now = 0;

uint8_t Volume = 0, Alarm_Flag = 0, Sleep_Flag = 0, Sleep_Min = 0, Sleep_Sec = 0;

uint16_t g_trackNo;
uint16_t g_numTotalTrack;

StAlarmTime_t   g_stAlarmTime;
EnSystemState_t g_SystemState;
EnSystemState_t g_SystemStateLast;
StDateTime_t    g_StDateTime;

StSleepMode_t g_StSleepMode;
uint32_t      g_ui32SleepModeCounter;
bool          g_bSleepMode;

StEventFifo_t g_StEventFifo;

void initQueue(StEventFifo_t *pFifo) { pFifo->head = pFifo->tail = 0; }
void enqueue(StEventFifo_t *pFifo, EnBtnEvent_t btnEvent) {
  pFifo->eBtnEventBuf[pFifo->head++] = btnEvent;
}
bool dequeue(StEventFifo_t *pFifo, EnBtnEvent_t *pBtnEvent) {
  if (pFifo->head == pFifo->tail) {
    return false;
  } else {
    *pBtnEvent = pFifo->eBtnEventBuf[pFifo->tail++];
    return true;
  }
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
void Alarm_Handler() { }
void initApp() {
  start_P();
  initDateTime(&g_StDateTime);
  //g_numTotalTrack = DFGetTotalTrack();

  // EepromLoad();
  g_SystemState = ST_SYS_MAIN;
  enqueue(&g_StEventFifo, EVT_IS_ENTRY);

  EnEepomBase_t base = EEP_OFFSET_DATE_TIME;
  loadFromEeprom((uint8_t*) &g_StDateTime, sizeof(StDateTime_t), base);

  base = EEP_OFFSET_SLEEP_MODE;
  loadFromEeprom((uint8_t*) &g_StSleepMode, sizeof(StSleepMode_t), base);
  g_bSleepMode = false;

}
void loop(void) {
  initApp();
  btnProcess_t btns[NUM_BTN];
  initBtns(&btns[0], NUM_BTN);
  while (1) {
    updateBtns(&btns[0], NUM_BTN);
    checkForPowerSave();
    updateMainPeriodFunc();

    switch (g_SystemState) {
    case ST_SYS_MAIN:              handleEvtMain();         break;
    case ST_SYS_MENU_SEL:          handleEvtSetting();          break;
    case ST_SYS_SETTING_SLEEP:     handleEvtSettingSleepMode(); break;
    case ST_SYS_SETTING_DATE_TIME: handleEvtSettingDateTime();  break;
    case ST_SYS_SETTING_ALARM:     handleEvtSettingAlarmSet();  break;
    case ST_SYS_SLEEP_MODE:        handleEvtPowSaveMode();      break;
    default: break;
    }
  }
}
void updateMainPeriodFunc(){
	static uint32_t last;
	uint32_t now = HAL_GetTick();
	if (now - last > 1000) {
		last = now;
		enqueue(&g_StEventFifo, EVT_ON_UPDATE_MAIN);
	}
}

// each event process
void handleEvtMain() {
  EnBtnEvent_t BtnEvent;
  if (!dequeue(&g_StEventFifo, &BtnEvent)) return;
  refreshSleepModeCounter(BtnEvent);

  switch (BtnEvent) {
  case EVT_IS_ENTRY:
  case EVT_ON_UPDATE_MAIN:   updateMain(); break;
  case EVT_BTN1_SHORT_PRESS: DFPlayNextTrack();          break;
  case EVT_BTN1_LONG_PRESS:  DFPlayThisTrack(g_trackNo); break;
  case EVT_BTN2_SHORT_PRESS: DFPlayPreviousTrack();      break;
  case EVT_BTN2_LONG_PRESS:  DFPause();                  break;
  case EVT_BTN3_SHORT_PRESS: Volume_Up();                break;
  case EVT_BTN3_LONG_PRESS:  enqueue(&g_StEventFifo, EVT_ON_POW_SAVE); break;
  case EVT_BTN4_SHORT_PRESS: Volume_Down(); break;
  case EVT_BTN4_LONG_PRESS:
    enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    g_SystemState = ST_SYS_MENU_SEL;
    break;
  case EVT_ON_POW_SAVE:
    enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    g_SystemStateLast = g_SystemState;
    g_SystemState = ST_SYS_SLEEP_MODE;
    break;
  default: break;
  }
}
void handleEvtSetting() {
  static int8_t cCurPos = 0;
  EnBtnEvent_t BtnEvent;
  if (!dequeue(&g_StEventFifo, &BtnEvent)) return;
  refreshSleepModeCounter(BtnEvent);
  switch (BtnEvent) {
  case EVT_IS_ENTRY:
    cCurPos = 0;
    OLED_Buffer_Clear();
    updateSettingMenu(cCurPos);
    break;
  case EVT_BTN1_SHORT_PRESS:
    if (--cCurPos < 0) cCurPos = 2;
    updateSettingMenu(cCurPos);
    break;
  case EVT_BTN2_SHORT_PRESS:
    if (++cCurPos > 2) cCurPos = 0;
    updateSettingMenu(cCurPos);
    break;
  case EVT_BTN3_SHORT_PRESS:
    enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    switch (cCurPos) {
    case 0: g_SystemState = ST_SYS_SETTING_DATE_TIME; break;
    case 1: g_SystemState = ST_SYS_SETTING_ALARM; break;
    case 2: g_SystemState = ST_SYS_SETTING_SLEEP; break;
    default: break;
    }
    break;
  case EVT_BTN4_SHORT_PRESS:
    enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    g_SystemState = ST_SYS_MAIN;
    break;
  case EVT_ON_POW_SAVE:
    enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    g_SystemStateLast = g_SystemState;
    g_SystemState = ST_SYS_SLEEP_MODE;
    break;
  default: break;
  }
}
void handleEvtSettingAlarmSet() {
  EnBtnEvent_t BtnEvent;
  if (!dequeue(&g_StEventFifo, &BtnEvent)) return;
  refreshSleepModeCounter(BtnEvent);

  switch (BtnEvent) {
  case EVT_IS_ENTRY:
    OLED_Buffer_Clear();
    updateSettingAlarmSet(&g_stAlarmTime);
    break;
  case EVT_BTN1_SHORT_PRESS:
    incAlarmTime(&g_stAlarmTime);
    updateSettingAlarmSet(&g_stAlarmTime);
    break;
  case EVT_BTN2_SHORT_PRESS:
    decAlarmTime(&g_stAlarmTime);
    updateSettingAlarmSet(&g_stAlarmTime);
    break;
  case EVT_BTN3_SHORT_PRESS:
    if (++g_stAlarmTime.curPos > 2) g_stAlarmTime.curPos = 0;
    updateSettingAlarmSet(&g_stAlarmTime);
    break;
  case EVT_BTN4_SHORT_PRESS:
    EnEepomBase_t offset = EEP_OFFSET_ALARM_SET;
    saveToEeprom((uint8_t*) &g_stAlarmTime, sizeof(StAlarmTime_t), offset);
    enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    g_SystemState = ST_SYS_MENU_SEL;
    break;
  case EVT_ON_POW_SAVE:
    enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    g_SystemStateLast = g_SystemState;
    g_SystemState = ST_SYS_SLEEP_MODE;
    break;
  default: break;
  }
}
void handleEvtSettingDateTime() {
  EnBtnEvent_t BtnEvent;
  if (!dequeue(&g_StEventFifo, &BtnEvent)) return;
  refreshSleepModeCounter(BtnEvent);

  switch (BtnEvent) {
  case EVT_IS_ENTRY:
    OLED_Buffer_Clear();
    updateSettingDateTime(&g_StDateTime);
    break;
  case EVT_BTN1_SHORT_PRESS:
    incDateTime(&g_StDateTime);
    updateSettingDateTime(&g_StDateTime);
    break;
  case EVT_BTN2_SHORT_PRESS:
    decDateTime(&g_StDateTime);
    updateSettingDateTime(&g_StDateTime);
    break;
  case EVT_BTN3_SHORT_PRESS:
    if (++g_StDateTime.cCurPos > 5) g_StDateTime.cCurPos = 0;
    updateSettingDateTime(&g_StDateTime);
    break;
    break;
  case EVT_BTN4_SHORT_PRESS:
    EnEepomBase_t offset = EEP_OFFSET_DATE_TIME;
    saveToEeprom((uint8_t*) &g_StDateTime, sizeof(StDateTime_t), offset);
    enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    g_SystemState = ST_SYS_MENU_SEL;
    break;
  case EVT_ON_POW_SAVE:
    enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    g_SystemStateLast = g_SystemState;
    g_SystemState = ST_SYS_SLEEP_MODE;
    break;
  default: break;
  }
}
void handleEvtSettingSleepMode() {
  EnBtnEvent_t BtnEvent;
  if (!dequeue(&g_StEventFifo, &BtnEvent)) return;
  refreshSleepModeCounter(BtnEvent);
  switch (BtnEvent) {
  case EVT_IS_ENTRY:
    OLED_Buffer_Clear();
    updateSettingSleepMode(); break;
  case EVT_BTN1_SHORT_PRESS: incSleepMode(); updateSettingSleepMode(); break;
  case EVT_BTN2_SHORT_PRESS: decSleepMode(); updateSettingSleepMode(); break;
  case EVT_BTN3_SHORT_PRESS:
    if (++g_StSleepMode.cCurPos > 2) {
      g_StSleepMode.cCurPos = 0;
    }
    updateSettingSleepMode(); break;
  case EVT_BTN4_SHORT_PRESS: {
    setSleepMode();
    EnEepomBase_t offset = EEP_OFFSET_SLEEP_MODE;
    saveToEeprom((uint8_t*) &g_StSleepMode, sizeof(StSleepMode_t), offset);
    enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    g_SystemState = ST_SYS_MENU_SEL;
  }
    break;
  case EVT_ON_POW_SAVE:
    enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    g_SystemStateLast = g_SystemState;
    g_SystemState = ST_SYS_SLEEP_MODE;
    break;
  default: break;
  }
}
void handleEvtPowSaveMode(){
  EnBtnEvent_t BtnEvent;
  if (!dequeue(&g_StEventFifo, &BtnEvent)) return;
  refreshSleepModeCounter(BtnEvent);

  switch (BtnEvent) {
  case EVT_IS_ENTRY:
    OLED_Buffer_Clear();
    updatePowSaveMode(); break;
  case EVT_BTN1_SHORT_PRESS:
  case EVT_BTN2_SHORT_PRESS:
  case EVT_BTN3_SHORT_PRESS:
  case EVT_BTN4_SHORT_PRESS:
	  //https://meet.google.com/dif-ovou-atk
   enqueue(&g_StEventFifo, EVT_IS_ENTRY);
    g_SystemState = g_SystemStateLast;
    break;
  default: break;
  }
}

// screen process
void updateMain() {
  //OLED_Clear();
  OLED_Buffer_Clear();
  char msgBuf[32];
  OLED_Show_Picture(  0, 0, 16, 16, Dir_Icon);
  OLED_Show_Picture(100, 0, 16, 16, Alarm_Icon);
  OLED_Show_Picture(120, 0,  8, 16, Battry_Icon);

  sprintf(msgBuf, "VOL:%02d", Volume);
  OLED_Show_Str(40, 3, msgBuf, Font8x13, 0);

  //sprintf(str_buf, "Track:%02ld/%02ld", Tr_now, Tr_all);
  //sprintf(msgBuf, "Track:%02d/%02d", (int)Tr_now, (int)g_numTotalTrack);
  //OLED_Show_Str(20, 19, msgBuf, Font8x13, 0);

  uint16_t usAdcValue = Battery_ADC_Get();
  double batteryLevel = usAdcValue * 3.3 / 4096;

#define VOLTAGE_DIVIDER
#ifdef VOLTAGE_DIVIDER
  batteryLevel *= 2;
#endif

  uint8_t dec = (int)batteryLevel;
  uint8_t fra = (int)(batteryLevel * 10) % 10;

  sprintf(msgBuf, "Bat Level: %d.%d", dec, fra);
  OLED_Show_Str(0, 13*2, msgBuf, Font8x13, 0);

  DHT11_Type dht11={0};
  getTempHumi(&dht11);
  sprintf(msgBuf, "T:%d.%d H:%d.%d"
	  ,dht11.Temp_Int ,dht11.Temp_Float
	  ,dht11.Humi_Int ,dht11.Humi_Float );
  OLED_Show_Str(0, 13*3, msgBuf, Font8x13, 0);

  OLED_Display();
}
void getTempHumi(DHT11_Type *pDht11){
  static DHT11_Type dht11_last;
  DHT11_Type dht11;

  bool bIsValid = DHT11_Read_Data(&dht11);
  if (bIsValid) {
    dht11_last = dht11;
  }
  memcpy(pDht11, &dht11_last, sizeof(DHT11_Type));
}
void updateSettingMenu(uint8_t curPos) {
  OLED_Show_Str(0,           0, "Options", Font8x13, 0);

  //OLED_Show_Str(0, 64 - 13 * 3, "Date/Time Set",  Font8x13, curPos == 0);
  //OLED_Show_Str(0, 64 - 13 * 2, "Alarm Set",      Font8x13, curPos == 1);
  //OLED_Show_Str(0, 64 - 13 * 1, "Sleep Mode Set", Font8x13, curPos == 2);
  OLED_Show_Str(0,           0, "Options", Font8x13, 0);
  char *msgArray[] ={"Date/Taime", "Alarm Set", "Sleep Mode Set"};
  for (int i=0; i<3; ++i) {
    OLED_Show_Str(0, 13 * (i+1), msgArray[i], Font8x13, curPos == i);
  }
  OLED_Display();
}
void updateSettingDateTime(StDateTime_t *pStDateTime) {

  char msgBuf[32];
  OLED_Show_Str(0, 0, "DATE/TIME SET", Font8x13, 0);

  /////////////////////////////////  date
  uint8_t xPos = 0;
  sprintf(msgBuf, "%4d", pStDateTime->stDate.usYear);
  OLED_Show_Str(xPos, 64 - 13 * 3, msgBuf, Font8x13, pStDateTime->cCurPos == 0);
  OLED_Display();

  xPos += strlen(msgBuf) * 8;
  OLED_Show_Str(xPos, 64 - 13 * 3, "-", Font8x13, 0);
  OLED_Display();

  xPos += 8;
  sprintf(msgBuf, " %02d", pStDateTime->stDate.ucMonth);
  OLED_Show_Str(xPos, 64 - 13 * 3, msgBuf, Font8x13, pStDateTime->cCurPos == 1);
  OLED_Display();

  xPos += strlen(msgBuf) * 8;
  OLED_Show_Str(xPos, 64 - 13 * 3, "-", Font8x13, 0);
  OLED_Display();

  xPos += 8;
  sprintf(msgBuf, "%02d", pStDateTime->stDate.ucDay);
  OLED_Show_Str(xPos, 64 - 13 * 3, msgBuf, Font8x13, pStDateTime->cCurPos == 2);
  OLED_Display();
  /////////////////////////////////  time
  xPos = 0;
  sprintf(msgBuf, "%02d", pStDateTime->stTime.cHour);
  OLED_Show_Str(xPos, 64 - 13 * 2, msgBuf, Font8x13, pStDateTime->cCurPos == 3);
  OLED_Display();

  xPos += strlen(msgBuf) * 8;
  OLED_Show_Str(xPos, 64 - 13 * 2, ":", Font8x13, 0);
  OLED_Display();

  xPos += 8;
  sprintf(msgBuf, "%02d", pStDateTime->stTime.cMinute);
  OLED_Show_Str(xPos, 64 - 13 * 2, msgBuf, Font8x13, pStDateTime->cCurPos == 4);
  OLED_Display();

  xPos += strlen(msgBuf) * 8;
  OLED_Show_Str(xPos, 64 - 13 * 2, ":", Font8x13, 0);
  OLED_Display();

  xPos += 8;
  sprintf(msgBuf, "%02d", pStDateTime->stTime.cSecond);
  OLED_Show_Str(xPos, 64 - 13 * 2, msgBuf, Font8x13, pStDateTime->cCurPos == 5);
  OLED_Display();
}
void updateSettingAlarmSet(StAlarmTime_t *pStAlarmTime) {
  char msgBuf[32];
  OLED_Show_Str(0, 0, "ALARM SET", Font8x13, 0);
  OLED_Show_Str(0, 64 - 13 * 3, "Enable: ", Font8x13, 0);
  OLED_Show_Str(0, 64 - 13 * 3, " ON", Font8x13, pStAlarmTime->curPos == 0);

  sprintf(msgBuf, "%02d :", pStAlarmTime->stTime.cHour);
  OLED_Show_Str(0, 64 - 13 * 2, msgBuf, Font8x13, pStAlarmTime->curPos == 1);

  sprintf(msgBuf, "%02d", pStAlarmTime->stTime.cMinute);
  OLED_Show_Str(0, 64 - 13 * 2, msgBuf, Font8x13, pStAlarmTime->curPos == 2);

  OLED_Display();
}
void updateSettingSleepMode() {
  char msgBuf[32];
  OLED_Show_Str(0, 0, "SLEEP MODE SET", Font8x13, 0);
  OLED_Display();

  uint8_t xPos = 0;
  sprintf(msgBuf, "%s", "Enable:");
  OLED_Show_Str(xPos, 64 - 13 * 3, msgBuf, Font8x13, 0);
  OLED_Display();

  xPos += strlen(msgBuf) * 8;
  sprintf(msgBuf, "%s", "   ");
  sprintf(msgBuf, "%s", (g_StSleepMode.bSleepMode) ? "ON" : "OFF");
  OLED_Show_Str(xPos, 64 - 13 * 3, msgBuf, Font8x13, g_StSleepMode.cCurPos == 0);
  OLED_Display();

  xPos = 0;
  sprintf(msgBuf, "%02u", g_StSleepMode.stTime.cMinute);
  OLED_Show_Str(xPos, 64 - 13 * 2, msgBuf, Font8x13, g_StSleepMode.cCurPos == 1);
  OLED_Display();

  xPos += strlen(msgBuf) * 8;
  OLED_Show_Str(xPos, 64 - 13 * 2, ":", Font8x13, 0);
  OLED_Display();

  xPos += 8;
  sprintf(msgBuf, "%02u", g_StSleepMode.stTime.cSecond);
  OLED_Show_Str(xPos, 64 - 13 * 2, msgBuf, Font8x13, g_StSleepMode.cCurPos == 2);

  OLED_Display();
}
void updatePowSaveMode(){
  char msgBuf[32];
  sprintf(msgBuf, "%s", "SLEEP MODE");
  //uint8_t xPos = 128/2 - strlen(msgBuf) * 12 / 2;
  //OLED_Show_Str(xPos, 64/2-11, msgBuf, Font12x22, 0);
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
void initBtns(btnProcess_t *pBtn, uint8_t numBtn ){
  initBtn(&pBtn[0], SW1_GPIO_Port, SW1_Pin, btn1CbShort, btn1CbLong);
  initBtn(&pBtn[1], SW2_GPIO_Port, SW2_Pin, btn2CbShort, btn2CbLong);
  initBtn(&pBtn[2], SW3_GPIO_Port, SW3_Pin, btn3CbShort, btn3CbLong);
  initBtn(&pBtn[3], SW4_GPIO_Port, SW4_Pin, btn4CbShort, btn4CbLong);
}
void updateBtns(btnProcess_t *pBtns, uint8_t numBtn ){
  for (int i = 0; i < numBtn; ++i)
    updateBtn(&pBtns[i]);
}

// publish btn events
void btn1CbShort() { enqueue(&g_StEventFifo, EVT_BTN1_SHORT_PRESS); }
void btn1CbLong()  { enqueue(&g_StEventFifo, EVT_BTN1_LONG_PRESS); }
void btn2CbShort() { enqueue(&g_StEventFifo, EVT_BTN2_SHORT_PRESS); }
void btn2CbLong()  { enqueue(&g_StEventFifo, EVT_BTN2_LONG_PRESS); }
void btn3CbShort() { enqueue(&g_StEventFifo, EVT_BTN3_SHORT_PRESS); }
void btn3CbLong()  { enqueue(&g_StEventFifo, EVT_BTN3_LONG_PRESS); }
void btn4CbShort() { enqueue(&g_StEventFifo, EVT_BTN4_SHORT_PRESS); }
void btn4CbLong()  { enqueue(&g_StEventFifo, EVT_BTN4_LONG_PRESS); }

// utility functions
/*
  void printDbgMessage(uint8_t btnNumber, bool bIsShortPress) {
  char msgBuf[64];
  sprintf(msgBuf, "btn%d %s pressed!!", btnNumber, bIsShortPress ? "short" : "long");
  OLED_Show_Str(0, 0, msgBuf, Font8x13, 0);
  OLED_Display();
  }
*/
uint8_t getLastDayOfMonth(StDateTime_t *pStDateTime) {

  uint8_t ret = 0xff;
  switch (pStDateTime->stDate.ucMonth) {
  case 1:
  case 3:
  case 5:
  case 7:
  case 8:
  case 10:
  case 12:
    ret = 31;
    break;
  case 6:
  case 9:
  case 11:
    ret = 30;
    break;
  case 2:
    //占썩몿�뫒亦낆룈�뜮癒��땻占� 占쎈뎨占쎈뿪�돇
    uint16_t usYear = pStDateTime->stDate.usYear;
    if ((((usYear % 4) == 0) && ((usYear % 100) != 0))
	|| ((usYear % 400) == 0)) {
      ret = 29;
    } else {
      ret = 28;
    }
    break;
  default:
    break;
  }
  return ret;
}
void incDateTime(StDateTime_t *pStDateTime) {
  //if (++pStDateTime->curPos > 5) pStDateTime->curPos = 0;
  switch (pStDateTime->cCurPos) {
  case 0:
    if (++pStDateTime->stDate.usYear > 2099)
      pStDateTime->stDate.usYear = 2000;
    break;
  case 1:
    if (++pStDateTime->stDate.ucMonth > 12)
      pStDateTime->stDate.ucMonth = 1;
    break;
  case 2:
    if (++pStDateTime->stDate.ucDay > getLastDayOfMonth(pStDateTime))
      pStDateTime->stDate.ucDay = 1;
    break;
  case 3:
    if (++pStDateTime->stTime.cHour > 23)
      pStDateTime->stTime.cHour = 0;
    break;
  case 4:
    if (++pStDateTime->stTime.cMinute > 59)
      pStDateTime->stTime.cMinute = 0;
    break;
  case 5:
    if (++pStDateTime->stTime.cSecond > 59)
      pStDateTime->stTime.cSecond = 0;
    break;
  default:
    break;
  }
}
void decDateTime(StDateTime_t *pStDateTime) {
  //if (++pStDateTime->curPos > 5) pStDateTime->curPos = 0;
  switch (pStDateTime->cCurPos) {
  case 0:
    if (--pStDateTime->stDate.usYear < 2000)
      pStDateTime->stDate.usYear = 2099;
    break;
  case 1:
    if (--pStDateTime->stDate.ucMonth < 1)  // there is no 0 month
      pStDateTime->stDate.ucMonth = 12;
    break;
  case 2:
    if (--pStDateTime->stDate.ucDay < 1)  // there is no 0 day
      pStDateTime->stDate.ucDay = getLastDayOfMonth(pStDateTime);
    break;
  case 3:
    if (--pStDateTime->stTime.cHour < 0)
      pStDateTime->stTime.cHour = 23;
    break;
  case 4:
    if (--pStDateTime->stTime.cMinute < 0)
      pStDateTime->stTime.cMinute = 59;
    break;
  case 5:
    if (--pStDateTime->stTime.cSecond < 0)
      pStDateTime->stTime.cSecond = 59;
    break;
  default:
    break;
  }
}
void initDateTime(StDateTime_t *pStDateTime) {
  pStDateTime->cCurPos = 0;
  pStDateTime->stDate.usYear = 2000;
  pStDateTime->stDate.ucMonth = 1;
  pStDateTime->stDate.ucDay = 1;
  pStDateTime->stTime.cHour = 0;
  pStDateTime->stTime.cMinute = 0;
  pStDateTime->stTime.cSecond = 0;
}
void incAlarmTime(StAlarmTime_t *pStAlarmTime) {
  switch (pStAlarmTime->curPos) {
  case 0:
    if (++pStAlarmTime->stTime.cHour > 23)
      pStAlarmTime->stTime.cHour = 0;
    break;
  case 1:
    if (++pStAlarmTime->stTime.cMinute > 59)
      pStAlarmTime->stTime.cHour = 0;
    break;
  case 2:
    pStAlarmTime->bEnable ^= 1;
    break;
  default:
    break;
  }
}
void decAlarmTime(StAlarmTime_t *pStAlarmTime) {
  switch (pStAlarmTime->curPos) {
  case 0:
    if (--pStAlarmTime->stTime.cHour < 0)
      pStAlarmTime->stTime.cHour = 23;
    break;
  case 1:
    if (--pStAlarmTime->stTime.cMinute < 0)
      pStAlarmTime->stTime.cHour = 59;
    break;
  case 2:
    pStAlarmTime->bEnable ^= 1;
    break;
  default:
    break;
  }
}
void saveToEeprom(uint8_t *pData, size_t size, EnEepomBase_t base) {
  for (uint16_t i = 0; i < size; ++i) {
    eeprom_8bit_write(base + i, *pData++);
  }
}
void loadFromEeprom(uint8_t *pData, size_t size, EnEepomBase_t base) {
  for (int i = 0; i < size; ++i) {
    *pData++ = eeprom_8bit_read(base + i);
  }
}
void incSleepMode() {
  switch (g_StSleepMode.cCurPos) {
  case 0:
    g_StSleepMode.bSleepMode ^= 1;
    break;
  case 1:
    if (++g_StSleepMode.stTime.cMinute > 59) {
      g_StSleepMode.stTime.cMinute = 0;
    }
    break;
  case 2:
    if (g_StSleepMode.stTime.cMinute == 0) {
      if (++g_StSleepMode.stTime.cSecond > 59) {
	g_StSleepMode.stTime.cSecond = 10;
      }

    } else {
      if (++g_StSleepMode.stTime.cSecond > 59) {
	g_StSleepMode.stTime.cSecond = 0;
      }
    }
    break;
  default: break;
  }
}
void decSleepMode() {
  switch (g_StSleepMode.cCurPos) {
  case 0:
    g_StSleepMode.bSleepMode ^= 1;
    break;
  case 1:
    if (--g_StSleepMode.stTime.cMinute < 0) {
      g_StSleepMode.stTime.cMinute = 23;
    }
    break;
  case 2:
    if (g_StSleepMode.stTime.cMinute == 0) {
      if (--g_StSleepMode.stTime.cSecond < 10)
	g_StSleepMode.stTime.cSecond = 59;
    } else {
      if (--g_StSleepMode.stTime.cSecond < 0)
	g_StSleepMode.stTime.cSecond = 59;
    }
    break;
  default: break;
  }
}
void setSleepMode(){
  g_bSleepMode = g_StSleepMode.bSleepMode;
  g_ui32SleepModeCounter = g_StSleepMode.stTime.cMinute * 60
    + g_StSleepMode.stTime.cSecond;
}
void refreshSleepModeCounter(EnBtnEvent_t event){
  switch (event) {
  case EVT_BTN1_SHORT_PRESS: 
  case EVT_BTN2_SHORT_PRESS: 
  case EVT_BTN3_SHORT_PRESS: 
  case EVT_BTN4_SHORT_PRESS: 
    g_ui32SleepModeCounter = g_StSleepMode.stTime.cMinute * 60
      + g_StSleepMode.stTime.cSecond;
    break;
  default: break;
  }
}
void checkForPowerSave(){
  static	uint32_t last;
  uint32_t now = HAL_GetTick();
  if(now - last > 1000){
    last = now;
    if(g_bSleepMode){
      if (--g_ui32SleepModeCounter > 0) {
      }else{
	//g_bSleepMode = false;
	enqueue(&g_StEventFifo, EVT_ON_POW_SAVE);
      }
    }
  }
}

/*
void delayUs(uint16_t us){
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	while (__HAL_TIM_GET_COUNTER(&htim6) < us) ;
}
*/




