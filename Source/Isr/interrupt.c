#include "intrins.h"
//#include "pca_define.h"
//#include "timer_define.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif
#include "api_ext.h"
#include "task.h"
#include "defines.h"
#include "Uart2PM.h"
#include "motor.h"


//UINT16 Val;

/*-------------------------------------------------------
FunctionName  : INT_INT0
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_INT0(void) interrupt 0
//{ 
//	_push_(INSCON);
//    select_bank1();
///*#ifdef MODE3_PWM8
//	if(P0CF&0x01)
//	{
//	  P0CPH0 +=0x30;
//	}	   
//	
//#endif */
//#ifdef MODE0
//	if(P0CF&0x01)
//	{
//	  _nop_();
//	}
//#endif
//#ifdef MODE1
//	if(P0CF&0x01)
//	{
//	 _nop_();
//	}
//#endif
//	//P0CF =0x0; //�������־/��׽�Ƚϱ�־
//
//
//
//
//	_pop_(INSCON);  
//}

/*-------------------------------------------------------
FunctionName  : INT_TouchKey
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_TouchKey(void) interrupt 1
//{ 
//	_push_(INSCON);
//
//
//	//TKF0 = 0x00;	//!< clear isr flag
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_INT1
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_INT1(void) interrupt 2
//{ 
//	_push_(INSCON);
//   
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_TIMER5
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_TIMER5(void) interrupt 3
//{ 
//	_push_(INSCON);
//   	select_bank0();
//	P0_0 = ~P0_0;
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_EUART0
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
void INT_EUART0(void) interrupt 4
{ 
	//uchar ucData = 0;

	_push_(INSCON);
   
	if(1 == TI)
	{
		/**TX isr handler*/
		U2P_vTxEventHandler();
		TI = 0;
	}
	else if(1 == RI)
	{
		/**RX isr handler*/
		//ucData = SBUF;
		U2P_vRxEventHandler();
		RI = 0;
	}

	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_TIMER3
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/

void INT_TIMER3(void) interrupt 11
{ 
	_push_(INSCON);
	
	select_bank1();
	//if (1 == TF3)	
	//{
		TASK_vIRQHandler();
		TF3 = 0;	//!< sofware or hardware clear, decide by OP_TF3
	//}
	select_bank0();
	_pop_(INSCON);
}

/*-------------------------------------------------------
FunctionName  : INT_ADC
Description   : 
Input         : ��
OutPut        : ��
Author        : jet
Date          : 2014.11.21
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//UINT16 Adc_Val;
//void INT_ADC(void) interrupt 6
//{ 
//	_push_(INSCON);
//    select_bank0();
//
//	ADCON1 &=0xbf; 
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_TWI
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_TWI(void) interrupt 7
//{ 
//	_push_(INSCON);
//   
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_LED
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_LED(void) interrupt 8
//{ 
//	_push_(INSCON);
//	select_bank0();
//   
//   	IEN1 &=0xFD;
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_TIMER4
Description   : 
Input         : ��
OutPut        : ��
Author        : jet
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_TIMER4(void) interrupt 9
//{ 
//    //P0_0 = ~P0_0;
//}

/*-------------------------------------------------------
FunctionName  : INT_INT4
Description   : 
Input         : ��
OutPut        : ��
Author        : jet
Date          : 2014.11.21
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
void INT_INT4(void) interrupt 10
{ 
	//uchar ucValue = 0;

	_push_(INSCON);
   
	if (EXF1 & 0x40)	//!< IF46
	{
		EXF1 &= ~ 0x40;
		//ucValue = GET_IO_STATE(MOT_SWITCH_PORT, MOT_SWITCH_PIN);
		MOT_vFinishPosition();
	}


	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_INT3
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_INT3(void) interrupt 11
//{ 
//	_push_(INSCON);
//   
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_INT2
Description   : 
Input         : ��
OutPut        : ��
Author        : jet
Date          : 2014.11.21
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_INT2(void) interrupt 12
//{
//
//}

/*-------------------------------------------------------
FunctionName  : INT_LPD
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
//--------------------------------------------------------*/
//void INT_LPD(void) interrupt 13
//{ 
//	_push_(INSCON);
//   	select_bank0();
//	LPDCON &= 0xef;
//	P0_0 = ~P0_0;
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_SCM
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_SCM(void) interrupt 14
//{ 
//	_push_(INSCON);
//   
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_EUART1
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_EUART1(void) interrupt 15
//{ 
//	_push_(INSCON);
//   
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_EUART2
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_EUART2(void) interrupt 16
//{ 
//	_push_(INSCON);
//   
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_CRC
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
//--------------------------------------------------------*/
//void INT_CRC(void) interrupt 17
//{ 
//	_push_(INSCON);
//   	select_bank0();
//	CRCCON &= 0xbf;
//	CRCCON |= 0x80;
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_PWM0
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_PWM0(void) interrupt 18
//{ 
//	_push_(INSCON);
//   
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_SPI
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_SPI(void) interrupt 19
//{ 
//	_push_(INSCON);
//   
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_PCA0
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_PCA0(void) interrupt 20
//{ 
//	_push_(INSCON);
//   	select_bank1();
//	//P0CF = 0x0;
//   
//	_pop_(INSCON);       
//}

/*-------------------------------------------------------
FunctionName  : INT_PWM1
Description   : 
Input         : ��
OutPut        : ��
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : ע������޸ļ�¼�������汾��
--------------------------------------------------------*/
//void INT_PWM1(void) interrupt 21
//{ 
//	_push_(INSCON);
//   
//   
//	_pop_(INSCON);       
//}
