#ifndef _LIGHT_H
#define _LIGHT_H

#include "defines.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif



#define LIG_IO_PORT		P1
#define LIG_IO_PIN		0x02


typedef enum {
	LIG_STATE_OFF = 0,
	LIG_STATE_ON
}TLigStateDef;



extern void LIG_vInit(void);
extern void LIG_vTaskHandler(void);
extern void LIG_vSetTargetState(TLigStateDef tState);
extern void LIG_vUpdateWorkingTime(void);



























#endif