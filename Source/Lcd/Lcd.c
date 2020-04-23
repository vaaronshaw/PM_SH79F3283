#include "api_ext.h"
#include "SH79F328.h"
#include"cpu.h"


/*****************************************************************************************************
 *  Function Name: init_lcd
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
void init_lcd()
{
	/*****************************************************************************************************
	1.ѡ��1/4ռ�ձȣ�1/3ƫ�� ,COM5~8����ΪSEG25~28
	2.ƫ�õ���900K
	******************************************************************************************************/
	DISPCON1 = 0;
	DISPCLK0 = 0;	//64Hz

	/******************************************************************************************************
	����COM�ڡ�SEG��(ȫ��ΪLCD��)
	ע���û��ɸ�����Ҫ������Ӧ��Ŀ��COM�ں�SEG��	
	********************************************************************************************************/
	LCDSEG0 = 0xff;
	LCDSEG1 = 0xff;
	LCDSEG2 = 0xff;
	LEDCOM = 0xff;
	/******************************************************************************************************
	�Աȶ�ΪVdd.	
	********************************************************************************************************/
	LCDCON  = 0x8F;
}