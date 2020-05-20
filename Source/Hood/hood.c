#include "hood.h"
#include "Uart2PM.h"


static THoodStateDef HOOD_tWorkingState = (THoodStateDef)0;




void HOOD_vSetWorkingState(THoodStateDef tState)
{
	HOOD_tWorkingState = tState;

	/**ToDo: transmit status to UI*/
	U2P_vSetPropertyUploadFlag(U2P_PIID_MACHINE_STATE);
	U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);
}

BOOL HOOD_bIsWorkingState(THoodStateDef tState)
{
	return (tState == HOOD_tWorkingState);
}

THoodStateDef HOOD_tGetWorkingState(void)
{
	return HOOD_tWorkingState;
}