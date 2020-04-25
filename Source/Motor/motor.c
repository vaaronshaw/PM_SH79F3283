#include "motor.h"

static volatile TMotorTaskDef MOT_tTaskState[MOT_INDEX_NUM] = {0, 0};
static volatile uint MOT_uiRawAdcValue[MOT_INDEX_NUM][MOT_AD_SAMPLE_TIMES];
static volatile uint MOT_uiAdValue[MOT_INDEX_NUM] = {0, 0};
static volatile uchar MOT_ucRawAdcValueCount[MOT_INDEX_NUM] = {0, 0};


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
	while ((ADCON & 0x01) || (ADCON & 0x40))	//!< check go/done or ADCIF flag
	{
		ADCON &= ~0x40;	//!< clear flag
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
	switch (MOT_tTaskState[tMotorIndex])
	{
		default:
		case MOT_TASK_INIT:
		{
			MOT_tTaskState[tMotorIndex] = MOT_TASK_IDLE;

			break;
		}
			
		case MOT_TASK_IDLE:
		{
			MOT_vAdSampleTask(tMotorIndex);

			break;
		}

		case MOT_TASK_CLOSE:
		{

			break;
		}

		case MOT_TASK_OPEN:
		{

			break;
		}
			
	}
}


void MOT_vTaskHandler(void)
{
	MOT_vSubTaskHandler(MOT_INDEX_LEFT);
	MOT_vSubTaskHandler(MOT_INDEX_RIGHT);
}