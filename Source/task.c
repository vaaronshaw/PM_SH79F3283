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


bit F_2ms;
bit F_10ms;
bit F_50ms;
bit F_1000ms;

static uint TASK_uiSysTick = 0;




static void TASK_vUpdateWorkingTime(void);

//static TtaskDef code TASK_tTaskTbl[] = {
//	/**slice reset value,		task handler*/
//	{TASK_MS2TICK(10),			TMR_vTimerIsr},		//!< must not changed, software timer
//	{TASK_MS2TICK(1000),		TASK_vUpdateWorkingTime},
//	{TASK_MS2TICK(0),			U2P_vDllTaskHandler},
//	{TASK_MS2TICK(0),			U2P_vPLTaskHandler},
//	{TASK_MS2TICK(6),			BUZZ_vTaskHandler},
//	{TASK_MS2TICK(50),			MOT_vTaskHandler},
//	{TASK_MS2TICK(100),			FAN_vTaskHandler},
//	{TASK_MS2TICK(100),			LIG_vTaskHandler}
//};

/**index sequence must match with TASK_tTaskTbl[]*/
////static TaskRunningInfoDef TASK_tTaskRunningInfo[] = {
////	/**Running Flah,			init slice time*/
////	{0, 						TASK_MS2TICK(10)},
////	{0, 						TASK_MS2TICK(1000)},
////	{0,							TASK_MS2TICK(0)},
////	{0,							TASK_MS2TICK(0)},
////	{0,							TASK_MS2TICK(6)},
////	{0, 						TASK_MS2TICK(50)},
////	{0, 						TASK_MS2TICK(100)},
////	{0, 						TASK_MS2TICK(100)}
////};

//#define TASK_NUMER_OF_TASKS	(sizeof(TASK_tTaskTbl) / sizeof(TASK_tTaskTbl[0]))

void TASK_vIRQHandler(void)
{
	//uchar i = 0;

	//for(i=0; i<TASK_NUMER_OF_TASKS; i++)
	//{
	//	if(TASK_tTaskRunningInfo[i].uiSliceValue)
	//	{
	//		TASK_tTaskRunningInfo[i].uiSliceValue--;

	//		if(0 == TASK_tTaskRunningInfo[i].uiSliceValue)
	//		{
	//			TASK_tTaskRunningInfo[i].ucIsRun = 1u;
	//			TASK_tTaskRunningInfo[i].uiSliceValue = TASK_tTaskTbl[i].uiSliceResetValue;
	//		}
	//	}
	//}
	TASK_uiSysTick++;	//!< every 2ms

	if ((TASK_uiSysTick % 1) == 0)
	{
		F_2ms = 1;
	}

	if ((TASK_uiSysTick % 5) == 0)
	{
		F_10ms = 1;
	}

	if ((TASK_uiSysTick % 25) == 0)
	{
		F_50ms = 1;
	}

	if ((TASK_uiSysTick % 500) == 0)
	{
		F_1000ms = 1;
	}

	if (TASK_uiSysTick >= 65500)
	{
		TASK_uiSysTick = 0;
	}
}

void TASK_vScheduleTasks(void)
{
	//uchar i = 0;

	//for(i=0; i<TASK_NUMER_OF_TASKS; i++)
	//{
	//	//WDT_FEED();

	//	if(TASK_tTaskRunningInfo[i].ucIsRun || (0==TASK_tTaskTbl[i].uiSliceResetValue))
	//	{
	//		TASK_tTaskTbl[i].pvTaskPointer();
	//		TASK_tTaskRunningInfo[i].ucIsRun = 0;
	//	}
	//}
	U2P_vDllTaskHandler();
	U2P_vPLTaskHandler();

	if (_testbit_(F_2ms))
	{
		BUZZ_vTaskHandler();
	}

	if (_testbit_(F_10ms))
	{
		TMR_vTimerIsr();
	}


	if (_testbit_(F_50ms))
	{
		MOT_vTaskHandler();
		FAN_vTaskHandler();
		LIG_vTaskHandler();
	}

	if (_testbit_(F_1000ms))
	{
		TASK_vUpdateWorkingTime();
	}
}

static void TASK_vUpdateWorkingTime(void)
{
	//U2P_vSetPropertyUploadFlag(U2P_PIID_MACHINE_STATE);
	//U2P_vSetPropertyUploadFlag(U2P_PIID_WIND_VOLUME);

	//U2P_vSetPropertyUploadFlag(U2P_PIID_LIGHT);
	//U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_CTRL);
	//U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_STATUS);

	//U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_CTRL2);
	//U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_STATUS2);
	//U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);	//!< test
	//BUZZ_vSetBuzzAlarm(BUZZ_RHYTHM_POWER_ON);	//!< test

	FAN_vUpdateWorkingTime();
	LIG_vUpdateWorkingTime();
}

