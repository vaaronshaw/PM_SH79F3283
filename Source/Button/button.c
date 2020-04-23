#include "button.h"
#include "api_ext.h"
#include "sw_timer.h"



static TbuttonTaskDef BTN_tTaskState = (TbuttonTaskDef)0;
static TscanTaskDef BTN_tScanState = (TscanTaskDef)0;
static TbuttonInfo BTN_tInfo;

static void BTN_vScanTaskHandler(void)
{
	switch (BTN_tScanState)
	{
		default:
		case BTN_SCAN_IDLE:
		{
			if (0 == TMR_uiTimer[TMR_BTN_SCAN])
			{
				TMR_uiTimer[TMR_BTN_SCAN] = TMR_TIME_MS2TICKS(100);	//!< button scan in every 100ms 

				/**start scanning*/
				BTN_SCAN_START();
				BTN_tTaskState = BTN_SCAN_RUNNING;
			}

			break;
		}

		case BTN_SCAN_RUNNING:
		{
			/**result code is not 0, or timeout*/
			if (BTN_tInfo.ucScanResult || (0 == TMR_uiTimer[TMR_BTN_SCAN]))
			{
				BTN_tTaskState = BTN_SCAN_IDLE;

				if (BTN_RESULT_OK == BTN_tInfo.ucScanResult)
				{
					/**read button value from address: 0x500H ~ 0x52FH*/

				}
			}

			break;
		}
	}
}

static void BTN_vPressTimeHandler(void)
{
#define KEY_PRESS_MAX_TIME	(200)	//!< 10s

	/*when button is pressed, check how long the press last*/
	if (0 != BTN_tInfo.ucButtonState)
	{
		if (BTN_tInfo.ucButtonStateLast != BTN_tInfo.ucButtonState)
		{
			BTN_tInfo.ucPressEventInvokeFlag = 0;	//!< clear press event handle status immediately when key event is changed
			BTN_tInfo.ucPressTime = 0;
		}

		if (BTN_tInfo.ucPressTime < KEY_PRESS_MAX_TIME)
		{
			BTN_tInfo.ucPressTime++;
		}
	}
	else
	{
		BTN_tInfo.ucPressTime = 0;
		BTN_tInfo.ucPressEventInvokeFlag = 0;
	}

	BTN_tInfo.ucButtonStateLast = BTN_tInfo.ucButtonState;	//!< update key event
}

uchar BTN_ucGetKeyPressCount(void)
{
	uchar ucCountValue = 0;
	uchar ucIndex;

	for (ucIndex = 0; ucIndex < 8; ucIndex++)
	{
		if (((BIT_0) << ucIndex) & BTN_tInfo.ucButtonState)
		{
			ucCountValue++;
		}
	}

	return ucCountValue;
}

BOOL BTN_bIsKeyPressed(uchar ucKeyIndex)
{
	return (DEF_TEST(BTN_tInfo.ucButtonState, ucKeyIndex) == ucKeyIndex);
}

BOOL BTN_bIsAnyKeyPressed(void)
{
	return (0 != BTN_tInfo.ucButtonState);
}

BOOL BTN_bIsPressEventHandled(uchar ucEvent)
{
	return DEF_TEST(BTN_tInfo.ucPressEventInvokeFlag, ucEvent);
}

void BTN_vSetPressEventHandled(uchar ucEvent)
{
	DEF_SET(BTN_tInfo.ucPressEventInvokeFlag, ucEvent);
}

uchar BTN_ucGetPressTimeInTicks(void)
{
	return BTN_tInfo.ucPressTime;
}

void BTN_vTouchInit(void)
{
#if 0
	/*使能为触摸按键通道*/
	P0SS = 0x7F;	//!< 0: set as I/O; 1: set as touch port
	P1SS = 0xFF;
	P4SS = 0x3E;
	P5SS = 0x0F;

	/*设置扫描该按键通道*/
	TKU1 = 0xFF;	//!< 0: skip this button; 1: scan this button in turns
	TKU2 = 0xFF;
	TKU3 = 0xFF;

	/*设置TKCON1寄存器*/
	TKCON1 = 0x82;	//!< [7]: enable touch button working, sample 6 times

	/*设置TKST寄存器*/
	TKST = 0x0C;	//!< touch on/off switch frequency = Fsys / TKST

	/*设置TKRANDOM寄存器*/
	TKRANDOM = 0x20; //!< 不开防水补偿，不开随机抖动，打开比较器滞回使能

	/*设置TKVREF寄存器*/
	TKVREF = 0xFF;	//!< 基准电压0.6V，去抖时间64clk，充电电压1.2V，放电时间640clk

	/*设置放大系数寄存器（即被除数）*/
	TKDIV01 = 0x00;
	TKDIV02 = 0x00;
	TKDIV03 = 0x90;
	TKDIV04 = 0x01;

	/*软件延时*/
	Delay(); //软件延时

	/*中断使能*/
	IEN0 |= 0x02;
#endif
}



void BTN_vTaskHandler(void)
{
	switch (BTN_tTaskState)
	{
		default:
		case BTN_TASK_INIT:
		{
			BTN_tTaskState = BTN_TASK_RUNNING;

			break;
		}
		
		case BTN_TASK_RUNNING:
		{
			BTN_vScanTaskHandler();
			BTN_vPressTimeHandler();

			break;
		}
	}
}


void BTN_vIsrEventHandler(void)
{
	//if (BTN_SCAN_IS_END())
	//{
	//	BTN_tInfo.ucScanResult = BTN_RESULT_OK;
	//}
	//else if (BTN_DATA_IS_OVER())	
	//{
	//	//!< result overflow, should decrease TKDIV01~04
	//	BTN_tInfo.ucScanResult = BTN_RESULT_DATA_OVER;
	//}
	//else if (BTN_GO_IS_ERR())
	//{
	//	/**scanning start failed, restart next scanning*/
	//	BTN_tInfo.ucScanResult = BTN_RESULT_GO_ERR;
	//}
	//else if (BTN_COUNT_IS_OVER())
	//{
	//	/**scanning counyer overflow, should decrease C1*/
	//	BTN_tInfo.ucScanResult = BTN_RESULT_COUNT_OVER;
	//}
	//else
	//{
	//	BTN_tInfo.ucScanResult = BTN_RESULT_UNKNOW;
	//}


}

