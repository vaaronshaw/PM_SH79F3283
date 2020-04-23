#include "sw_timer.h"


//Declaration of the module locale (static) variables
volatile uint TMR_uiTimer[TMR_NUM]  = {0};	
static uchar TMR_ucT2LanchFlag = 0;
static uchar TMR_ucT1LanchFlag = 0;


/* Timer interrupt handler function
 * time base = 10ms
 * */
void TMR_vTimerIsr(void)
{
	unsigned char ucIndex;

	for(ucIndex=0; ucIndex<TMR_NUM; ucIndex++)
	{
		if(TMR_uiTimer[ucIndex])	//counter > 0
		{
			TMR_uiTimer[ucIndex]--;
		}
	}
}

//check timer16 item is over or not
// BOOL TMR_bIsTimeExpired(TSWTimerIndex tTimerIndex)
// {
// 	return (0 == TMR_uiTimer[tTimerIndex]);
// }

// //set timer16 time value
// void TMR_vSetTime(TSWTimerIndex tTimerIndex, uint uiValue)
// {
// 	TMR_uiTimer[tTimerIndex] = uiValue;
// }

// //get timer16 current value
// ulong TMR_ulGetTimerValue(TSWTimerIndex tTimerIndex)
// {
// 	return TMR_uiTimer[tTimerIndex];
// }
