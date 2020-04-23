#include "motor.h"

static TmotorSpeedDef MOT_tSpeedToSet = (TmotorSpeedDef)0;

void MOT_vInit(void)
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

void MOT_vSetSpeed(TmotorSpeedDef tSpeedIndex)
{
	switch (MOT_tSpeedToSet)
	{
		default:
		case MOT_SPEED_0:
		{
			MOT_RELAY1_OFF();
			MOT_RELAY2_OFF();
			MOT_RELAY3_OFF();
			MOT_RELAY4_OFF();
			break;
		}

		case MOT_SPEED_1:
		{
			MOT_RELAY1_ON();
			MOT_RELAY2_OFF();
			MOT_RELAY3_OFF();
			MOT_RELAY4_OFF();
			break;
		}

		case MOT_SPEED_2:
		{
			MOT_RELAY1_OFF();
			MOT_RELAY2_ON();
			MOT_RELAY3_OFF();
			MOT_RELAY4_OFF();
			break;
		}

		case MOT_SPEED_3:
		{
			MOT_RELAY1_OFF();
			MOT_RELAY2_OFF();
			MOT_RELAY3_ON();
			MOT_RELAY4_OFF();
			break;
		}

		case MOT_SPEED_4:
		{
			MOT_RELAY1_OFF();
			MOT_RELAY2_OFF();
			MOT_RELAY3_OFF();
			MOT_RELAY4_ON();
			break;
		}
	}
}