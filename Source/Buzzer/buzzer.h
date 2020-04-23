#ifndef _BUZZER_H
#define _BUZZER_H

#include "defines.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif

typedef enum _TBuzzerTask {
	BUZZ_TASK_INIT = 0,
	BUZZ_TASK_IDLE,
	BUZZ_TASK_ON,
	BUZZ_TASK_OFF
}TBuzzerTask;

typedef enum {
	BUZZ_TONE_INDEX_0 = 0,
	BUZZ_TONE_INDEX_1,
	BUZZ_TONE_INDEX_2,
	BUZZ_TONE_INDEX_NUM
}TtoneScaleIndex;



typedef enum _TBuzzerAlarm {
	BUZZ_RHYTHM_POWER_ON = 0,
	BUZZ_RHYTHM_POWER_OFF,
	BUZZ_RHYTHM_PAUSE,
	BUZZ_RHYTHM_START,
	BUZZ_RHYTHM_WASH_END,
	BUZZ_RHYTHM_ERROR,
	BUZZ_RHYTHM_SELECT,
	BUZZ_RHYTHM_INVALID,
	BUZZ_RHYTHM_NUM
}TBuzzerRhythmIndex;

typedef struct {
	uchar ucToneIndex;
	uint uiBuzzerOnTime;
	uint uiBuzzerOffTime;
}TRhythmConfig;

typedef struct {
	uint uiOnTime;
	uint uiOffTime;
	uchar ucRhythmCounter;
	TBuzzerTask tTaskState;
	TBuzzerRhythmIndex tIndex;
	uchar ucIndexInRhythm;
}TBuzzerInfo;

#define BUZZ_BASE_TICK	6
#define BUZZ_MS2TICK(x) ((x) / BUZZ_BASE_TICK)


//#define BUZZ_PWM_GPIO_PORT      GpioPortC
//#define BUZZ_PWM_GPIO_PIN       GpioPin13
//
//#define BUZZ_CTRL_GPIO_PORT		GpioPortC
//#define BUZZ_CTRL_GPIO_PIN		GpioPin14

#define BUZZ_CTRL_BEEP_ON()		 (BUZCON = 0x03)	//Gpio_ClrIO(BUZZ_CTRL_GPIO_PORT, BUZZ_CTRL_GPIO_PIN) 
#define BUZZ_CTRL_BEEP_OFF()	 (BUZCON = 0x02)	//Gpio_SetIO(BUZZ_CTRL_GPIO_PORT, BUZZ_CTRL_GPIO_PIN) 

#define BUZZ_OUTPUT_ENABLE()     (BUZCON = 0x03)	//{Tim3_M0_Enable_Output(TRUE); /*TIM3 端口输出使能*/Tim3_M0_Run();}
#define BUZZ_OUTPUT_DISABLE()    (BUZCON = 0x02)	//{Tim3_M0_Enable_Output(FALSE); /*TIM3 端口输出使能*/Tim3_M0_Stop();}


extern void BUZZ_vSetBuzzAlarm(TBuzzerRhythmIndex tType);
extern void BUZZ_vTaskHandler(void);

extern void BUZZ_vUpdateFrequency(uint uiValue);





































#endif