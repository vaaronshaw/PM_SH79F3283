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
 *  Description:   从指定地址处读取1个byte返回
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
UCHAR EEPromByteRead(UCHAR nAddrH,UCHAR nAddrL)  				// 字节读,耗时2个时钟 
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


//	一个扇区256个字节
/*****************************************************************************************************
 *  Function Name: void EEPromByteProgram(UCHAR nAddrH,UCHAR nAddrL, UCHAR nData)   
 *  Created By:    Cathy.feng
 *  Created Date:  2016-07-16
 *  Input:         nAddrH, nAddrL，nData
 *  Output:        None
 *  Return:        
 *  Description:   向指定地址处写入1个byte
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
void EEPromByteProgram(UCHAR nAddrH,UCHAR nAddrL, UCHAR nData)  // 字节编程，如果系统时钟=8M，耗时74 = 0x4a 个时钟
{
	FLASHCON = TRUE ;

	XPAGE = nAddrH ;			 			//0x00开始
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
void EEPromSectorErase(UCHAR nAddrH)// 扇区擦除，如果系统时钟=8M，耗时28000 = 0x6d60 个时钟 
{											// 总用时21ms,21000us/0.75 = 28000
	FLASHCON = TRUE ;
	
	XPAGE = nAddrH ;			 			//0x00开始

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
