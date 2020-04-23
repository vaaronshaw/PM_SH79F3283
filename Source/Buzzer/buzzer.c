/*~A:Includes*/
#include "buzzer.h"
#include "intrins.h"
#include "sw_timer.h"
/*~E*/

/*~A:Definitions and Declarations*/
static TBuzzerInfo BUZZ_tBuzzerInfor;

static const uint BUZZ_uiToneScale[BUZZ_TONE_INDEX_NUM] = { 0, 0x03, 0x05};

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


typedef struct {
	const TRhythmConfig* tRhythmTable;
	uchar ucTableSize;
}TRhythmTable;

static const TRhythmTable BUZZ_tRhythmTable[BUZZ_RHYTHM_NUM] = {
	{BUZZ_tPowerOnRhythm,	sizeof(BUZZ_tPowerOnRhythm) / sizeof(BUZZ_tPowerOnRhythm[0])},
	{BUZZ_tPowerOffRhythm,	sizeof(BUZZ_tPowerOffRhythm) / sizeof(BUZZ_tPowerOffRhythm[0])},
	{BUZZ_tSelectRhythm,	sizeof(BUZZ_tSelectRhythm) / sizeof(BUZZ_tSelectRhythm[0])},
	{BUZZ_tInvalidRhythm,	sizeof(BUZZ_tInvalidRhythm) / sizeof(BUZZ_tInvalidRhythm[0])}
};

/*~E*/

/*~A:Static Functions*/
static void BUZZ_vInit(void)
{

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

	switch (BUZZ_tBuzzerInfor.tTaskState)
	{
		default:
		case BUZZ_TASK_INIT:
		{
			//BUZZ_vInit();
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
				//BUZZ_OUTPUT_ENABLE();
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
					//BUZZ_OUTPUT_DISABLE();
					BUZZ_CTRL_BEEP_OFF();
				}
			}

			break;
		}
	}
}

void BUZZ_vUpdateFrequency(uint uiValue)
{
	BUZCON = (uchar)uiValue;
	//uint16_t vData_U16;
	//uint32_t vData_U32;

	//vData_U32 = 1000000;	//!< 1M

	//if (uiValue < 16)
	//{
	//	vData_U32 /= 16;
	//}
	//else
	//{
	//	vData_U32 /= uiValue;
	//}

	//vData_U32 *= 2;	//!< to match the real frequency table?

	//vData_U16 = (65535 - (uint16_t)vData_U32 + 1);
	//Tim3_M0_ARRSet(vData_U16);                             //ÉèÖÃÖØÔØÖµ
}

/*~E*/

//void init_pwm0()
//{
//	PWM0PL = 0xff;
//	PWM0PH = 0xf;
//	PWM0DL = 0xff;
//	PWM0DH = 0x7;
//	PWM0CON = 0x81;
//
//}
//
//void init_pwm1()
//{
//	_push_(INSCON);
//	Select_Bank1();
//	PWM1PL = 0xff;
//	PWM1PH = 0xf;
//	PWM1DL = 0xff;
//	PWM1DH = 0x7;
//	PWM1CON = 0xc1;
//	_pop_(INSCON);
//}