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



static void TASK_vUpdateWorkingTime(void);

static TaskStruct TASK_tTaskTbl[] = {
/**running flag, 	init time slice,		time slice reset value,		task handler*/
	{0, 			TASK_MS2TICK(10), 		TASK_MS2TICK(10), 			TMR_vTimerIsr},		//!< must not changed, software timer
	{0, 			TASK_MS2TICK(1000), 	TASK_MS2TICK(1000), 		TASK_vUpdateWorkingTime},
	{0, 			TASK_MS2TICK(500), 		TASK_MS2TICK(500),			DIS_vBlinkTest},
	{0,				TASK_MS2TICK(0),		TASK_MS2TICK(0),			U2P_vDllTaskHandler},
	{0,				TASK_MS2TICK(0),		TASK_MS2TICK(0),			U2P_vPLTaskHandler},
	{0,				TASK_MS2TICK(6),		TASK_MS2TICK(6),			BUZZ_vTaskHandler}
};


#define TASK_NUMER_OF_TASKS	(sizeof(TASK_tTaskTbl) / sizeof(TASK_tTaskTbl[0]))

void TASK_vIRQHandler(void)
{
	uchar i = 0;

	for(i=0; i<TASK_NUMER_OF_TASKS; i++)
	{
		if(TASK_tTaskTbl[i].uiSliceValue)
		{
			TASK_tTaskTbl[i].uiSliceValue--;

			if(0 == TASK_tTaskTbl[i].uiSliceValue)
			{
				TASK_tTaskTbl[i].ucIsRun = 1u;
				TASK_tTaskTbl[i].uiSliceValue = TASK_tTaskTbl[i].uiSliceResetValue;
			}
		}
	}
}

void TASK_vScheduleTasks(void)
{
	uchar i = 0;

	for(i=0; i<TASK_NUMER_OF_TASKS; i++)
	{
		WDT_FEED();

		if(TASK_tTaskTbl[i].ucIsRun || (0==TASK_tTaskTbl[i].uiSliceResetValue))
		{
			TASK_tTaskTbl[i].pvTaskPointer();
			TASK_tTaskTbl[i].ucIsRun = 0;
		}
	}

	//U2P_vDllTaskHandler();
}

static void TASK_vUpdateWorkingTime(void)
{
	U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);
	BUZZ_vSetBuzzAlarm(BUZZ_RHYTHM_POWER_ON);
}

