#ifndef _FAN_H
#define _FAN_H

#include "defines.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif

/**io configuration for fan control*/
#define FAN_RELAY1_PORT		P2
#define FAN_RELAY2_PORT		P2
#define FAN_RELAY3_PORT		P1
#define FAN_RELAY4_PORT		P1

#define FAN_RELAY1_PIN		0x40
#define FAN_RELAY2_PIN		0x20
#define FAN_RELAY3_PIN		0x10
#define FAN_RELAY4_PIN		0x08


#define FAN_RUNNING_TIME_MAX	6*3600	//!< 4 hours

typedef enum {
	FAN_SPEED_0 = 0,
	FAN_SPEED_1,
	FAN_SPEED_2,
	FAN_SPEED_3,
	FAN_SPEED_4
}TFanSpeedDef;







extern void FAN_vInit(void);


extern void FAN_vTaskHandler(void);
extern void FAN_vSetTargetSpeed(TFanSpeedDef tSpeedIndex);
extern void FAN_vUpdateWorkingTime(void);











































#endif