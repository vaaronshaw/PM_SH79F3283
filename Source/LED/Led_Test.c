#include"cpu.h"
#include "SH79F328.h"
#include"api_ext.h"
#include "LED_define.h"
extern  void display(UCHAR v);
/*****************************************************************************************************
 *  Function Name: void Led_test()
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
void Led_Test()
{
	UCHAR v_dis,v_dis_top, dis_cnt = 0,dis_cnt_max = 0x10;
	init_led();
	v_dis = 0xff;
	v_dis_top = 0x0;	//��¼��һ�η�ת��ֵ
	display(v_dis);
	LEDCON |= 0x80;
	while(1)
	{
		#ifdef LED_MODE2
		while(!(LEDCON & 0x18));	//wait frame interrupt.
		LEDCON &= 0xe7;	//clear flag
		if(dis_cnt_max == dis_cnt)
		{
			dis_cnt = 0x0;
			if((0xff == v_dis) || (0x0 == v_dis))	//��ʾ���Ｋֵ
			{
				v_dis_top = v_dis;
			}
			if(0x0 == v_dis_top)
			{
				v_dis ++;
				dis_cnt_max = 0x20;
			}
			else 	//v_dis_top == 0xff
			{
				v_dis --;
				dis_cnt_max = 0x10;
			}

		}
		else
		{
			dis_cnt ++;
		}
		
		display(v_dis);
		LEDCON |= 0x80;
		#endif
	}
}

