#ifndef _BUTTON_H
#define _BUTTON_H

#include "defines.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif
#include "intrins.h"




#define BTN_DATA_IS_OVER()		//((TKF0 & 0x40) != 0)
#define BTN_GO_IS_ERR()			//((TKF0 & 0x20) != 0) 
#define BTN_SCAN_IS_END()		//((TKF0 & 0x10) != 0)
#define BTN_COUNT_IS_OVER()		//((TKF0 & 0x08) != 0)


#define BTN_SCAN_START()		//(TKCON1 |= 0x20)


#define BTN_PRESS_TIME_BASE_TICK	50	//!< 50ms
#define BTN_PRESS_TIME_MS2TICKS(x)   ((x)/BTN_PRESS_TIME_BASE_TICK)	//!< convert time from millisecond to real time ticks
#define BTN_PRESS_TIME_SEC2TICKS(x)   ((x)*1000/BTN_PRESS_TIME_BASE_TICK)	//!< convert time from millisecond to real time ticks





typedef enum {
	BTN_TASK_INIT = 0,
	BTN_TASK_RUNNING
}TbuttonTaskDef;

typedef enum {
	BTN_SCAN_IDLE = 0,
	BTN_SCAN_RUNNING
}TscanTaskDef;

enum {
	BTN_RESULT_OK = 1,
	BTN_RESULT_GO_ERR,
	BTN_RESULT_DATA_OVER,
	BTN_RESULT_COUNT_OVER,
	BTN_RESULT_UNKNOW = 0xFF
};

enum {
	BTN_EVENT_SHORT_PRESS = 0x01,
	BTN_EVENT_LONG_PRESS_3S = 0x02
};


typedef struct {
	uchar ucScanResult;
	uchar ucButtonState;
	uchar ucButtonStateLast;
	uchar ucPressEventInvokeFlag;
	uchar ucPressTime;
}TbuttonInfo;



extern uchar BTN_ucGetKeyPressCount(void);
extern BOOL BTN_bIsKeyPressed(uchar ucKeyIndex);
extern BOOL BTN_bIsAnyKeyPressed(void);
extern BOOL BTN_bIsPressEventHandled(uchar ucEvent);
extern void BTN_vSetPressEventHandled(uchar ucEvent);
extern uchar BTN_ucGetPressTimeInTicks(void);

extern void BTN_vTouchInit(void);
extern void BTN_vTaskHandler(void);






















































#endif