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


#define MOT_PHASE1_PIN		0x01
#define MOT_SLEEP1_PIN		0x02
#define MOT_ENABLE1_PIN		0x04
#define MOT_PHASE2_PIN		0x08
#define MOT_SENSE_I1_PIN	0x10
#define MOT_SLEEP2_PIN		0x20
#define MOT_ENABLE2_PIN		0x40
#define MOT_SENSE_I2_PIN	0x80


#define MOT_SET_IO_HIGH(port, pin)	((port) |= (pin))
#define MOT_SET_IO_LOW(port, pin)	((port) &= ~(pin))

#define MOT_AD_SAMPLE_TIMES 5


typedef enum {
	MOT_TASK_INIT = 0,
	MOT_TASK_IDLE,
	MOT_TASK_CLOSE,
	MOT_TASK_OPEN
}TMotorTaskDef;

typedef enum {
	MOT_INDEX_LEFT = 0,
	MOT_INDEX_RIGHT,
	MOT_INDEX_NUM
}TMotorIndexDef;

extern void MOT_vInit(void);
extern void MOT_vTaskHandler(void);





















#endif









