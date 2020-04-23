#ifndef __SWTIMER_H_
#define __SWTIMER_H_

#include "defines.h"


//TMR16 timing array
typedef enum _TSWTimer16Index{
	TMR_BTN_SCAN,
	TMR_U2P_TRANSMIT_TIME_OUT,
	TMR_U2P_RECEIVE_TIME_OUT,
	TMR_U2P_MSG_NO_ACK_TIMEOUT,
	TMR_NUM
}TSWTimerIndex;


/* PUBLIC DECLARATIONS *********************************************************************************************/
#define TMR_TIME_BASE		10	//!<  base time
#define TMR_TIME_MS2TICKS(x)   ((x)/TMR_TIME_BASE)	//!< convert time from millisecond to real time ticks
#define TMR_TIME_SEC2TICKS(x)   ((x)*1000/TMR_TIME_BASE)	//!< convert time from millisecond to real time ticks
#define TMR_TIME_MIN2TICKS(x)   ((x)*60000/TMR_TIME_BASE)	//!< convert time from millisecond to real time ticks



extern volatile uint TMR_uiTimer[TMR_NUM];

extern void TMR_vTimerIsr(void);										//Timer interrupt handler function
// extern BOOL TMR_bIsTimeExpired(TSWTimerIndex tTimerIndex);			//check timer16 item is over or not
// extern void TMR_vSetTime(TSWTimerIndex tTimerIndex, uint uiValue);	//set timer16 time value
// extern uint TMR_ulGetTimerValue(TSWTimerIndex tTimerIndex);			//get timer16 current value































#endif