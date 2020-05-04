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


#define FAN_RELAY1_PORT		P2
#define FAN_RELAY2_PORT		P2
#define FAN_RELAY3_PORT		P1
#define FAN_RELAY4_PORT		P1

#define FAN_RELAY1_PIN		0x40
#define FAN_RELAY2_PIN		0x20
#define FAN_RELAY3_PIN		0x10
#define FAN_RELAY4_PIN		0x08

#define FAN_SET_IO_HIGH(port, pin)	((port) |= (pin))
#define FAN_SET_IO_LOW(port, pin)	((port) &= ~(pin))

#define FAN_GET_IO_STATE(port, pin) ((port) & (pin))

//#define FAN_RELAY1_ON()		(FAN_RELAY1_PORT |= FAN_RELAY1_PIN)
//#define FAN_RELAY1_OFF()	(FAN_RELAY1_PORT &= ~FAN_RELAY1_PIN)
//
//#define FAN_RELAY2_ON()		(FAN_RELAY2_PORT |= FAN_RELAY2_PIN)
//#define FAN_RELAY2_OFF()	(FAN_RELAY2_PORT &= ~FAN_RELAY2_PIN)
//
//#define FAN_RELAY3_ON()		(FAN_RELAY3_PORT |= FAN_RELAY3_PIN)
//#define FAN_RELAY3_OFF()	(FAN_RELAY3_PORT &= ~FAN_RELAY3_PIN)
//
//#define FAN_RELAY4_ON()		(FAN_RELAY4_PORT |= FAN_RELAY4_PIN)
//#define FAN_RELAY4_OFF()	(FAN_RELAY4_PORT &= ~FAN_RELAY4_PIN)




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












































#endif