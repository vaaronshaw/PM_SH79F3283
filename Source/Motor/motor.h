#ifndef _MOTOR_H
#define _MOTOR_H

#include "api_ext.h"
#include "defines.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif


#define MOT_PHASE1_PORT		P3
#define MOT_SLEEP1_PORT		P3
#define MOT_ENABLE1_PORT	P3
#define MOT_PHASE2_PORT		P3
#define MOT_SENSE_I1_PORT	P3
#define MOT_SLEEP2_PORT		P3
#define MOT_ENABLE2_PORT	P3
#define MOT_SENSE_I2_PORT	P3

#define MOT_SWITCH_PORT		P1


#define MOT_PHASE1_PIN		0x01
#define MOT_SLEEP1_PIN		0x02	//!< P3.1, set low to enter low-power sleep mode
#define MOT_ENABLE1_PIN		0x04	//!< P3.2, set high to enable the H-bridge
#define MOT_PHASE2_PIN		0x08
#define MOT_SENSE_I1_PIN	0x10
#define MOT_SLEEP2_PIN		0x20
#define MOT_ENABLE2_PIN		0x40
#define MOT_SENSE_I2_PIN	0x80	//!< P3.7

#define MOT_SWITCH_PIN		0x04	//!< P1.2


#define MOT_AD_SAMPLE_TIMES 5

#define MOT_BLOCK_AD_VALUE_MAX		200		

typedef enum {
	MOT_STATE_CLOSE = 0,
	MOT_STATE_OPEN
}TMotorStateDef;


typedef enum {
	MOT_TASK_INIT = 0,
	MOT_TASK_IDLE,
	MOT_TASK_RUNNING
}TMotorTaskDef;

typedef enum {
	MOT_INDEX_LEFT = 0,
	MOT_INDEX_RIGHT,
	MOT_INDEX_NUM
}TMotorIndexDef;

extern void MOT_vInit(void);
extern void MOT_vTaskHandler(void);





















#endif









