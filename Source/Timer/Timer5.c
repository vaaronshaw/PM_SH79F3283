#include "api_ext.h"
#include"cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif
#include"timer_define.h"
#include "intrins.h"
/*****************************************************************************************************
 *  Function Name: init_timer5
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-16
 *  Input:         None
 *  Output:        None
 *  Description:   ��ʼ����ʱ����������Ӧ��ģʽ��
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
void init_timer5()
{
  IEN0 |= 0x88;     //EA, ET5
  _push_(INSCON);
  Select_Bank1();
  //���ö�ʱ���ĳ�ֵΪ0xff00,���ʱ��Ϊ0x100��ϵͳʱ��
  TL5 = 0x0;		 
  TH5 = 0xff;
  T5CON |=0x02;  //������ʱ��5
  _pop_(INSCON);
}