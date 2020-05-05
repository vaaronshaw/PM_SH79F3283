/*~A:Includes*/
#include "buzzer.h"
#include "intrins.h"
#include "sw_timer.h"
/*~E*/

/*~A:Definitions and Declarations*/
static TBuzzerInfo BUZZ_tBuzzerInfor;

static const uint BUZZ_uiToneScale[BUZZ_TONE_INDEX_NUM] = { 0, 0x01F4, 0x05};

static const TRhythmConfig BUZZ_tPowerOnRhythm[] = {
	{BUZZ_TONE_INDEX_1,		BUZZ_MS2TICK(260),	BUZZ_MS2TICK(0)},
	{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(100),	BUZZ_MS2TICK(0)}
};

static const TRhythmConfig BUZZ_tPowerOffRhythm[] = {
	{BUZZ_TONE_INDEX_2,		BUZZ_MS2TICK(50),	BUZZ_MS2TICK(200)},
	{BUZZ_TONE_INDEX_1,		BUZZ_MS2TICK(260),	BUZZ_MS2TICK(0)}
};

static const TRhythmConfig BUZZ_tSelectRhythm[] = {
	{BUZZ_TONE_INDEX_1,		BUZZ_MS2TICK(250),	BUZZ_MS2TICK(0)}
};

static const TRhythmConfig BUZZ_tInvalidRhythm[] = {
	{BUZZ_TONE_INDEX_1,		BUZZ_MS2TICK(130),	BUZZ_MS2TICK(48)},
	{BUZZ_TONE_INDEX_1,		BUZZ_MS2TICK(130),	BUZZ_MS2TICK(48)}
};

static const TRhythmConfig BUZZ_tFaultRhythm[] = {
	{BUZZ_TONE_INDEX_1,		BUZZ_MS2TICK(130),	BUZZ_MS2TICK(48)},
	{BUZZ_TONE_INDEX_1,		BUZZ_MS2TICK(130),	BUZZ_MS2TICK(48)},
	{BUZZ_TONE_INDEX_1,		BUZZ_MS2TICK(130),	BUZZ_MS2TICK(48)}
};

typedef struct {
	const TRhythmConfig* tRhythmTable;
	uchar ucTableSize;
}TRhythmTable;

static const TRhythmTable BUZZ_tRhythmTable[] = {
	{BUZZ_tPowerOffRhythm,	sizeof(BUZZ_tPowerOffRhythm) / sizeof(BUZZ_tPowerOffRhythm[0])},
	{BUZZ_tSelectRhythm,	sizeof(BUZZ_tSelectRhythm) / sizeof(BUZZ_tSelectRhythm[0])},
	{BUZZ_tInvalidRhythm,	sizeof(BUZZ_tInvalidRhythm) / sizeof(BUZZ_tInvalidRhythm[0])},
	{BUZZ_tFaultRhythm,		sizeof(BUZZ_tFaultRhythm) / sizeof(BUZZ_tFaultRhythm[0])},			//!< fault
	{BUZZ_tPowerOnRhythm,	sizeof(BUZZ_tPowerOnRhythm) / sizeof(BUZZ_tPowerOnRhythm[0])}
};

/*~E*/

/*~A:Static Functions*/
void BUZZ_vInit(void)
{
	/**PWM0*/
	PWMLO = 0x55;	//!< enable register modify
	PWM0C = 0x02;	//!< clock = sys/8 = 1m;

	PWM0PL = 0xF4;	//!< period: 500us = 2khz
	PWM0PH = 0x01;
	PWM0DL = 0xfa;	//!< duty: 250us
	PWM0DH = 0x0;
	//PWMEN = 0x81;	//!< enable output
	PWMEN = 0;

	PWMLO = 0x0;	//!< disable register modify

	/**volume io*/
	P0CR |= 0x04;	//!< output
	P0PCR &= ~0x04;	//!< no pullup
	P0 &= ~0x04;	//!< low
}

/*~E*/

/*~A:Global Functions*/

void BUZZ_vSetBuzzAlarm(TBuzzerRhythmIndex tType)
{
	BUZZ_tBuzzerInfor.tIndex = tType;
	BUZZ_tBuzzerInfor.tTaskState = BUZZ_TASK_IDLE;
}

void BUZZ_vTaskHandler(void)
{
	uchar  ucTemp;

	if (P0 & 0x08)
	{
		P0 &= ~0x08;	//!< low
	}
	else
	{
		P0 |= 0x08;	//!< high
	}

	switch (BUZZ_tBuzzerInfor.tTaskState)
	{
		default:
		case BUZZ_TASK_INIT:
		{
			BUZZ_vInit();
			BUZZ_tBuzzerInfor.tTaskState = BUZZ_TASK_IDLE;
			BUZZ_tBuzzerInfor.tIndex = BUZZ_RHYTHM_NUM;		//!< init

			BUZZ_vSetBuzzAlarm(BUZZ_RHYTHM_POWER_ON);

			break;
		}

		case BUZZ_TASK_IDLE:
		{
			if (BUZZ_RHYTHM_NUM != BUZZ_tBuzzerInfor.tIndex)
			{
				BUZZ_tBuzzerInfor.tTaskState = BUZZ_TASK_ON;

				ucTemp = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].tRhythmTable[BUZZ_tBuzzerInfor.ucIndexInRhythm].ucToneIndex;

				BUZZ_tBuzzerInfor.ucIndexInRhythm = 0;
				BUZZ_tBuzzerInfor.ucRhythmCounter = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].ucTableSize;

				BUZZ_tBuzzerInfor.uiOnTime = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].tRhythmTable[BUZZ_tBuzzerInfor.ucIndexInRhythm].uiBuzzerOnTime;
				BUZZ_tBuzzerInfor.uiOffTime = BUZZ_tRhythmTable[BUZZ_tBuzzerInfor.tIndex].tRhythmTable[BUZZ_tBuzzerInfor.ucIndexInRhythm].uiBuzzerOffTime;
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

					BUZZ_CTRL_BEEP_ON();
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
	//PWM0PL = (uchar)(uiValue&0xFF);	//!< period: 500us = 2khz
	//PWM0PH = (uchar)(uiValue>>8);
	//PWM0DL = (uchar)((uiValue/2) & 0xFF);	//!< duty: 250us
	//PWM0DH = (uchar)((uiValue/2) >> 8);
}

/*~E*/
