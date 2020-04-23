#include "SH79F328.h"
#include"cpu.h"
#include"LED_define.h"
#define LED_RAM_MAX		28
UCHAR xdata LED_RAM[LED_RAM_MAX] _at_  0x400;
 /*****************************************************************************************************
 *  Function Name: init_led
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:   
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
 void init_led()
 {
	
/******************************************************************************************************
 配置COM口、SEG口(8COM,16SEG)
********************************************************************************************************/
	SEG01 = 0xff;
	SEG02 = 0xff;
	LEDCOM = 0xff; 
	#ifdef LED_MODE1
	LEDCON = 0x0;		//mode1,LEDON
 	#else
	#ifdef LED_MODE2
	//Enable LED Interrupt.
	IEN1 |= 0x02; 	//ELED
	ELEDCON |= 0x02;	//帧中断开启
	LEDCON = 0x20;		//mode2,LEDON
	#endif
	#endif
 }

void display(UCHAR v_LED_RAM)

{
	UCHAR i;
	for(i = 0x0; i <= 0x1b; i++)
	{				   
		LED_RAM[i] = v_LED_RAM;
	}

}