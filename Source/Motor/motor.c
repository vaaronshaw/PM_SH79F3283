#include "motor.h"
#include "Uart2PM.h"


static volatile TMotorTaskDef MOT_tTaskState[MOT_INDEX_NUM] = {0, 0};
static volatile uint MOT_uiRawAdcValue[MOT_INDEX_NUM][MOT_AD_SAMPLE_TIMES];
static volatile uint MOT_uiAdValue[MOT_INDEX_NUM] = {0, 0};
static volatile uchar MOT_ucRawAdcValueCount[MOT_INDEX_NUM] = {0, 0};

static TMotorStateDef MOT_tCtrlStateToSet[MOT_INDEX_NUM] = {0 , 0};
static TMotorStateDef MOT_tCtrlStateOfCurrent[MOT_INDEX_NUM] = {0 , 0};
static TMotorStateDef MOT_tCurrentStatus[MOT_INDEX_NUM] = { 0 , 0 };

static uchar MOT_ucMotorRunningTime[MOT_INDEX_NUM] = {0, 0};


static uchar MOT_ucSetMotorState(TMotorIndexDef tMotorIndex);
static uchar MOT_ucEnterIdleState(TMotorIndexDef tMotorIndex);


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

	/**P1.2: SWITCH*/
	P1CR &= ~(MOT_SWITCH_PIN);	//!< input
	P1PCR &= ~(MOT_SWITCH_PIN);	//!< no pull up

	/**adc init for input voltage detection*/
	ADCON = 0x84;	//!< ADON=1, VDD as vref,  ADC channel = AN4, GO/DONE = 0
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
	if (MOT_INDEX_LEFT == tMotorIndex)
	{
		/**AN4*/
		ADCON = 0x84;
		ADCON1 = 0x00;
	}
	else
	{
		/**AN7*/
		ADCON = 0x86;
		ADCON1 = 0x01;
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

static void MOT_vSubTaskHandler(TMotorIndexDef tMotorIndex)
{
#define MOT_TASK_BASE_TICKS		50	//!< 50ms
#define MOT_RUNNING_TIME_MAX	200	//!< 200*50 = 10s??


	switch (MOT_tTaskState[tMotorIndex])
	{
		default:
		case MOT_TASK_INIT:
		{
			MOT_tTaskState[tMotorIndex] = MOT_TASK_IDLE;
			(void)MOT_ucEnterIdleState(tMotorIndex);

			MOT_tCtrlStateToSet[tMotorIndex] = MOT_STATE_CLOSE;		//!< test
			MOT_tCtrlStateOfCurrent[tMotorIndex] = MOT_STATE_OPEN;	//!< test
			MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_OPEN;	//!< init

			break;
		}
			
		case MOT_TASK_IDLE:
		{
			if (MOT_tCtrlStateOfCurrent[tMotorIndex] != MOT_tCtrlStateToSet[tMotorIndex])
			{
				if (SET_OK == MOT_ucSetMotorState(tMotorIndex))
				{
					MOT_tTaskState[tMotorIndex] = MOT_TASK_RUNNING;

					if (MOT_STATE_OPEN == MOT_tCtrlStateToSet[tMotorIndex])
					{
						MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_OPENING;
					}
					else
					{
						MOT_tCurrentStatus[tMotorIndex] = MOT_STATE_CLOSING;
					}

					/**upload status to UI*/
					if (MOT_INDEX_LEFT == tMotorIndex)
					{
						U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_STATUS);
					}
					else
					{
						U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_STATUS2);
					}

					U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);
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
			if ((MOT_uiAdValue[tMotorIndex] > MOT_BLOCK_AD_VALUE_MAX) || (MOT_ucMotorRunningTime[tMotorIndex] >= MOT_RUNNING_TIME_MAX))	//!< ToDo: check it
			{
				if (SET_OK == MOT_ucEnterIdleState(tMotorIndex))
				{
					MOT_tTaskState[tMotorIndex] = MOT_TASK_IDLE;
					MOT_tCtrlStateOfCurrent[tMotorIndex] = MOT_tCtrlStateToSet[tMotorIndex];
					MOT_tCurrentStatus[tMotorIndex] = MOT_tCtrlStateToSet[tMotorIndex];	//!< update final status

					/**upload status to UI*/
					if (MOT_INDEX_LEFT == tMotorIndex)
					{
						U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_CTRL);
						U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_STATUS);
					}
					else
					{
						U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_CTRL2);
						U2P_vSetPropertyUploadFlag(U2P_PIID_DOOR_STATUS2);
					}

					U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);
				}
			}

			break;
		}		
	}
}

static uchar MOT_ucSetMotorState(TMotorIndexDef tMotorIndex)
{
	uchar ucResult = SET_FAIL;

	switch (MOT_tCtrlStateToSet[tMotorIndex])
	{
		default:
		case MOT_STATE_CLOSE:
		{
			if (MOT_INDEX_LEFT == tMotorIndex)
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
			else
			{
				SET_IO_HIGH(MOT_SLEEP2_PORT, MOT_SLEEP2_PIN);	//!< sleep ctrl, set high to quit sleep
				SET_IO_HIGH(MOT_PHASE2_PORT, MOT_PHASE2_PIN);	//!< direction ctrl
				SET_IO_HIGH(MOT_ENABLE2_PORT, MOT_ENABLE2_PIN);	//!< enbale ctrl

				if (GET_IO_STATE(MOT_PHASE2_PORT, MOT_PHASE2_PIN) && GET_IO_STATE(MOT_SLEEP2_PORT, MOT_SLEEP2_PIN)
					&& GET_IO_STATE(MOT_ENABLE2_PORT, MOT_ENABLE2_PIN))
				{
					ucResult = SET_OK;
				}
			}

			break;
		}

		case MOT_STATE_OPEN:
		{
			if (MOT_INDEX_LEFT == tMotorIndex)
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
			else
			{
				SET_IO_HIGH(MOT_SLEEP2_PORT, MOT_SLEEP2_PIN);	//!< sleep ctrl, set high to quit sleep
				SET_IO_LOW(MOT_PHASE2_PORT, MOT_PHASE2_PIN);	//!< direction ctrl
				SET_IO_HIGH(MOT_ENABLE2_PORT, MOT_ENABLE2_PIN);	//!< enbale ctrl

				if (!GET_IO_STATE(MOT_PHASE2_PORT, MOT_PHASE2_PIN) && GET_IO_STATE(MOT_SLEEP2_PORT, MOT_SLEEP2_PIN)
					&& GET_IO_STATE(MOT_ENABLE2_PORT, MOT_ENABLE2_PIN))
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

	if (MOT_INDEX_LEFT == tMotorIndex)
	{
		SET_IO_LOW(MOT_SLEEP1_PORT, MOT_SLEEP1_PIN);	//!< sleep ctrl, set low to enter sleep
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
	if (MOT_TASK_RUNNING == MOT_tTaskState[MOT_INDEX_LEFT])
	{
		MOT_ucMotorRunningTime[MOT_INDEX_LEFT]++;
	}
	else
	{
		MOT_ucMotorRunningTime[MOT_INDEX_LEFT] = 0;
	}

	if (MOT_TASK_RUNNING == MOT_tTaskState[MOT_INDEX_RIGHT])
	{
		MOT_ucMotorRunningTime[MOT_INDEX_RIGHT]++;
	}
	else
	{
		MOT_ucMotorRunningTime[MOT_INDEX_RIGHT] = 0;
	}
}


void MOT_vTaskHandler(void)
{
	MOT_vSubTaskHandler(MOT_INDEX_LEFT);
	MOT_vSubTaskHandler(MOT_INDEX_RIGHT);
}

void MOT_vSetMotorCtrlState(TMotorIndexDef tMotorIndex, TMotorStateDef tState)
{
	MOT_tCtrlStateToSet[tMotorIndex] = tState;
}

TMotorStateDef MOT_tGetMotorCtrlState(TMotorIndexDef tMotorIndex)
{
	return MOT_tCtrlStateOfCurrent[tMotorIndex];	//!< return current value
}

TMotorStateDef MOT_tGetMotorStatus(TMotorIndexDef tMotorIndex)
{
	return MOT_tCurrentStatus[tMotorIndex];	//!< return current value
}