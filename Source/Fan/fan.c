#include "fan.h"

static TFanSpeedDef FAN_tSpeedToSet = (TFanSpeedDef)0;
static TFanSpeedDef FAN_tSpeedOfCurrent = (TFanSpeedDef)0;

static uchar FAN_ucSetSpeed(TFanSpeedDef tSpeedIndex);
static ulong FAN_ulRunningTime = 0;

void FAN_vInit(void)
{
	/**io init
	*P1.3: relay4
	*P1.4: relay3
	*P2.5: relay2
	*P2.6: relay1
	*/
	P1CR |= 0x18;	//!< output
	P1PCR &= ~0x18;	//!< no pullup
	P1 &= ~0x18;	//!< low

	P2CR |= 0x60;	//!< output
	P2PCR &= ~0x60;	//!< no pullup
	P2 &= ~0x60;	//!< low
}

static uchar FAN_ucSetSpeed(TFanSpeedDef tSpeedIndex)
{
	uchar ucResult = SET_FAIL;

	switch (tSpeedIndex)
	{
		default:
		case FAN_SPEED_0:
		{
			SET_IO_LOW(FAN_RELAY1_PORT, FAN_RELAY1_PIN);
			SET_IO_LOW(FAN_RELAY2_PORT, FAN_RELAY2_PIN);
			SET_IO_LOW(FAN_RELAY3_PORT, FAN_RELAY3_PIN);
			SET_IO_LOW(FAN_RELAY4_PORT, FAN_RELAY4_PIN);
			
			/**check io state*/
			if (!GET_IO_STATE(FAN_RELAY1_PORT, FAN_RELAY1_PIN) && !GET_IO_STATE(FAN_RELAY2_PORT, FAN_RELAY2_PIN) 
					&& !GET_IO_STATE(FAN_RELAY3_PORT, FAN_RELAY3_PIN) && !GET_IO_STATE(FAN_RELAY4_PORT, FAN_RELAY4_PIN))
			{
				ucResult = SET_OK;
			}
			break;
		}

		case FAN_SPEED_1:
		{
			SET_IO_HIGH(FAN_RELAY1_PORT, FAN_RELAY1_PIN);
			SET_IO_LOW(FAN_RELAY2_PORT, FAN_RELAY2_PIN);
			SET_IO_LOW(FAN_RELAY3_PORT, FAN_RELAY3_PIN);
			SET_IO_LOW(FAN_RELAY4_PORT, FAN_RELAY4_PIN);

			/**check io state*/
			if (GET_IO_STATE(FAN_RELAY1_PORT, FAN_RELAY1_PIN) && !GET_IO_STATE(FAN_RELAY2_PORT, FAN_RELAY2_PIN)
				&& !GET_IO_STATE(FAN_RELAY3_PORT, FAN_RELAY3_PIN) && !GET_IO_STATE(FAN_RELAY4_PORT, FAN_RELAY4_PIN))
			{
				ucResult = SET_OK;
			}
			break;
		}

		case FAN_SPEED_2:
		{
			SET_IO_LOW(FAN_RELAY1_PORT, FAN_RELAY1_PIN);
			SET_IO_HIGH(FAN_RELAY2_PORT, FAN_RELAY2_PIN);
			SET_IO_LOW(FAN_RELAY3_PORT, FAN_RELAY3_PIN);
			SET_IO_LOW(FAN_RELAY4_PORT, FAN_RELAY4_PIN);

			/**check io state*/
			if (!GET_IO_STATE(FAN_RELAY1_PORT, FAN_RELAY1_PIN) && GET_IO_STATE(FAN_RELAY2_PORT, FAN_RELAY2_PIN)
				&& !GET_IO_STATE(FAN_RELAY3_PORT, FAN_RELAY3_PIN) && !GET_IO_STATE(FAN_RELAY4_PORT, FAN_RELAY4_PIN))
			{
				ucResult = SET_OK;
			}
			break;
		}

		case FAN_SPEED_3:
		{
			SET_IO_LOW(FAN_RELAY1_PORT, FAN_RELAY1_PIN);
			SET_IO_LOW(FAN_RELAY2_PORT, FAN_RELAY2_PIN);
			SET_IO_HIGH(FAN_RELAY3_PORT, FAN_RELAY3_PIN);
			SET_IO_LOW(FAN_RELAY4_PORT, FAN_RELAY4_PIN);

			/**check io state*/
			if (!GET_IO_STATE(FAN_RELAY1_PORT, FAN_RELAY1_PIN) && !GET_IO_STATE(FAN_RELAY2_PORT, FAN_RELAY2_PIN)
				&& GET_IO_STATE(FAN_RELAY3_PORT, FAN_RELAY3_PIN) && !GET_IO_STATE(FAN_RELAY4_PORT, FAN_RELAY4_PIN))
			{
				ucResult = SET_OK;
			}
			break;
		}

		case FAN_SPEED_4:
		{
			SET_IO_LOW(FAN_RELAY1_PORT, FAN_RELAY1_PIN);
			SET_IO_LOW(FAN_RELAY2_PORT, FAN_RELAY2_PIN);
			SET_IO_LOW(FAN_RELAY3_PORT, FAN_RELAY3_PIN);
			SET_IO_HIGH(FAN_RELAY4_PORT, FAN_RELAY4_PIN);

			/**check io state*/
			if (!GET_IO_STATE(FAN_RELAY1_PORT, FAN_RELAY1_PIN) && !GET_IO_STATE(FAN_RELAY2_PORT, FAN_RELAY2_PIN)
				&& !GET_IO_STATE(FAN_RELAY3_PORT, FAN_RELAY3_PIN) && GET_IO_STATE(FAN_RELAY4_PORT, FAN_RELAY4_PIN))
			{
				ucResult = SET_OK;
			}
			break;
		}
	}

	return ucResult;
}

void FAN_vTaskHandler(void)
{
	if (FAN_tSpeedOfCurrent != FAN_tSpeedToSet)
	{
		if (SET_OK == FAN_ucSetSpeed(FAN_tSpeedToSet))
		{
			/**io state set succes, then update current speed*/
			FAN_tSpeedOfCurrent = FAN_tSpeedToSet;
		}
	}
}

/**Call this funtion to set target fan speed to set*/
void FAN_vSetTargetSpeed(TFanSpeedDef tSpeedIndex)
{
	FAN_tSpeedToSet = tSpeedIndex;
}

void FAN_vUpdateWorkingTime(void)
{
	if (FAN_tSpeedOfCurrent > FAN_SPEED_0)
	{	
		if (++FAN_ulRunningTime >= FAN_RUNNING_TIME_MAX)
		{
			/**running time exceeds max time, should shutdown power*/
			FAN_vSetTargetSpeed(FAN_SPEED_0);
		}
	}
	else
	{
		FAN_ulRunningTime = 0;
	}
}