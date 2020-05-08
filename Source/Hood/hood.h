#ifndef _HOOD_H
#define _HOOD_H

#include "defines.h"


typedef enum{
	HOOD_STATE_POWER_OFF			= 0,
	HOOD_STATE_POWER_ON				= 1,
	HOOD_STATE_WORKING				= 2,
	HOOD_STATE_DELAY_POWER_OFF		= 3,
	HOOD_STATE_CHILD_LOCK			= 4,
	HOOD_STATE_CLEAN				= 5,
	HOOD_STATE_FACTORY_MODE			= 6
}THoodStateDef;




extern void HOOD_vSetWorkingState(THoodStateDef tState);
extern BOOL HOOD_bIsWorkingState(THoodStateDef tState);
extern THoodStateDef HOOD_tGetWorkingState(void);








































#endif