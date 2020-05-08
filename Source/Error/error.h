#ifndef _ERROR_H
#define _ERROR_H

#include "defines.h"






typedef enum {
	ERR_ID_COM_FAULT		= 1,
	ERR_ID_SW_UNMATCH		= 2,
	ERR_ID_WIND_PWR			= 3,
	ERR_ID_HEAT_OIL_PWR		= 4,
	ERR_ID_STEAM_WASH_PWR	= 6,
	ERR_ID_OVER_TEMP		= 7,
	ERR_ID_DOOR_BLOCK		= 8,	//!< left door
	ERR_ID_KEY_ADHESION		= 9,
	ERR_ID_FAN_OVER_TEMP	= 10,
	ERR_ID_AQS_OFFLINE		= 11,
	ERR_ID_WIFI_OFFLINE		= 12,
	ERR_ID_BLE_COM_FAULT	= 13,
	ERR_ID_DOOR_BLOCK2		= 14	//!< right door
}TErrorDef;





extern TErrorDef ERR_tGetErrorCode(void);





































#endif