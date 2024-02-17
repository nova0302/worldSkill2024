/****************************************************************************** *
 * ---------------------------------------------------------------------------- *
 *  elektronikaembedded@gamil.com ,https://elektronikaembedded.wordpress.com  	*
 * ---------------------------------------------------------------------------- *
 *                                                                              *
 * File Name  : DFR0299.h                                                       *
 *                                                                              *
 * Description : DFR0299.c header file                                            *
 *                                                                              *
 * Version     : PrototypeV1.0                                                  *
 *                                                                              *
 * ---------------------------------------------------------------------------  *
 * Authors: Sarath S                                                            *
 * Date: May 16, 2017                                                           *
 ********************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DFR0299_H
#define __DFR0299_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

//Serial Communication Commands
// 1. Directly send commands, no parameters returned

#define DFPLAYER_CMD_NEXT                    0x01
#define DFPLAYER_CMD_PREVIOUS                0x02
#define DFPLAYER_CMD_TRACK_NO                0x03
#define DFPLAYER_CMD_INC_VOL                 0x04
#define DFPLAYER_CMD_DEC_VOL                 0x05
#define DFPLAYER_CMD_SPECIFY_VOL             0x06
#define DFPLAYER_CMD_SPECIFY_EQ              0x07
#define DFPLAYER_CMD_SPECIFY_PLAYBACK        0x08
#define DFPLAYER_CMD_SPECIFY_SOURCE          0x09
#define DFPLAYER_CMD_LOW_POWER_MODE          0x0A
#define DFPLAYER_CMD_NORMAL_MODE             0x0B
#define DFPLAYER_CMD_RESET_MODE              0x0C
#define DFPLAYER_CMD_PLAYBACK                0x0D
#define DFPLAYER_CMD_PAUSE                   0x0E
#define DFPLAYER_CMD_SPECIFY_FOLDER          0x0F
#define DFPLAYER_CMD_VOL_ADJUST              0x10
#define DFPLAYER_CMD_REPET_PLAY              0x11

// Query the System Parameters

#define DFPLAYER_QUERY_STAY1				0x3C
#define DFPLAYER_QUERY_STAY2				0x3D
#define DFPLAYER_QUERY_STAY3				0x3E
#define DFPLAYER_QUERY_SEND_PRM		        0x3F
#define DFPLAYER_QUERY_ERROR				0x40
#define DFPLAYER_QUERY_REPLY				0x41
#define DFPLAYER_QUERY_STATUS		        0x42
#define DFPLAYER_QUERY_VOLUME				0x43
#define DFPLAYER_QUERY_EQ					0x44
#define DFPLAYER_QUERY_PLAYBACK_MODE		0x45
#define DFPLAYER_QUERY_SOFT_VERSION			0x46
#define DFPLAYER_QUERY_TF_CARD_FILES		0x47
#define DFPLAYER_QUERY_U_DISK_CARD_FILES	0x48
#define DFPLAYER_QUERY_FLASH_CARD_FILES	    0x49
#define DFPLAYER_QUERY_KEEPON		        0x4A
#define DFPLAYER_QUERY_CURRENT_TRACK_TF	    0x4B
#define DFPLAYER_QUERY_CURRENT_TRACK_U_DISK	0x4C
#define DFPLAYER_QUERY_CURRENT_TRACK_FLASH	0x4D


//Command Parameter Stack_Parameter_H,Stack_Parameter_L

#define DFPLAYER_EQ_NORMAL      0
#define DFPLAYER_EQ_POP         1
#define DFPLAYER_EQ_ROCK        2
#define DFPLAYER_EQ_JAZZ        3
#define DFPLAYER_EQ_CLASSIC     4
#define DFPLAYER_EQ_BASS        5

#define DFPLAYER_DEVICE_U_DISK  0
#define DFPLAYER_DEVICE_SD      1
#define DFPLAYER_DEVICE_AUX     2
#define DFPLAYER_DEVICE_SLEEP   3
#define DFPLAYER_DEVICE_FLASH   4

#define DFPLAYER_PLAYBACK_REPEAT                0
#define DFPLAYER_PLAYBACK_FOLDER_REPEAT         1
#define DFPLAYER_PLAYBACK_SINGLE_REPEAT         2
#define DFPLAYER_PLAYBACK_RANDOM                3

#define REPEAT_ON  1
#define REPEAT_OFF 0



//#define _DEBUG

#define TimeOut 0
#define WrongStack 1
#define DFPlayerCardInserted    2
#define DFPlayerCardRemoved     3
#define DFPlayerCardOnline      4
#define DFPlayerPlayFinished    5
#define DFPlayerError           6

#define Busy                    1
#define Sleeping                2
#define SerialWrongStack        3
#define CheckSumNotMatch        4
#define FileIndexOut            5
#define FileMismatch            6
#define Advertise               7

// DFPLAYER Frame Length and Index

#define DFPLAYER_RECEIVED_LENGTH        10
#define DFPLAYER_SEND_LENGTH            10

#define Stack_Header            0
#define Stack_Version           1
#define Stack_Length            2
#define Stack_Command           3
#define Stack_ACK               4
#define Stack_Parameter_H       5
#define Stack_Parameter_L       6
#define Stack_CheckSum_H        7
#define Stack_CheckSum_L        8
#define Stack_End               9

#define RECIVED 1
#define SUCCES 1
#define FAIL 0
#define WAITING 0

#define Online  1
#define Offline 0
#define PushIn   1
#define PushOut  0

#define done 0
#define notDone 1



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
void DFR0299Init(void);

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
void DFPlayNextTrack(void);

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
void DFPlayPreviousTrack(void);

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
void DFPlayThisTrack(uint16_t trackNo);

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
void DFIncreaseVolume(void);

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
void DFDecreaseVolume(void);

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
void DFSetVolume(uint8_t vol);

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
void DFSetEQ(uint8_t eq);

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
void DFPlayBackMode(uint8_t mode);

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
void DFResetModule(void);

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
void DFPlayback(void);

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
void DFPause(void);

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
void DFPlayFolderTrack(uint8_t folder,uint8_t track);

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
void DFRepetTrack(uint8_t repetStatus);


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
uint8_t DFGetCurrentVolume(void);

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
uint16_t DFGetCurrentTrack(void);

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

uint16_t DFGetTotalTrack(void);


void DFClearRXBuffer(void);


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
void DFSendCommand(uint8_t cmd,uint8_t para_H,uint8_t para_L);


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
void ProcessDFPlayer(void);



/* ----------------------------------------------------------------------------*
 *
 *  Function Name : dfplayerreport
 *
 *  Description  : report DFplayer Messages
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
void dfplayerreport(void);

void Usart1_Init(void);

#endif /* __DFR0299_H */

/******* https://elektronikaembedded.wordpress.com  *****END OF FILE***********/
