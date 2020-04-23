#ifndef _MOTOR_H
#define _MOTOR_H

#include "defines.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif


#define MOT_RELAY1_PIN		0x40
#define MOT_RELAY2_PIN		0x20
#define MOT_RELAY3_PIN		0x10
#define MOT_RELAY4_PIN		0x08



#define MOT_RELAY1_ON()		(P2 |= MOT_RELAY1_PIN)
#define MOT_RELAY1_OFF()	(P2 &= ~MOT_RELAY1_PIN)

#define MOT_RELAY2_ON()		(P2 |= MOT_RELAY2_PIN)
#define MOT_RELAY2_OFF()	(P2 &= ~MOT_RELAY2_PIN)

#define MOT_RELAY3_ON()		(P1 |= MOT_RELAY3_PIN)
#define MOT_RELAY3_OFF()	(P1 &= ~MOT_RELAY3_PIN)

#define MOT_RELAY4_ON()		(P1 |= MOT_RELAY4_PIN)
#define MOT_RELAY4_OFF()	(P1 &= ~MOT_RELAY4_PIN)




typedef enum {
	MOT_SPEED_0 = 0,
	MOT_SPEED_1,
	MOT_SPEED_2,
	MOT_SPEED_3,
	MOT_SPEED_4
}TmotorSpeedDef;







extern void MOT_vInit(void);
extern void MOT_vSetSpeed(TmotorSpeedDef tSpeedIndex);






































#endif









