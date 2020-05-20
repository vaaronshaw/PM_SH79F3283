#include "light.h"
#include "Uart2PM.h"


static TLigStateDef LIG_tStateToSet = (TLigStateDef)0;
static TLigStateDef LIG_tStateOfCurrent = (TLigStateDef)0;

static uchar LIG_ucSetState(TLigStateDef tState);
static ulong LIG_ulLightOnTime = 0;



void LIG_vInit(void)
{
	/**io init
	*P1.1: light
	*/
	P1CR |= 0x02;	//!< output
	P1PCR &= ~0x02;	//!< no pullup
	P1 &= ~0x02;	//!< low
}

static uchar LIG_ucSetState(TLigStateDef tState)
{
	uchar ucResult = 0;

	switch (tState)
	{
		default:
		case LIG_STATE_OFF:
		{
			SET_IO_LOW(LIG_IO_PORT, LIG_IO_PIN);

			if (!GET_IO_STATE(LIG_IO_PORT, LIG_IO_PIN))
			{
				ucResult = 1;
			}
			break;
		}

		case LIG_STATE_ON:
		{
			SET_IO_HIGH(LIG_IO_PORT, LIG_IO_PIN);

			if (GET_IO_STATE(LIG_IO_PORT, LIG_IO_PIN))
			{
				ucResult = 1;
			}
			break;
		}
	}

	return ucResult;
}

void LIG_vTaskHandler(void)
{
	if (LIG_tStateOfCurrent != LIG_tStateToSet)
	{
		if (SET_OK == LIG_ucSetState(LIG_tStateToSet))
		{
			LIG_tStateOfCurrent = LIG_tStateToSet;

			/**transmit status to UI*/
			U2P_vSetPropertyUploadFlag(U2P_PIID_LIGHT);
			U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);
		}
	}
}

void LIG_vSetTargetState(TLigStateDef tState)
{
	LIG_tStateToSet = tState;
}

TLigStateDef LIG_tGetLightStatus(void)
{
	return LIG_tStateOfCurrent;	//!< return current value
}

void LIG_vUpdateWorkingTime(void)
{
	if (LIG_STATE_ON == LIG_tStateOfCurrent)
	{
		LIG_ulLightOnTime++;
	}
	else
	{
		LIG_ulLightOnTime = 0;
	}
}