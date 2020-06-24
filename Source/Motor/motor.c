#include "motor.h"
#include "Uart2PM.h"
#include <string.h>
#include "sw_timer.h"


static volatile TMotorTaskDef MOT_tTaskState[MOT_INDEX_NUM] = {0, 0};
static volatile uint MOT_uiRawAdcValue[MOT_INDEX_NUM][MOT_AD_SAMPLE_TIMES];
static volatile uint MOT_uiAdValue[MOT_INDEX_NUM] = {0, 0};
static volatile uchar MOT_ucRawAdcValueCount[MOT_INDEX_NUM] = {0, 0};

static TMotorStateDef MOT_tCtrlStateToSet[MOT_INDEX_NUM] = {0 , 0};
static TMotorStateDef MOT_tCtrlStateToSetBuffer[MOT_INDEX_NUM] = { 0 , 0 };

static TMotorStateDef MOT_tCtrlStateOfCurrent[MOT_INDEX_NUM] = {0 , 0};
static TMotorStateDef MOT_tCurrentStatus[MOT_INDEX_NUM] = { 0 , 0 };

static uint MOT_uiMotorRunningTime[MOT_INDEX_NUM] = {0, 0};
static uint MOT_uiMotorOffTime[MOT_INDEX_NUM] = { 0, 0 };

static uint MOT_uiMotorBlockTime[MOT_INDEX_NUM] = { 0, 0 };
static uint MOT_uiTargetRunningTime[MOT_INDEX_NUM] = {0 , 0};
//static uchar MOT_ucMidPositionLocateFlag = 0;	//!< flag of position request after first power on
static uchar MOT_ucMidPositionLocateRetryCount = 0;

static ulong MOT_ulOpenTime[MOT_INDEX_NUM] = {0 , 0};






static uchar MOT_ucSetMotorState(TMotorIndexDef tMotorIndex);
static uchar MOT_ucEnterIdleState(TMotorIndexDef tMotorIndex);
static void MOT_vUpdateRunningTime(void);

void MOT_vInit(void)
{
	/**io init
	*P3.0: PHASE1
	*P3.1: SLEEP1
	*P3.2: ENABLE1
	*P3.3: PHASE2
	*P3.4: SENSE_I1
	*P3.5: SLEEP2
	*P3.6: ENABLE2
	*P3.7: SENSE_I2
	*/
	P3CR |= (MOT_PHASE1_PIN | MOT_SLEEP1_PIN | MOT_ENABLE1_PIN | MOT_PHASE2_PIN | MOT_SLEEP2_PIN | MOT_ENABLE2_PIN);	//!< output
	P3PCR &= ~(MOT_PHASE1_PIN | MOT_SLEEP1_PIN | MOT_ENABLE1_PIN | MOT_PHASE2_PIN | MOT_SLEEP2_PIN | MOT_ENABLE2_PIN);	//!< no pullup
	P3 &= ~(MOT_PHASE1_PIN | MOT_SLEEP1_PIN | MOT_ENABLE1_PIN | MOT_PHASE2_PIN | MOT_SLEEP2_PIN | MOT_ENABLE2_PIN);	//!< low
	//P3 |= (MOT_PHASE1_PIN | MOT_SLEEP1_PIN | MOT_ENABLE1_PIN | MOT_PHASE2_PIN | MOT_SLEEP2_PIN | MOT_ENABLE2_PIN);	//!< low

	/**P1.2: SWITCH, INT46*/
	P1CR &= ~(MOT_SWITCH_PIN);	//!< input
	P1PCR |= (MOT_SWITCH_PIN);	//!< no pull up
	IEN1 |= 0x08;	//!< enable EX4 isr
	//IENC |= 0x40;	//!< enable EXS46
	EXF0 |= 0x80;	//!< rise edge //0xC0;	//!< both edge trig
	EXF1 &= ~0x40;	//!< clear isr flag

	//MOT_SWITCH_ISR_DISABLE();

	/**adc init for input voltage detection*/
	ADCON = 0x88;	//!< ADON=1, VDD as vref,  ADC channel = AN4, GO/DONE = 0
	ADCON1 = 0x00;	//!< 12bits resolution

	/***************************************************************************************
	*********************************************************************.*******************/
	ADT = 0xA7;	//!< tAD = 16*tsys = 2us, TS = (7+1)*tAD = 16us, Tconvert = 12*tAD + TS = 40us
	ADCH = 0x90;	//!<P3.4 as AN4, P3.7 as AN7
}

static uint MOT_uiGetAdValue(uint* puiRawValue)
{
	ulong ulSum = 0;
	uint uiMin = puiRawValue[0];
	uint uiMax = puiRawValue[0];
	uchar i = 0;

	for (i=0; i< MOT_AD_SAMPLE_TIMES; i++)
	{
		ulSum += puiRawValue[i];

		if (uiMin > puiRawValue[i])
		{
			uiMin = puiRawValue[i];
		}

		if (uiMax < puiRawValue[i])
		{
			uiMax = puiRawValue[i];
		}
	}

	return (uint)((ulSum - (ulong)uiMax - (ulong)uiMin) / (MOT_AD_SAMPLE_TIMES - 2));
}

static void MOT_vAdSampleTask(TMotorIndexDef tMotorIndex)
{
	uint uiTempH;
	uint uiTempL;
	/********************************************************
	* start AD conversion
	********************************************************/
	if (MOT_INDEX_UP_DOWN == tMotorIndex)
	{
		/**AN4*/
		ADCON = 0x88;
		ADCON1 = 0x00;
	}
	else
	{
		/**AN7*/
		ADCON = 0x8E;
		ADCON1 = 0x00;
	}

	Delay();

	ADCON |= 0x01;   //!< ADC GO
	while ((ADCON & 0x01))	//!< check go/done 
	{
		/**wait*/
	}

	//get result
	uiTempH = (uint)ADDH;
	uiTempL = (uint)ADDL;
	MOT_uiRawAdcValue[tMotorIndex][MOT_ucRawAdcValueCount[tMotorIndex]++] = (uiTempH << 4) + (uiTempL & 0x0F);

	if (MOT_ucRawAdcValueCount[tMotorIndex] >= MOT_AD_SAMPLE_TIMES)
	{
		MOT_ucRawAdcValueCount[tMotorIndex] = 0;
		MOT_uiAdValue[tMotorIndex] = MOT_uiGetAdValue(&MOT_uiRawAdcValue[tMotorIndex][0]);
	}
}

static void MOT_vUpDownTaskHandler(TMotorIndexDef tMotorIndex)
{

	switch (MOT_tTaskState[tMotorIndex])
	{
		default:
		case MOT_TASK_INIT:
		{
			MOT_tTaskState[tMotorIndex] = MOT_TASK_IDLE;
			(void)MOT_ucEnterIdleState(tMotorIndex);

			MOT_tCtrlStateToSet[tMotorIndex] = MOT_STATE_CLOSE;		//!< test
			MOT_tCtrlStateToSetBuffer[tMotorIndex] = MOT_STATE_CLOSE;		//!< test

			MOT_tCtrlStateOfCurrent[tMotorIndex] = MOT_STATE_OPEN;	//!< test
			MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_OPEN;	//!< init

			break;
		}
			
		case MOT_TASK_IDLE:
		{
#if (MOT_DRIVE_MODE_DELAY == MOT_DRIVE_MODE)
			if (MOT_tCtrlStateToSetBuffer[tMotorIndex] != MOT_tCtrlStateToSet[tMotorIndex])
			{
				MOT_tCtrlStateToSetBuffer[tMotorIndex] = MOT_tCtrlStateToSet[tMotorIndex];	//!< update buffer with UI
			}

			if (MOT_tCtrlStateOfCurrent[tMotorIndex] != MOT_tCtrlStateToSetBuffer[tMotorIndex])
			{
				if (SET_OK == MOT_ucSetMotorState(tMotorIndex))
				{
					MOT_tTaskState[tMotorIndex] = MOT_TASK_RUNNING;

					/**clear ad value*/
					MOT_ucRawAdcValueCount[tMotorIndex] = 0;
					MOT_uiAdValue[tMotorIndex] = 0;
					(void)memset(MOT_uiRawAdcValue[tMotorIndex], 0, MOT_AD_SAMPLE_TIMES);

					if (MOT_STATE_OPEN == MOT_tCtrlStateToSetBuffer[tMotorIndex])
					{
						MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_OPENING;
					}
					else
					{
						MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_CLOSING;
					}
				}
			}
#elif (MOT_DRIVE_MODE_REAL_TIME == MOT_DRIVE_MODE)
			if (MOT_tCtrlStateOfCurrent[tMotorIndex] != MOT_tCtrlStateToSet[tMotorIndex])
			{
				if (MOT_uiMotorOffTime[tMotorIndex] >= MOT_TIME_MS2TICKS(500))	//!< move again after motor stop over 0.5 seconds
				{
					if (SET_OK == MOT_ucSetMotorState(tMotorIndex))
					{
						MOT_tTaskState[tMotorIndex] = MOT_TASK_RUNNING;

						MOT_tCtrlStateOfCurrent[tMotorIndex] = MOT_tCtrlStateToSet[tMotorIndex];	//!< sync state here

						/**clear ad value*/
						MOT_ucRawAdcValueCount[tMotorIndex] = 0;
						MOT_uiAdValue[tMotorIndex] = 0;
						(void)memset(MOT_uiRawAdcValue[tMotorIndex], 0, MOT_AD_SAMPLE_TIMES);

						if (MOT_STATE_OPEN == MOT_tCtrlStateToSet[tMotorIndex])
						{
							MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_OPENING;
						}
						else
						{
							MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_CLOSING;
						}
					}
				}
			}
#endif

			break;
		}

		case MOT_TASK_RUNNING:
		{
			MOT_vAdSampleTask(tMotorIndex);

			/**
			* 1. check ad value, if exceeds threshold means reach target position, then back to idle state
			*/
			if ((MOT_uiAdValue[tMotorIndex] > MOT_BLOCK_AD_VALUE_MAX) || (MOT_uiMotorRunningTime[tMotorIndex] >= MOT_TIME_SEC2TICKS(10)))	//!< ToDo: check it
			{
				if (SET_OK == MOT_ucEnterIdleState(tMotorIndex))
				{
					MOT_tTaskState[tMotorIndex] = MOT_TASK_IDLE;
#if (MOT_DRIVE_MODE_DELAY == MOT_DRIVE_MODE)
					MOT_tCtrlStateOfCurrent[tMotorIndex] = MOT_tCtrlStateToSetBuffer[tMotorIndex];
					MOT_tCurrentStatus[tMotorIndex] = MOT_tCtrlStateToSetBuffer[tMotorIndex];	//!< update final status
#elif (MOT_DRIVE_MODE_REAL_TIME == MOT_DRIVE_MODE)
					MOT_tCurrentStatus[tMotorIndex] = MOT_tCtrlStateToSet[tMotorIndex];
#endif

					/**upload status to UI*/
					if (MOT_INDEX_UP_DOWN == tMotorIndex)
					{
						U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_CTRL);
						U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_STATUS);
					}

					U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);
				}
			}

#if (MOT_DRIVE_MODE_REAL_TIME == MOT_DRIVE_MODE)
			/**target position changed, back to idle state first*/
			if (MOT_tCtrlStateOfCurrent[tMotorIndex] != MOT_tCtrlStateToSet[tMotorIndex])
			{
				if (SET_OK == MOT_ucEnterIdleState(tMotorIndex))
				{
					MOT_tTaskState[tMotorIndex] = MOT_TASK_IDLE;
				}
			}
#endif

			break;
		}		
	}
}

static void MOT_vLeftRightTaskHandler(TMotorIndexDef tMotorIndex)
{
	switch (MOT_tTaskState[tMotorIndex])
	{
		default:
		case MOT_TASK_INIT:
		{
			(void)MOT_ucEnterIdleState(tMotorIndex);

			MOT_tCtrlStateToSet[tMotorIndex] = MOT_STATE_MIDDLE;		//!< test
			MOT_tCtrlStateToSetBuffer[tMotorIndex] = MOT_STATE_MIDDLE;		//!< test

			/**check position*/
			if (GET_IO_STATE(MOT_SWITCH_PORT, MOT_SWITCH_PIN))
			{
				/**middle*/
				MOT_tCtrlStateOfCurrent[tMotorIndex] = MOT_STATE_MIDDLE;	//!< test
				MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_MIDDLE;	//!< init

				/**upload status to UI*/
				if (MOT_INDEX_LEFT_RIGHT == tMotorIndex)
				{
					U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_CTRL2);
					U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_STATUS2);
					U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);
				}
			}
			else
			{
				//MOT_ucMidPositionLocateFlag = 1;
				MOT_ucMidPositionLocateRetryCount = 0;

				MOT_SWITCH_ISR_ENABLE();
				MOT_tCtrlStateOfCurrent[tMotorIndex] = MOT_STATE_LEFT;	//!< test
				MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_LEFT;	//!< init
			}

			MOT_tTaskState[tMotorIndex] = MOT_TASK_IDLE;


			break;
		}


		case MOT_TASK_IDLE:
		{
			if (MOT_tCtrlStateToSetBuffer[tMotorIndex] != MOT_tCtrlStateToSet[tMotorIndex])
			{
				MOT_tCtrlStateToSetBuffer[tMotorIndex] = MOT_tCtrlStateToSet[tMotorIndex];	//!< update buffer with UI

				if (MOT_STATE_MIDDLE == MOT_tCtrlStateToSetBuffer[tMotorIndex])
				{
					/**target position is middle*/
					MOT_ucMidPositionLocateRetryCount = 0;	//!< reset value
				}
			}

			if (MOT_tCtrlStateOfCurrent[tMotorIndex] != MOT_tCtrlStateToSetBuffer[tMotorIndex])
			{
				if (MOT_uiMotorOffTime[tMotorIndex] >= MOT_TIME_MS2TICKS(500))	//!< move again after motor stop over 0.5 seconds
				{
					if (SET_OK == MOT_ucSetMotorState(tMotorIndex))
					{
						MOT_tTaskState[tMotorIndex] = MOT_TASK_RUNNING;
						MOT_uiMotorBlockTime[tMotorIndex] = 0;

						if (MOT_STATE_MIDDLE == MOT_tCtrlStateToSetBuffer[tMotorIndex])
						{
							/**target position is middle*/
							MOT_SWITCH_ISR_ENABLE();
							MOT_uiTargetRunningTime[tMotorIndex] = MOT_TIME_SEC2TICKS(18);	//!< should not exceeds 
						}
						else if ((MOT_tCtrlStateOfCurrent[tMotorIndex] == MOT_STATE_MIDDLE))
						{
							/**target position is left/right, and current is middle*/
							MOT_uiTargetRunningTime[tMotorIndex] = MOT_TIME_SEC2TICKS(17);	//!< should not exceeds 
						}
						else
						{
							/**target position is left/right, and current is not middle*/
							MOT_uiTargetRunningTime[tMotorIndex] = MOT_TIME_SEC2TICKS(35);	//!< should not exceeds 
						}

						/**clear ad value*/
						MOT_ucRawAdcValueCount[tMotorIndex] = 0;
						MOT_uiAdValue[tMotorIndex] = 0;
						(void)memset(MOT_uiRawAdcValue[tMotorIndex], 0, MOT_AD_SAMPLE_TIMES);

						MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_OPENING;
					}
				}
			}

			break;
		}

		case MOT_TASK_RUNNING:
		{
			MOT_vAdSampleTask(tMotorIndex);

			/**
			* 1. check ad value, if exceeds threshold means reach target position, then back to idle state
			* 2.
			*/
			if (MOT_uiAdValue[tMotorIndex] > 160)
			{
				MOT_uiMotorBlockTime[tMotorIndex]++;
			}
			else
			{
				MOT_uiMotorBlockTime[tMotorIndex] = 0;
			}

			if ((MOT_uiMotorBlockTime[tMotorIndex] >= MOT_TIME_MS2TICKS(800)) || (MOT_uiMotorRunningTime[tMotorIndex] >= MOT_uiTargetRunningTime[tMotorIndex]))
			{
				if (SET_OK == MOT_ucEnterIdleState(tMotorIndex))
				{
					MOT_tTaskState[tMotorIndex] = MOT_TASK_IDLE;

					/**if locating middle position*/
					if (MOT_STATE_MIDDLE == MOT_tCtrlStateToSetBuffer[tMotorIndex])
					{
						if (++MOT_ucMidPositionLocateRetryCount > MOT_MIDDLE_POSITION_RETRY_TIMES)
						{
							/**exceeds max retry times*/
							MOT_SWITCH_ISR_DISABLE();

							/**force update current state*/
							MOT_tCtrlStateOfCurrent[tMotorIndex] = MOT_tCtrlStateToSetBuffer[tMotorIndex];
							MOT_tCurrentStatus[tMotorIndex] = MOT_tCtrlStateToSetBuffer[tMotorIndex];
						}
						else
						{
							/**not reach middle point, should reverse direction*/
							if (MOT_tCtrlStateOfCurrent[tMotorIndex] != MOT_STATE_RIGHT)
							{
								MOT_tCtrlStateOfCurrent[tMotorIndex] = MOT_STATE_RIGHT;
								MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_RIGHT;
							}
							else
							{
								MOT_tCtrlStateOfCurrent[tMotorIndex] = MOT_STATE_LEFT;
								MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_LEFT;
							}
						}
					}
					else
					{
						MOT_tCtrlStateOfCurrent[tMotorIndex] = MOT_tCtrlStateToSetBuffer[tMotorIndex];
						MOT_tCurrentStatus[tMotorIndex] = MOT_tCtrlStateToSetBuffer[tMotorIndex];	//!< update final status
					}

					/**upload status to UI*/
					if (MOT_INDEX_LEFT_RIGHT == tMotorIndex)
					{
						U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_CTRL2);
						U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_STATUS2);

						U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);
					}
				}
			}

#if (MOT_DRIVE_MODE_REAL_TIME == MOT_DRIVE_MODE)
			/**target position changed, back to idle state first*/
			if (MOT_tCtrlStateToSetBuffer[tMotorIndex] != MOT_tCtrlStateToSet[tMotorIndex])
			{
				MOT_tCtrlStateToSetBuffer[tMotorIndex] = MOT_tCtrlStateToSet[tMotorIndex];

				if (MOT_STATE_MIDDLE == MOT_tCtrlStateToSetBuffer[tMotorIndex])
				{
					/**target position is middle*/
					MOT_ucMidPositionLocateRetryCount = 0;	//!< reset value
				}

				if (SET_OK == MOT_ucEnterIdleState(tMotorIndex))
				{
					MOT_tTaskState[tMotorIndex] = MOT_TASK_IDLE;
				}
			}
#endif

			break;
		}
	}
}

void MOT_vFinishPosition(void)
{
	if (MOT_uiMotorRunningTime[MOT_INDEX_LEFT_RIGHT] > MOT_TIME_MS2TICKS(300))
	{
		if (MOT_STATE_MIDDLE == MOT_tCtrlStateToSetBuffer[MOT_INDEX_LEFT_RIGHT])
		{
			if (SET_OK == MOT_ucEnterIdleState(MOT_INDEX_LEFT_RIGHT))
			{
				MOT_tTaskState[MOT_INDEX_LEFT_RIGHT] = MOT_TASK_IDLE;
				MOT_SWITCH_ISR_DISABLE();

				MOT_tCtrlStateOfCurrent[MOT_INDEX_LEFT_RIGHT] = MOT_STATE_MIDDLE;
				MOT_tCurrentStatus[MOT_INDEX_LEFT_RIGHT] = MOT_STATE_MIDDLE;			//!< update final status

				/**upload status to UI*/
				U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_CTRL2);
				U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_STATUS2);

				U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);
			}
		}
	}
}

static uchar MOT_ucSetMotorState(TMotorIndexDef tMotorIndex)
{
	uchar ucResult = SET_FAIL;

	/**for left right motor*/
	if (MOT_INDEX_LEFT_RIGHT == tMotorIndex)
	{
		if (MOT_tCtrlStateToSetBuffer[tMotorIndex] > MOT_tCtrlStateOfCurrent[tMotorIndex])	//!< target position is in the right side
		{
			/**move to right*/
			SET_IO_HIGH(MOT_SLEEP2_PORT, MOT_SLEEP2_PIN);	//!< sleep ctrl, set high to quit sleep
			SET_IO_LOW(MOT_PHASE2_PORT, MOT_PHASE2_PIN);	//!< direction ctrl
			SET_IO_HIGH(MOT_ENABLE2_PORT, MOT_ENABLE2_PIN);	//!< enbale ctrl

			if (!GET_IO_STATE(MOT_PHASE2_PORT, MOT_PHASE2_PIN) && GET_IO_STATE(MOT_SLEEP2_PORT, MOT_SLEEP2_PIN)
				&& GET_IO_STATE(MOT_ENABLE2_PORT, MOT_ENABLE2_PIN))
			{
				ucResult = SET_OK;
			}
		}
		else
		{
			/**move to left*/
			SET_IO_HIGH(MOT_SLEEP2_PORT, MOT_SLEEP2_PIN);	//!< sleep ctrl, set high to quit sleep
			SET_IO_HIGH(MOT_PHASE2_PORT, MOT_PHASE2_PIN);	//!< direction ctrl
			SET_IO_HIGH(MOT_ENABLE2_PORT, MOT_ENABLE2_PIN);	//!< enbale ctrl

			if (GET_IO_STATE(MOT_PHASE2_PORT, MOT_PHASE2_PIN) && GET_IO_STATE(MOT_SLEEP2_PORT, MOT_SLEEP2_PIN)
				&& GET_IO_STATE(MOT_ENABLE2_PORT, MOT_ENABLE2_PIN))
			{
				ucResult = SET_OK;
			}
		}

		return ucResult;
	}

	/**for UP DOWN motor*/
#if (MOT_DRIVE_MODE_DELAY == MOT_DRIVE_MODE)
	switch (MOT_tCtrlStateToSetBuffer[tMotorIndex])
#elif (MOT_DRIVE_MODE_REAL_TIME == MOT_DRIVE_MODE)
	switch (MOT_tCtrlStateToSet[tMotorIndex])
#endif
	{
		default:
		case MOT_STATE_CLOSE:
		{
			if (MOT_INDEX_UP_DOWN == tMotorIndex)
			{
				SET_IO_HIGH(MOT_SLEEP1_PORT, MOT_SLEEP1_PIN);	//!< sleep ctrl, set high to quit sleep
				SET_IO_HIGH(MOT_PHASE1_PORT, MOT_PHASE1_PIN);	//!< direction ctrl
				SET_IO_HIGH(MOT_ENABLE1_PORT, MOT_ENABLE1_PIN);	//!< enbale ctrl

				if (GET_IO_STATE(MOT_PHASE1_PORT, MOT_PHASE1_PIN) && GET_IO_STATE(MOT_SLEEP1_PORT, MOT_SLEEP1_PIN)
					&& GET_IO_STATE(MOT_ENABLE1_PORT, MOT_ENABLE1_PIN))
				{
					ucResult = SET_OK;
				}
			}

			break;
		}

		case MOT_STATE_OPEN:
		{
			if (MOT_INDEX_UP_DOWN == tMotorIndex)
			{
				SET_IO_HIGH(MOT_SLEEP1_PORT, MOT_SLEEP1_PIN);	//!< sleep ctrl, set high to quit sleep
				SET_IO_LOW(MOT_PHASE1_PORT, MOT_PHASE1_PIN);	//!< direction ctrl
				SET_IO_HIGH(MOT_ENABLE1_PORT, MOT_ENABLE1_PIN);	//!< enbale ctrl

				if (!GET_IO_STATE(MOT_PHASE1_PORT, MOT_PHASE1_PIN) && GET_IO_STATE(MOT_SLEEP1_PORT, MOT_SLEEP1_PIN)
					&& GET_IO_STATE(MOT_ENABLE1_PORT, MOT_ENABLE1_PIN))
				{
					ucResult = SET_OK;
				}
			}

			break;
		}
	}

	return ucResult;
}



static uchar MOT_ucEnterIdleState(TMotorIndexDef tMotorIndex)
{
	uchar ucResult = SET_FAIL;

	if (MOT_INDEX_UP_DOWN == tMotorIndex)
	{
		SET_IO_LOW(MOT_ENABLE1_PORT, MOT_ENABLE1_PIN);	//!< enbale ctrl
		SET_IO_LOW(MOT_PHASE1_PORT, MOT_PHASE1_PIN);	//!< direction ctrl
		SET_IO_LOW(MOT_SLEEP1_PORT, MOT_SLEEP1_PIN);	//!< sleep ctrl, set low to enter sleep

		if (!GET_IO_STATE(MOT_PHASE1_PORT, MOT_PHASE1_PIN) && !GET_IO_STATE(MOT_ENABLE1_PORT, MOT_ENABLE1_PIN)
			&& !GET_IO_STATE(MOT_SLEEP1_PORT, MOT_SLEEP1_PIN))
		{
			ucResult = SET_OK;
		}
	}
	else
	{
		SET_IO_LOW(MOT_ENABLE2_PORT, MOT_ENABLE2_PIN);	//!< enbale ctrl
		SET_IO_LOW(MOT_PHASE2_PORT, MOT_PHASE2_PIN);	//!< direction ctrl
		SET_IO_LOW(MOT_SLEEP2_PORT, MOT_SLEEP2_PIN);	//!< sleep ctrl, set low to enter sleep

		if (!GET_IO_STATE(MOT_PHASE2_PORT, MOT_PHASE2_PIN) && !GET_IO_STATE(MOT_ENABLE2_PORT, MOT_ENABLE2_PIN)
			&& !GET_IO_STATE(MOT_SLEEP2_PORT, MOT_SLEEP2_PIN))
		{
			ucResult = SET_OK;
		}
	}

	return ucResult;
}

static void MOT_vUpdateRunningTime(void)
{
	if (MOT_TASK_RUNNING == MOT_tTaskState[MOT_INDEX_UP_DOWN])
	{
		MOT_uiMotorRunningTime[MOT_INDEX_UP_DOWN]++;
		MOT_uiMotorOffTime[MOT_INDEX_UP_DOWN] = 0;
	}
	else
	{
		MOT_uiMotorRunningTime[MOT_INDEX_UP_DOWN] = 0;
		MOT_uiMotorOffTime[MOT_INDEX_UP_DOWN]++;
	}

	if (MOT_TASK_RUNNING == MOT_tTaskState[MOT_INDEX_LEFT_RIGHT])
	{
		MOT_uiMotorRunningTime[MOT_INDEX_LEFT_RIGHT]++;
		MOT_uiMotorOffTime[MOT_INDEX_LEFT_RIGHT] = 0;
	}
	else
	{
		MOT_uiMotorRunningTime[MOT_INDEX_LEFT_RIGHT] = 0;
		MOT_uiMotorOffTime[MOT_INDEX_LEFT_RIGHT]++;
	}

	/**open time*/
	if ((MOT_tCurrentStatus[MOT_INDEX_UP_DOWN] != MOT_STATE_CLOSE) && (MOT_tCurrentStatus[MOT_INDEX_UP_DOWN] != MOT_STATE_CLOSING))
	{
		MOT_ulOpenTime[MOT_INDEX_UP_DOWN]++;
	}
	else
	{
		MOT_ulOpenTime[MOT_INDEX_UP_DOWN] = 0;
	}
}


void MOT_vTaskHandler(void)
{
	if (TMR_uiTimer[TMR_POWER_ON_INIT] == 0)
	{
		MOT_vUpDownTaskHandler(MOT_INDEX_UP_DOWN);
		MOT_vLeftRightTaskHandler(MOT_INDEX_LEFT_RIGHT);
	}

	MOT_vUpdateRunningTime();
}

void MOT_vSetMotorCtrlState(TMotorIndexDef tMotorIndex, TMotorStateDef tState)
{
	//if (tState <= MOT_STATE_OPEN)
	{
		MOT_tCtrlStateToSet[tMotorIndex] = tState;

		if (MOT_INDEX_UP_DOWN == tMotorIndex)
		{
			MOT_vUpDownTaskHandler(tMotorIndex);
		}
		else
		{
			MOT_vLeftRightTaskHandler(tMotorIndex);
		}
	}
}

TMotorStateDef MOT_tGetMotorCtrlState(TMotorIndexDef tMotorIndex)
{
	return MOT_tCtrlStateOfCurrent[tMotorIndex];	//!< return current value
}

TMotorStateDef MOT_tGetMotorStatus(TMotorIndexDef tMotorIndex)
{
	return MOT_tCurrentStatus[tMotorIndex];	//!< return current value
}

ulong MOT_ulGetOpenTime(TMotorIndexDef tMotorIndex)
{
	return MOT_ulOpenTime[tMotorIndex];	//!< return current value
}