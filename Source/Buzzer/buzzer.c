/*~A:Includes*/
#include "buzzer.h"
#include <intrins.h>
#include "sw_timer.h"
#include "api_ext.h"

/*~E*/

/*~A:Definitions and Declarations*/
static TBuzzerInfo BUZZ_tBuzzerInfor;

static uint code BUZZ_uiToneScale[BUZZ_TONE_INDEX_NUM] = {
	0,
	0x0BB8,		//!< 1 Khz
	0x07D0,		//!< 1.5K hz
	0x05DC,		//!< 2 khz
	0x04B0,		//!< 2.5 Khz
};

static TRhythmConfig code BUZZ_tPowerOnRhythm[] = {
	{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(250),	BUZZ_MS2TICK(0)},
};

static TRhythmConfig code BUZZ_tPowerOffRhythm[] = {
	//{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(50),	BUZZ_MS2TICK(200)},
	{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(260),	BUZZ_MS2TICK(0)}
};

static TRhythmConfig code BUZZ_tSelectRhythm[] = {
	{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(250),	BUZZ_MS2TICK(50)}
};

static TRhythmConfig code BUZZ_tInvalidRhythm[] = {
	{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(150),	BUZZ_MS2TICK(50)},
	{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(150),	BUZZ_MS2TICK(80)}
};

static TRhythmConfig code BUZZ_tFaultRhythm[] = {
	{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(130),	BUZZ_MS2TICK(48)},
	{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(130),	BUZZ_MS2TICK(48)},
	{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(130),	BUZZ_MS2TICK(48)}
};

static TRhythmConfig code BUZZ_tLongBeepRhythm[] = {
	{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(1000),	BUZZ_MS2TICK(0)}
};


typedef struct {
	const TRhythmConfig* tRhythmTable;
	uchar ucTableSize;
}TRhythmTable;

static TRhythmTable code BUZZ_tRhythmTable[] = {
	{BUZZ_tPowerOffRhythm,	sizeof(BUZZ_tPowerOffRhythm) / sizeof(BUZZ_tPowerOffRhythm[0])},
	{BUZZ_tSelectRhythm,	sizeof(BUZZ_tSelectRhythm) / sizeof(BUZZ_tSelectRhythm[0])},
	{BUZZ_tInvalidRhythm,	sizeof(BUZZ_tInvalidRhythm) / sizeof(BUZZ_tInvalidRhythm[0])},
	{BUZZ_tFaultRhythm,		sizeof(BUZZ_tFaultRhythm) / sizeof(BUZZ_tFaultRhythm[0])},			//!< fault
	{BUZZ_tPowerOnRhythm,	sizeof(BUZZ_tPowerOnRhythm) / sizeof(BUZZ_tPowerOnRhythm[0])},
	{BUZZ_tLongBeepRhythm,	sizeof(BUZZ_tLongBeepRhythm) / sizeof(BUZZ_tLongBeepRhythm[0])}
};

/*~E*/

/*~A:Static Functions*/
void BUZZ_vInit(void)
{
	/**P0.3 - PWM0*/
	PWMLO = 0x55;	//!< enable register modify

	PWM0C = 0x01;	//!< clock = sys/4 = 3m;
	PWM0PL = 0xDC;	//!< period: 500us = 2khz
	PWM0PH = 0x05;
	PWM0DL = 0xEE;	//!< duty: 250us
	PWM0DH = 0x02;

	//PWMEN = 0x81;	//!< enable output
	PWMEN = 0;

	PWMLO = 0x0;	//!< disable register modify

	/**P0.2 - volume io*/
	P0CR |= 0x04;	//!< output
	P0PCR &= ~0x04;	//!< no pullup
	P0 &= ~0x04;	//!< low
}

/*~E*/

/*~A:Global Functions*/

void BUZZ_vSetBuzzAlarm(TBuzzerRhythmIndex tType)
{
	if (tType < BUZZ_RHYTHM_NUM)
	{
		BUZZ_tBuzzerInfor.tIndex = tType;
		BUZZ_tBuzzerInfor.tTaskState = BUZZ_TASK_IDLE;
	}
}

void BUZZ_vTaskHandler(void)
{
	uchar  ucTemp;
	uint uiTime = 0;
	
	switch (BUZZ_tBuzzerInfor.tTaskState)
	{
		default:
		case BUZZ_TASK_INIT:
		{
			BUZZ_tBuzzerInfor.tTaskState = BUZZ_TASK_IDLE;
			BUZZ_tBuzzerInfor.tIndex = BUZZ_RHYTHM_NUM;		//!< init

			break;
		}

		case BUZZ_TASK_IDLE:
		{
			if (BUZZ_RHYTHM_NUM > BUZZ_tBuzzerInfor.tIndex)
			{
				BUZZ_tBuzzerInfor.tTaskState = BUZZ_TASK_ON;

				ucTemp = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].tRhythmTable[BUZZ_tBuzzerInfor.ucIndexInRhythm].ucToneIndex;

				BUZZ_tBuzzerInfor.ucIndexInRhythm = 0;
				BUZZ_tBuzzerInfor.ucRhythmCounter = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].ucTableSize;

				uiTime = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].tRhythmTable[BUZZ_tBuzzerInfor.ucIndexInRhythm].uiBuzzerOnTime;
				BUZZ_tBuzzerInfor.uiOnTime = uiTime;
				uiTime  = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].tRhythmTable[BUZZ_tBuzzerInfor.ucIndexInRhythm].uiBuzzerOffTime;
				BUZZ_tBuzzerInfor.uiOffTime = uiTime;

				BUZZ_vUpdateFrequency(BUZZ_uiToneScale[ucTemp]);
				BUZZ_OUTPUT_ENABLE();
				BUZZ_CTRL_BEEP_ON();
			}

			break;
		}

		case BUZZ_TASK_ON:
		{
			if (BUZZ_tBuzzerInfor.uiOnTime)
			{
				BUZZ_tBuzzerInfor.uiOnTime--;
			}
			else
			{
				BUZZ_tBuzzerInfor.tTaskState = BUZZ_TASK_OFF;
				BUZZ_CTRL_BEEP_OFF();
				BUZZ_OUTPUT_DISABLE();	//!< ToDo:

			}

			break;
		}

		case BUZZ_TASK_OFF:
		{
			if (BUZZ_tBuzzerInfor.uiOffTime)
			{
				BUZZ_tBuzzerInfor.uiOffTime--;
			}
			else
			{
				if (++BUZZ_tBuzzerInfor.ucIndexInRhythm < BUZZ_tBuzzerInfor.ucRhythmCounter)
				{
					BUZZ_tBuzzerInfor.tTaskState = BUZZ_TASK_ON;
					ucTemp = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].tRhythmTable[BUZZ_tBuzzerInfor.ucIndexInRhythm].ucToneIndex;
					BUZZ_vUpdateFrequency(BUZZ_uiToneScale[ucTemp]);

					BUZZ_tBuzzerInfor.uiOnTime = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].tRhythmTable[BUZZ_tBuzzerInfor.ucIndexInRhythm].uiBuzzerOnTime;
					BUZZ_tBuzzerInfor.uiOffTime = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].tRhythmTable[BUZZ_tBuzzerInfor.ucIndexInRhythm].uiBuzzerOffTime;
					uiTime = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].tRhythmTable[BUZZ_tBuzzerInfor.ucIndexInRhythm].uiBuzzerOnTime;
					BUZZ_tBuzzerInfor.uiOnTime = uiTime;
					uiTime = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].tRhythmTable[BUZZ_tBuzzerInfor.ucIndexInRhythm].uiBuzzerOffTime;
					BUZZ_tBuzzerInfor.uiOffTime = uiTime;

					BUZZ_CTRL_BEEP_ON();
					BUZZ_OUTPUT_ENABLE();	//!< ToDo

				}
				else
				{
					BUZZ_tBuzzerInfor.tTaskState = BUZZ_TASK_IDLE;
					BUZZ_tBuzzerInfor.tIndex = BUZZ_RHYTHM_NUM;
					BUZZ_tBuzzerInfor.ucIndexInRhythm = 0;
					BUZZ_OUTPUT_DISABLE();
					BUZZ_CTRL_BEEP_OFF();
				}
			}

			break;
		}
	}
}

void BUZZ_vUpdateFrequency(uint uiValue)
{
	PWMLO = 0x55;	//!< enable register modify

	PWM0PL = (uchar)(uiValue&0xFF);	//!< period: 500us = 2khz
	PWM0PH = (uchar)(uiValue>>8);
	PWM0DL = (uchar)((uiValue/2) & 0xFF);	//!< duty: 250us
	PWM0DH = (uchar)((uiValue/2) >> 8);
	PWMLO = 0x0;	//!< disable register modify

}

/*~E*/
