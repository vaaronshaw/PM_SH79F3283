#include "eeprom_user.h"
#include "intrins.h"







/*****************************************************************************************************
 *  Function Name: void EEP_vReadBytes(uchar ucSectorIndex, uchar ucAddr, uchar* pucData, uchar ucDataLen)  				
 *	Description:	// �ֽڶ�,��ʱ2��ʱ�� 
 *  Created By:    Xiao Chengzhi
 *  Created Date:  2020-05-06
 *  Para:
 *  Return:
 *****************************************************************************************************/
void EEP_vReadBytes(uchar ucSectorIndex, uchar ucAddr, uchar* pucData, uchar ucDataLen)  				
{
	uint uiTemp;
	//uchar code* ucTargetAddr;

	EA = 0;
	FLASHCON = TRUE;	//!< access to EEPROM

	while (ucDataLen--)
	{
		uiTemp = (ucSectorIndex<<8) | ucAddr++;
		*pucData++ = *((uchar code *)uiTemp);
	}

	FLASHCON = FALSE;	//!< access to Main Block
	EA = 1;
}


/*****************************************************************************************************
 *  Function Name: void EEP_vSectorErase(uchar ucSectorIndex)
 *	Description:	// �ֽڱ�̣����ϵͳʱ��=8M����ʱ74 = 0x4a ��ʱ��
 *  Created By:    Xiao Chengzhi
 *  Created Date:  2020-05-06
 *  Para:         
 *  Return:
 *****************************************************************************************************/
void EEP_vProgramBytes(uchar ucSectorIndex, uchar ucAddr, uchar* pucData, uchar ucDataLen)
{
	EA = 0;
	FLASHCON = TRUE;

	while (ucDataLen--)
	{
		XPAGE = ucSectorIndex;			 			//0x00��ʼ
		IB_OFFSET = ucAddr++;
		IB_DATA = *pucData++;

		IB_CON1 = 0x6E;	//!< program
		IB_CON2 = 0x05;	//!< must be this value, if not program will be terminated
		IB_CON3 = 0x0A;	//!< must be this value, if not program will be terminated
		IB_CON4 = 0x09;	//!< must be this value, if not program will be terminated
		IB_CON5 = 0x06;	//!< must be this value, if not program will be terminated
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}

	XPAGE = 0;
	FLASHCON = FALSE;
	EA = 1;
}
/*****************************************************************************************************
 *  Function Name: void EEP_vSectorErase(uchar ucSectorIndex)
 *	Description:	�������������ϵͳʱ��=8M����ʱ28000 = 0x6d60 ��ʱ�� 
 *					����ʱ21ms,21000us/0.75 = 28000
 *  Created By:    Xiao Chengzhi
 *  Created Date:  2020-05-06
 *  Para:         ucSectorIndex
 *  Return:
 *****************************************************************************************************/
void EEP_vSectorErase(uchar ucSectorIndex)
{											
	if (ucSectorIndex >= EEP_SECTOR_NUM)
	{
		/**check sector number validation*/
		return;
	}

	EA = 0;
	FLASHCON = TRUE;	//!< access to EEPROM

	XPAGE = ucSectorIndex;			 	
	IB_CON1 = 0xE6;	//!< sector erase
	IB_CON2 = 0x05;	//!< must be this value, if not program will be terminated
	IB_CON3 = 0x0A;	//!< must be this value, if not program will be terminated
	IB_CON4 = 0x09;	//!< must be this value, if not program will be terminated
	IB_CON5 = 0x06;	//!< must be this value, if not program will be terminated
	_nop_();
	_nop_();
	_nop_();
	_nop_();

	XPAGE = 0;
	FLASHCON = FALSE;	//!< access to Main Block
	EA = 1;
}