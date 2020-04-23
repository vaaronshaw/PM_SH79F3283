#include "api_ext.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif
#include "timer_define.h"
#include "intrins.h"
/*****************************************************************************************************
 *  Function Name: init_timer4
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-16
 *  Input:         None
 *  Output:        None
 *  Description:   
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
void init_timer4()
{
  IEN0 |= 0x80;     //EA
  IEN1 |= 0x04;		//ET4,�ж�ʹ��
  _push_(INSCON);
  Select_Bank1();
  #ifdef TIMER4_OUT
  T4CON = 0x40;	  //T4�������
  //���ö�ʱ���ĳ�ֵΪ0x0,���ʱ��Ϊ0x8000��ϵͳʱ��
  TL4 = 0x0;		 
  TH4 = 0x80;

  #endif
  #ifdef TIMER4_EDGETRIG
  T4CON = 0x48;	//T4�����ش������ҿ��Զ�δ���
  TL4 = 0x0;
  TH4 = 0x0;
  #endif
  T4CON |=0x02;  //������ʱ��4
  _pop_(INSCON);
}