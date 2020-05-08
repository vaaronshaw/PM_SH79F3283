#include "hood.h"

static THoodStateDef HOOD_tWorkingState = (THoodStateDef)0;




void HOOD_vSetWorkingState(THoodStateDef tState)
{
	HOOD_tWorkingState = tState;
}

BOOL HOOD_bIsWorkingState(THoodStateDef tState)
{
	return (tState == HOOD_tWorkingState);
}

THoodStateDef HOOD_tGetWorkingState(void)
{
	return HOOD_tWorkingState;
}