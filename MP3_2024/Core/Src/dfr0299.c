/*******************************************************************************
 * ----------------------------------------------------------------------------*
 *  elektronikaembedded@gamil.com ,https://elektronikaembedded.wordpress.com   *
 * ----------------------------------------------------------------------------*
 *                                                                             *
 * File Name  : dfr0299.c                                                      *
 *                                                                             *
 * Description : DFR0299 DFPlayer Mini Mp3 driver                              *
 *                                                                             *
 * Version     : PrototypeV1.0                                                 *
 *                                                                             *
 * --------------------------------------------------------------------------- *
 * Authors: Sarath S                                                           *
 * Date: May 16, 2017                                                          *
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "dfr0299.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

uint8_t DF_Rx_Buf[DFPLAYER_RECEIVED_LENGTH];
uint8_t DF_Tx_Buf[DFPLAYER_SEND_LENGTH] = { 0x7E, 0xFF, 06, 00, 01, 00, 00, 00, 00, 0xEF };

volatile uint8_t DF_Rx_Index = 0;
volatile int8_t DFRxData;
volatile uint8_t Stack_Packet;
volatile uint8_t done_flag = 0;

/* DFPlayer Status Structure */
typedef struct
{
	uint8_t onlineStatus;
	uint8_t replayStatus;
	uint8_t volume;
	uint8_t eq;
	uint8_t finishTrack;
	uint16_t trackNo;
	uint16_t TotalTrackNum;
} dfStatus_t;

dfStatus_t dfStatus;

/* DFPlayer Status Flag Structure */
typedef struct
{
	uint8_t onlineStatusReport;
	uint8_t replayStatusReport;
	uint8_t volumeReport;
	uint8_t eqReport;
	uint8_t finishTrackReport;
	uint16_t trackNoReport;
} dfStatusReport_t;

dfStatusReport_t dfStatusReport;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : Usart1_Interrupt
 *
 *  Description  :USART1 ISR
 *
 *  Input : None
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: October 12, 2023
 * ---------------------------------------------------------------------------*/
void Usart1_Interrupt(void)
{
	if ((USART1->ISR & UART_FLAG_RXNE) != RESET)
	{
		if (USART1->RDR == 0x7E)
		{
			DF_Rx_Index = 0;
		}
		DF_Rx_Buf[DF_Rx_Index] = USART1->RDR;
		DF_Rx_Index++;
		if (DF_Rx_Index > (DFPLAYER_RECEIVED_LENGTH - 1)) DF_Rx_Index = 0;
		if ((DF_Rx_Buf[Stack_Header] == 0x7E) && (DF_Rx_Buf[Stack_End] == 0xEF))
		{
			Stack_Packet = 1;
		}
		__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	}

}/* End of this function */

void Usart1_Init(void)
{
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFR0299Init
 *
 *  Description  :Initialization of DFR0299:Provide 1.5s~3s initialization time
 *                :DFR0299 Baudrate 9600bps
 *  Input : None
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFR0299Init(void)
{
	HAL_Delay(3000);                //Initialization dely 1.5s ~ 3s
	DFClearRXBuffer();            //Clear DFPlayer Reciver Buffer
	DFSendCommand(DFPLAYER_QUERY_SEND_PRM, 0, 0);    //Init DFPlayer
	HAL_Delay(10);
	DFSendCommand(DFPLAYER_CMD_SPECIFY_SOURCE, 0, DFPLAYER_DEVICE_SD); //Init TDFPlayer SD Card
	HAL_Delay(10);
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFPlayNextTrack
 *
 *  Description  :DFPlayer Next Track has to Play
 *
 *  Input : None
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/

void DFPlayNextTrack(void)
{
	DFSendCommand(DFPLAYER_CMD_NEXT, 0, 0);
	HAL_Delay(100);
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFPlayPreviousTrack
 *
 *  Description  :DFPlayer Previous Track has to Play
 *
 *  Input : None
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFPlayPreviousTrack(void)
{
	DFSendCommand(DFPLAYER_CMD_PREVIOUS, 0, 0);
	HAL_Delay(100);
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFPlayThisTrack
 *
 *  Description  :DFPlayer Play Specifc Track
 *
 *  Input : uint16_t Track_No
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFPlayThisTrack(uint16_t trackNo)
{
	uint8_t track_L = 0, track_H = 0;
	if (trackNo < 3000)
	{
		track_H = (uint8_t) ((trackNo >> 8) & 0x00FF);
		track_L = (uint8_t) (trackNo & 0x00FF);
		DFSendCommand(DFPLAYER_CMD_TRACK_NO, track_H, track_L);
		HAL_Delay(100);
	}
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFIncreaseVolume
 *
 *  Description  :DFPlayer Increase the Current Volume (0~30)
 *
 *  Input : None
 *
 *  Output : None
 *
 *  Return : Current Volume 
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFIncreaseVolume(void)
{
	dfStatus.volume++;
	if (dfStatus.volume >= 30) //Maximun Volume
	{
		dfStatus.volume = 30;
	}
	DFSendCommand(DFPLAYER_CMD_INC_VOL, 0, dfStatus.volume);
	HAL_Delay(100);
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFDecreaseVolume
 *
 *  Description  :DFPlayer Decrease the Current Volume (0~30)
 *
 *  Input : None
 *
 *  Output : None
 *
 *  Return : Current Volume 
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFDecreaseVolume(void)
{
	dfStatus.volume--;
	if (dfStatus.volume <= 0) //Minimun Volume
	{
		dfStatus.volume = 0;
	}
	DFSendCommand(DFPLAYER_CMD_DEC_VOL, 0, dfStatus.volume);
	HAL_Delay(100);
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFSetVolume
 *
 *  Description  :DFPlayer Set Volume  (0~30)
 *
 *  Input : unit8_t Volume
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFSetVolume(uint8_t vol)
{
	if ((vol <= 30) && (vol >= 0))
	{
		DFSendCommand(DFPLAYER_CMD_SPECIFY_VOL, 0, vol);
		HAL_Delay(100);
	}
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFSetEQ
 *
 *  Description  :DFPlayer Setting Equalizer
 *
 *  Input : uint8_t EQ
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFSetEQ(uint8_t eq)
{
	switch (eq)
	{
	case DFPLAYER_EQ_NORMAL:
		DFSendCommand(DFPLAYER_CMD_SPECIFY_EQ, 0, DFPLAYER_EQ_NORMAL);
		HAL_Delay(100);
		break;
	case DFPLAYER_EQ_POP:
		DFSendCommand(DFPLAYER_CMD_SPECIFY_EQ, 0, DFPLAYER_EQ_POP);
		HAL_Delay(100);
		break;
	case DFPLAYER_EQ_ROCK:
		DFSendCommand(DFPLAYER_CMD_SPECIFY_EQ, 0, DFPLAYER_EQ_ROCK);
		HAL_Delay(100);
		break;
	case DFPLAYER_EQ_JAZZ:
		DFSendCommand(DFPLAYER_CMD_SPECIFY_EQ, 0, DFPLAYER_EQ_JAZZ);
		HAL_Delay(100);
		break;
	case DFPLAYER_EQ_CLASSIC:
		DFSendCommand(DFPLAYER_CMD_SPECIFY_EQ, 0, DFPLAYER_EQ_CLASSIC);
		HAL_Delay(100);
		break;
	case DFPLAYER_EQ_BASS:
		DFSendCommand(DFPLAYER_CMD_SPECIFY_EQ, 0, DFPLAYER_EQ_BASS);
		HAL_Delay(100);
		break;
	default:
		break;
	}

}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFPlayBackMode
 *
 *  Description  :DFPlayer Playback Modes
 *
 *  Input : uint8_t EQ
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFPlayBackMode(uint8_t mode)
{
	switch (mode)
	{
	case DFPLAYER_PLAYBACK_REPEAT:
		DFSendCommand(DFPLAYER_CMD_SPECIFY_PLAYBACK, 0, DFPLAYER_PLAYBACK_REPEAT);
		HAL_Delay(100);
		break;
	case DFPLAYER_PLAYBACK_FOLDER_REPEAT:
		DFSendCommand(DFPLAYER_CMD_SPECIFY_PLAYBACK, 0, DFPLAYER_PLAYBACK_FOLDER_REPEAT);
		HAL_Delay(100);
		break;
	case DFPLAYER_PLAYBACK_SINGLE_REPEAT:
		DFSendCommand(DFPLAYER_CMD_SPECIFY_PLAYBACK, 0, DFPLAYER_PLAYBACK_SINGLE_REPEAT);
		HAL_Delay(100);
		break;
	case DFPLAYER_PLAYBACK_RANDOM:
		DFSendCommand(DFPLAYER_CMD_SPECIFY_PLAYBACK, 0, DFPLAYER_PLAYBACK_RANDOM);
		HAL_Delay(100);
		break;
	default:
		break;
	}

}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFResetModule
 *
 *  Description  :DFPlayer Module Reset
 *
 *  Input : None
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFResetModule(void)
{
	DFSendCommand(DFPLAYER_CMD_RESET_MODE, 0, 0);
	HAL_Delay(100);
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFPlayback
 *
 *  Description  :DFPlayer Playback Track
 *
 *  Input : None
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFPlayback(void)
{
	DFSendCommand(DFPLAYER_CMD_PLAYBACK, 0, 0);
	HAL_Delay(100);
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFPause
 *
 *  Description  :DFPlayer Pause Current Track
 *
 *  Input : None
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFPause(void)
{
	DFSendCommand(DFPLAYER_CMD_PAUSE, 0, 0);
	HAL_Delay(100);
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFPlayFolderTrack
 *
 *  Description  :DFPlayer DFPlayFolderTrack Specify Folder(1~99) and Track (1~255)
 *
 *  Input : uint8_t Folder,uint8_t Track
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFPlayFolderTrack(uint8_t folder, uint8_t track)
{
	if ((folder <= 99) && (track <= 255))
	{
		DFSendCommand(DFPLAYER_CMD_SPECIFY_FOLDER, folder, track);
		HAL_Delay(100);
	}
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFRepetTrack
 *
 *  Description  :DFPlayer Repet Track/Folder
 *
 *  Input : uint8_t Repet On(1) / Repet Off(0)
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFRepetTrack(uint8_t repetStatus)
{
	switch (repetStatus)
	{
	case REPEAT_ON:
		DFSendCommand(DFPLAYER_CMD_REPET_PLAY, 0, REPEAT_ON);
		HAL_Delay(100);
		break;
	case REPEAT_OFF:
		DFSendCommand(DFPLAYER_CMD_REPET_PLAY, 0, REPEAT_OFF);
		HAL_Delay(100);
		break;
	default:
		break;
	}

}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFGetCurrentVolume
 *
 *  Description  :DFPlayer Current Volume
 *
 *  Input : None
 *
 *  Output : uint8_t vol
 *
 *  Return : Current Volume 
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
uint8_t DFGetCurrentVolume(void)
{
	uint8_t vol;
	DFClearRXBuffer();
	DFSendCommand(DFPLAYER_QUERY_VOLUME, 0, 0);
	HAL_Delay(50);
	vol = dfStatus.volume;
	return vol;
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFGetCurrentTrack
 *
 *  Description  :DFPlayer Current Track
 *
 *  Input : None
 *
 *  Output : uint16_t trackNo
 *
 *  Return : Current Track Number
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
uint16_t DFGetCurrentTrack(void)
{
	DFSendCommand(DFPLAYER_QUERY_CURRENT_TRACK_TF, 0, 0);
	while (!done_flag);
	return dfStatus.trackNo;
}/* End of this function */

uint16_t DFGetTotalTrack(void)
{
	DFSendCommand(DFPLAYER_QUERY_U_DISK_CARD_FILES, 0, 0);
	while (!done_flag);
	return dfStatus.TotalTrackNum;
}

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFClearRXBuffer
 *
 *  Description  :Clear DFPlayer Reciver Buffer
 *
 *  Input : None
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFClearRXBuffer(void)
{
	for (DF_Rx_Index = 0; DF_Rx_Index < DFPLAYER_RECEIVED_LENGTH; DF_Rx_Index++)
	{
		DF_Rx_Buf[DF_Rx_Index] = '\0';
	}
	DF_Rx_Index = 0;
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : DFSendCommand
 *
 *  Description  :DFPlayer Command Sending Function 
 *
 *  Input : unit8_t CMD,unit8_t Para_H,unit8_t Para_H
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void DFSendCommand(uint8_t cmd, uint8_t para_H, uint8_t para_L)
{
	uint8_t frameIndex;
	uint16_t checkSum = 0;
	uint8_t checkSum_H, checkSum_L;

	DF_Tx_Buf[Stack_Command] = cmd;
	DF_Tx_Buf[Stack_Parameter_H] = para_H;
	DF_Tx_Buf[Stack_Parameter_L] = para_L;
	for (frameIndex = 1; frameIndex < 7; frameIndex++)
	{
		checkSum = checkSum + DF_Tx_Buf[frameIndex];
	}
	checkSum = (-1 * checkSum);
	checkSum_H = (uint8_t) ((checkSum >> 8) & 0x00FF);
	checkSum_L = (uint8_t) (checkSum & 0x00FF);

	DF_Tx_Buf[Stack_CheckSum_H] = checkSum_H;
	DF_Tx_Buf[Stack_CheckSum_L] = checkSum_L;
	done_flag = 0;

	for (frameIndex = 0; frameIndex < DFPLAYER_SEND_LENGTH; frameIndex++)
	{
		while ((USART1->ISR & UART_FLAG_TC) == 0);
		USART1->TDR = DF_Tx_Buf[frameIndex];
	}
}/* End of this function */

/* ----------------------------------------------------------------------------*
 *
 *  Function Name : ProcessDFPlayer
 *
 *  Description  : Process the DFPlayer Data
 *
 *  Input : None
 *
 *  Output : None
 *
 *  Return : None
 * ----------------------------------------------------------------------------*
 * Authors: Sarath S
 * Date: May 17, 2017
 * ---------------------------------------------------------------------------*/
void ProcessDFPlayer(void)
{
	if (Stack_Packet == 1)
	{
		if (DF_Rx_Buf[Stack_Command] == 0x3F && DF_Rx_Buf[Stack_Parameter_L] == 0x02) //Card Online
		{
			dfStatus.onlineStatus = 1;
			dfStatusReport.onlineStatusReport = notDone;
			Stack_Packet = 0;
			DFClearRXBuffer();
		}

		if (DF_Rx_Buf[Stack_Command] == 0x41) //Replay
		{
			dfStatus.replayStatus = 1;
			dfStatusReport.replayStatusReport = notDone;
			Stack_Packet = 0;
			DFClearRXBuffer();
		}
		if (DF_Rx_Buf[Stack_Command] == 0x43)  //Volume
		{
			dfStatus.volume = DF_Rx_Buf[Stack_Parameter_L];
			dfStatusReport.volumeReport = notDone;
			Stack_Packet = 0;
			DFClearRXBuffer();
		}
		if (DF_Rx_Buf[Stack_Command] == 0x4B) //Track Number
		{
			dfStatus.trackNo = DF_Rx_Buf[Stack_Parameter_L];
			dfStatus.trackNo = dfStatus.trackNo | (DF_Rx_Buf[Stack_Parameter_H] << 8);
			dfStatusReport.trackNoReport = notDone;
			Stack_Packet = 0;
			DFClearRXBuffer();
		}
		if (DF_Rx_Buf[Stack_Command] == DFPLAYER_QUERY_U_DISK_CARD_FILES) //Total Track Number
		{
			dfStatus.TotalTrackNum = DF_Rx_Buf[Stack_Parameter_L];
			dfStatus.TotalTrackNum = dfStatus.TotalTrackNum | (DF_Rx_Buf[Stack_Parameter_H] << 8);
			dfStatusReport.trackNoReport = notDone;
			Stack_Packet = 0;
			DFClearRXBuffer();
		}
		if (DF_Rx_Buf[Stack_Command] == 0x3D) //Finish Play
		{
			dfStatus.finishTrack = 1;
			dfStatus.trackNo = DF_Rx_Buf[Stack_Parameter_L];
			dfStatus.trackNo = dfStatus.trackNo | (DF_Rx_Buf[Stack_Parameter_H] << 8);
			dfStatusReport.finishTrackReport = notDone;
			Stack_Packet = 0;
			DFClearRXBuffer();
		}
		done_flag = 1;
	}

}/* End of this function */

/*******************************************************************************
 * End of file *
 *******************************************************************************/
