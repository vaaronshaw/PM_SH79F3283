#ifndef _UART_2_PM_H
#define _UART_2_PM_H

#include "defines.h"


/**Basic configuration, please adjust too meet project's needs*/
#define U2P_NO_ACK_RETRY_TIMES			3
#define U2P_NO_ACK_TIME					200	//!< ms
#define U2P_BYTE_INTERVAL_TIME			20	//!< ms

/**Message ID and data length definition*/
#define U2P_MSG_HEAD_DEVICE		0xCC
#define U2P_MSG_HEAD_FORWARD	0xCD	//!< forward message

#define U2P_MSG_ID_ACK					0x01
#define U2P_MSG_DATALEN_ACK				0

#define U2P_MSG_ID_SW_INFO_SYNC			0x02
#define U2P_MSG_DATALEN_SW_INFO_SYNC		4

#define U2P_MSG_ID_PROD_INFO_SYNC		0x03
#define U2P_MSG_DATALEN_PROD_INFO_SYNC	0xFF	//!< data lengtn is variable

#define U2P_MSG_ID_HEART_BEAT			0x04
#define U2P_MSG_DATALEN_HEART_BEAT		0

#define U2P_MSG_ID_STATUS_GET			0x07
#define U2P_MSG_DATALEN_STATUS_GET		0xFF	//!< data lengtn is variable

#define U2P_MSG_ID_STATUS_SET			0x08
#define U2P_MSG_DATALEN_STATUS_SET		0xFF	//!< data lengtn is variable

typedef struct {
	uchar ucHead;
	uchar ucMsgSeqValue;
	uchar ucMsgID;
	uchar ucDataLen;
}TmsgHeader;

typedef struct {
	TmsgHeader tMsgHeader;
	uchar ucData[30];   //!< maximum size of data
}Tu2pMessage;

typedef struct {
	uchar ucMsgID;
	uchar ucAckRequire;
	uchar ucDataLen;
	void (*pServiceFunc)(Tu2pMessage* tMsgObject);// reentrant;
}Tu2pMessageObject;

typedef struct {
	uchar ucPIID;
	uchar ucPiidValueLen;
	void (*pPropGetFunc)(Tu2pMessage* tMsgObject);// reentrant;
	void (*pPropSetFunc)(Tu2pMessage* tMsgObject);// reentrant;
}TpropertyConfig;


enum {
	U2P_NO_ACK = 0,
	U2P_ACK = 1
};

typedef enum {
	U2P_TASK_INIT = 0,
	U2P_TASK_IDLE,
	U2P_TASK_RECEIVE_SEQ_NUM,
	U2P_TASK_RECEIVE_ID,
	U2P_TASK_RECEIVE_DATALEN,
	U2P_TASK_RECEIVE_DATA,
	U2P_TASK_SEND_DATA
}TU2PDllTaskIndex;

/**index must be the same as U2P_tPropertyTable[]*/
enum {
	U2P_PIID_MACHINE_STATE = 0,		//!< Machine State
	U2P_PIID_WIND_VOLUME,			//!< wind volume
	//U2P_PIID_LIGHT,					//!< light 
	//U2P_PIID_WIND_PRESSURE,			//!< wind pressure
	//U2P_PIID_TURBO_STATE,			//!< Turbo State
	//U2P_PIID_AIR_CONTROL,			//!< air control
	//U2P_PIID_AIR_QUALITY,			//!< air quality
	U2P_PIID_DELAY_TIME,			//!< delay time (s)
	//U2P_PIID_DELAY_SPEED,			//!< delay speed
	//U2P_PIID_BOOST_TIME,			//!< boost time
	//U2P_PIID_POWER_FEEDBACK,		//!< power feedback
	//U2P_PIID_HEAT_OIL_CTRL,			//!< heat oil control
	//U2P_PIID_STEAM_WASH_CTRL,		//!< steam wash control
	//U2P_PIID_STEAM_WASH_TEMP,		//!< steam wash temp
	//U2P_PIID_DOOR_CTRL,				//!< door control
	//U2P_PIID_DOOR_STATE,			//!< door state
	//U2P_PIID_DOOR_RESISTANCE,		//!< door resistance value
	//U2P_PIID_MOTOR_TEMP,			//!< motor temp
	//U2P_PIID_DETECTION_TEMP1,		//!< detection temp1
	//U2P_PIID_DETECTION_TEMP2,		//!< detection temp2
	//U2P_PIID_BUZZER_CTRL,			//!< buzzer control
	//U2P_PIID_VOICE_CTRL,			//!< voice control
	//U2P_PIID_IR_CTRL,				//!< IR control
	//U2P_PIID_INTERLINK_CTRL,		//!< interlink control
	//U2P_PIID_CLEAN_REMIND,			//!< clean remind
	//U2P_PIID_KEY_VALUE,				//!< key value
	//U2P_PIID_INDICATOR_LIGHT,		//!< indicator light control value
	//U2P_PIID_GESTURE_CTRL,			//!< gesture control
	//U2P_PIID_ERROR,					//!< error
};



extern void U2P_vInit();
extern void U2P_vTxEventHandler(void);
extern void U2P_vRxEventHandler(uchar ucData);

extern BOOL U2P_bIsSameSeqValue(void);
extern void U2P_vDllTaskHandler(void);
extern void U2P_vPLTaskHandler(void);
extern void U2P_vTransmitMessage(uchar ucMsgID);



























































#endif