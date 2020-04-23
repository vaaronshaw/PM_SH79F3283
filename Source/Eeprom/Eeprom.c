#include "SH79F328.h"
#include "intrins.h"
#include "cpu.h"

/*****************************************************************************************************
 *  Function Name: UCHAR EEPromByteRead(UCHAR nAddrH,UCHAR nAddrL)  
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-16
 *  Input:         nAddrH, nAddrL
 *  Output:        None
 *  Return:        nTemp
 *  Description:   ��ָ����ַ����ȡ1��byte����
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
UCHAR EEPromByteRead(UCHAR nAddrH,UCHAR nAddrL)  				// �ֽڶ�,��ʱ2��ʱ�� 
{
	UCHAR nTemp ;
	UINT16 nInt ;
	UCHAR code * nAddr ;

	FLASHCON = TRUE ;
	nInt = nAddrH ;
	nInt <<= 8 ;
	nInt += nAddrL ;
	nAddr = nInt ;

	nTemp = * nAddr ;
	FLASHCON = FALSE ;

	return (nTemp) ;
}


//	һ������256���ֽ�
/*****************************************************************************************************
 *  Function Name: void EEPromByteProgram(UCHAR nAddrH,UCHAR nAddrL, UCHAR nData)   
 *  Created By:    Cathy.feng
 *  Created Date:  2016-07-16
 *  Input:         nAddrH, nAddrL��nData
 *  Output:        None
 *  Return:        
 *  Description:   ��ָ����ַ��д��1��byte
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
void EEPromByteProgram(UCHAR nAddrH,UCHAR nAddrL, UCHAR nData)  // �ֽڱ�̣����ϵͳʱ��=8M����ʱ74 = 0x4a ��ʱ��
{
	FLASHCON = TRUE ;

	XPAGE = nAddrH ;			 			//0x00��ʼ
	IB_OFFSET = nAddrL ;
	IB_DATA = nData ;

	IB_CON1	 = 0x6E ;
	IB_CON2	 = 0x05 ;
	IB_CON3	 = 0x0A ;
	IB_CON4	 = 0x09 ;
	IB_CON5	 = 0x06 ;
	_nop_() ;				
	_nop_() ;
	_nop_() ;
	_nop_() ;	

	FLASHCON = FALSE ;
}
/*****************************************************************************************************
 *  Function Name: void EEPromSectorErase(UCHAR nAddrH)   
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         nAddrH
 *  Output:        None
 *  Return:        
 *  Description:   
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
void EEPromSectorErase(UCHAR nAddrH)// �������������ϵͳʱ��=8M����ʱ28000 = 0x6d60 ��ʱ�� 
{											// ����ʱ21ms,21000us/0.75 = 28000
	FLASHCON = TRUE ;
	
	XPAGE = nAddrH ;			 			//0x00��ʼ

	IB_CON1	 = 0xE6 ;	
	IB_CON2	 = 0x05 ;
	IB_CON3	 = 0x0A ;
	IB_CON4	 = 0x09 ;
	IB_CON5	 = 0x06 ;
	_nop_() ;				
	_nop_() ;
	_nop_() ;
	_nop_() ;	

	FLASHCON = FALSE ;
}
