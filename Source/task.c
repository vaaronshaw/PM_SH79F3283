/*
 * task.c
 *
 *  Created on: 2018��9��19��
 *      Author: hp
 */


#include "task.h"
#include "sw_timer.h"
#include "Button/button.h"
#include "Display/display.h"
#include "Uart/Uart2PM.h"
#include "wdt.h"
#include "Buzzer/buzzer.h"
#include "Motor/motor.h"
#include "Fan/fan.h"
#include "Light/light.h"



static void TASK_vUpdateWorkingTime(void);

static TtaskDef code TASK_tTaskTbl[] = {
	/**slice reset value,		task handler*/
	{TASK_MS2TICK(10),			TMR_vTimerIsr},		//!< must not changed, software timer
	{TASK_MS2TICK(1000),		TASK_vUpdateWorkingTime},
	{TASK_MS2TICK(0),			U2P_vDllTaskHandler},
	{TASK_MS2TICK(0),			U2P_vPLTaskHandler},
	{TASK_MS2TICK(6),			BUZZ_vTaskHandler},
	{TASK_MS2TICK(50),			MOT_vTaskHandler},
	{TASK_MS2TICK(100),			FAN_vTaskHandler},
	{TASK_MS2TICK(100),			LIG_vTaskHandler}
};

/**index sequence must match with TASK_tTaskTbl[]*/
static TaskRunningInfoDef TASK_tTaskRunningInfo[] = {
	/**Running Flah,			init slice time*/
	{0, 						TASK_MS2TICK(10)},
	{0, 						TASK_MS2TICK(1000)},
	{0,							TASK_MS2TICK(0)},
	{0,							TASK_MS2TICK(0)},
	{0,							TASK_MS2TICK(6)},
	{0, 						TASK_MS2TICK(50)},
	{0, 						TASK_MS2TICK(100)},
	{0, 						TASK_MS2TICK(100)}
};

#define TASK_NUMER_OF_TASKS	(sizeof(TASK_tTaskTbl) / sizeof(TASK_tTaskTbl[0]))

void TASK_vIRQHandler(void)
{
	uchar i = 0;

	for(i=0; i<TASK_NUMER_OF_TASKS; i++)
	{
		if(TASK_tTaskRunningInfo[i].uiSliceValue)
		{
			TASK_tTaskRunningInfo[i].uiSliceValue--;

			if(0 == TASK_tTaskRunningInfo[i].uiSliceValue)
			{
				TASK_tTaskRunningInfo[i].ucIsRun = 1u;
				TASK_tTaskRunningInfo[i].uiSliceValue = TASK_tTaskTbl[i].uiSliceResetValue;
			}
		}
	}
}

void TASK_vScheduleTasks(void)
{
	uchar i = 0;

	for(i=0; i<TASK_NUMER_OF_TASKS; i++)
	{
		//WDT_FEED();

		if(TASK_tTaskRunningInfo[i].ucIsRun || (0==TASK_tTaskTbl[i].uiSliceResetValue))
		{
			TASK_tTaskTbl[i].pvTaskPointer();
			TASK_tTaskRunningInfo[i].ucIsRun = 0;
		}
	}
}

static void TASK_vUpdateWorkingTime(void)
{
	U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);
	BUZZ_vSetBuzzAlarm(BUZZ_RHYTHM_POWER_ON);
	FAN_vUpdateWorkingTime();
}

