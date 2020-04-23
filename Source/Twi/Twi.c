#include "intrins.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif
#include "Twi_define.h"
#include "api_ext.h"
void TwiInit()
{
  CLKCON |=0x08;
  Delay();
  CLKCON |=0x04;
  TWITOUT = 0x02;		 //TURN ON RESISTOR OF SCL,SDA
  TWIBR=0x02;		 //���÷��Ͳ����ʣ���ֹ���߳�ʱ�ж�	   f=fsys/(16+2*CR*TWIBR)
  TWISTA=0x0;		  //64��Ƶ
  TWICON = 0x40;	//ENTWI	����ֹ�ߵ�ƽ��ʱ
  TWTFREE = 0xff;	//���ʱ����

}
//TWI Master
/*****************************************************************************************************
 *  Function Name: M_TwiSendStart()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         
 *  Return:         FAIL:������ʼ���������г���  OK:������ʼ�����ɹ�
 *  Description:  TWI��Ϊ�������Ϳ�ʼ������
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
bit M_TwiSendStart()
{
  TWICON |=0x20;     //���߿���ʱ������ʼ����
  //TWICON &= 0xdf;
  while(1)  //�ȴ�TWI�ж�
  {

	 if(TWICON&0x08)
	 {
	   if(((TWISTA&0xF8)==0x08)||((TWISTA&0xF8)==0x10))
	   {
		 return OK;		 //�ɹ����Ϳ�ʼ�������ظ���ʼ����
	   }
	   else
	   {
		 return FAIL;
	   }
	 }
	 if(0x02 == (0x02 & TWICON))
	 {
	     TWICON &= 0xdf;
		 return FAIL;       //�ȴ��жϳ�ʱ
	 }
  }

}
/*****************************************************************************************************
 *  Function Name: M_TwiSendCmd(UCHAR addr,UCHAR cmd)
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         addr���ӻ���ַ,cmd������(0:д��0xFF:��)
 *  Return:         ACK
                    NAK���ӻ�û����Ӧ��ǰ��ַ�������Ӧ���ͣ��ظ���ʼ����/��ֹ����
					LOSEARBITRATION����������SLA+R/Wʱ��ʧȥ�ٲã�����TWINT�����߻ᱻ�ͷţ������Ѱַ�ӻ�ģʽ	��
					RCVCMDWRITE����Ϊ��������SLA+R/Wʧȥ�ٲú��յ��������͵�SLA+W���ѻ�ACK
					RCVCMDREAD����Ϊ��������SLA+R/Wʧȥ�ٲú��յ��������͵�SLA+R���ѻ�ACK
					RCVADDRESS0����Ϊ��������SLA+R/Wʧȥ�ٲú��յ��������͵�ͨ�õ�ַ�� �ѻ�ACK
					ERR��		 ��������SLA+R/Wʱ��������������
 *  Description:  Twi ��Ϊ�������ʹӻ���ַ+��/д
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 


UCHAR M_TwiSendCmd(UCHAR addr,UCHAR cmd)
{
	UCHAR SlaveAddr;
	UCHAR i=0;
	if(cmd)
	{
	 	SlaveAddr=((addr&0x7f)<<1)|0x01;         //��

	}
	else
	{
		SlaveAddr=((addr&0x7f)<<1);               //д
	}
	TWICON &= 0xf7;		//����жϱ�־
	TWIDAT=SlaveAddr;
	while(1)  //�ȴ�TWI�ж�
	{
		i++;
		if(TWICON&0x08)
		{
		   if(cmd)
		   {
			   if((TWISTA&0xF8)==0x40)
			   {
				 return ACK;		 //�ɹ�����SLA+R
			   }
			   else	if((TWISTA&0xF8)==0x48)
			   {
				 return NAK;
			   }
		   }
		   else
		   {
			   if((TWISTA&0xF8)==0x18)
			   {
				 return ACK;		 //�ɹ�����SLA+W
			   }
			   else	if((TWISTA&0xF8)==0x20)
			   {
				 return NAK;
			   }
			   
		   }
		   if((TWISTA&0xF8)==0x38)
		   {
			   return LOSEARBITRATION;
		   }
		   else	if((TWISTA&0xF8)==0x68)
		   {
			   return RCVCMDWRITE;
		   }
		   else	if((TWISTA&0xF8)==0x78)
		   {
			   return RCVCMDREAD;
		   }
		   else	if((TWISTA&0xF8)==0xB0)
		   {
			   return RCVADDRESS0;
		   }
		   else
		   {
			   return ERR;
		   }
		}
		if(i>=250)
		{
		     return ERR;       //�ȴ��жϳ�ʱ
		}
	}

}
/*****************************************************************************************************
 *  Function Name: M_TwiSendData(UCHAR byte)
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         byte
 *  Return:         ACK���ӻ���ӦACK ����ʾ�ӻ��пɼ�����������
					NAK���ӻ���ӦNAK,
					LOSEARBITRATION�������������ݹ�����ʧȥ�ٲã�����TWINT�����߻ᱻ�ͷţ������Ѱַ�ӻ�ģʽ��
 *  Description:  Twi ��Ϊ������������
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
UCHAR M_TwiSendData(UCHAR byte)
{
	UCHAR i=0;
	TWICON &= 0xf7;
	TWIDAT=byte;
	TWICON=0x40; 
	while(1)
	{
	   i++;
	   if(TWICON&0x08)
	   {
		  if((TWISTA&0xF8)==0x28)
		  {
		    return ACK;
		  }
		  else if((TWISTA&0xF8)==0x30)
		  {
		    return NAK;
		  }
		  else if((TWISTA&0xF8)==0x38)
		  {
		    return LOSEARBITRATION;
		  }
		  else
		  {
			 return ERR;
		  }
	   }
	   if(i>=250)
	   {
		 return ERR;       //�ȴ��жϳ�ʱ
	   }
	}

}
/*****************************************************************************************************
 *  Function Name: M_TwiRcvData()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         
 *  Return:        ACK�������յ����ݺ��ѻ�ӦACK���ɼ�����������
				   LOSEARBITRATION�������������ݹ�����ʧȥ�ٲã���TWINT�����߻ᱻ�ͷţ������Ѱַ�ӻ�ģʽ��
				   ERR��������������
 *  Description:  Twi ��Ϊ������������
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
UCHAR M_TwiRcvData()
{

	TWIDAT=0;
	TWICON=0x44;
	while(1)
	{
	  if(TWICON&0x08)
	  {
		 if((TWISTA&0xF8)==0x50)
		 {
		    return ACK;
		 }
		 else if((TWISTA&0xF8)==0x38)
		 {
		    return LOSEARBITRATION;
		 }
		 else
		 {
			return ERR;
		 }

	  }

	}

}  
/*****************************************************************************************************
 *  Function Name: M_TwiRcvLastData()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         
 *  Return:        NAK�������յ����ݺ��ѻ�ӦNAK����ʾ�����ټ�����������
				   LOSEARBITRATION�������������ݹ�����ʧȥ�ٲã���TWINT�����߻ᱻ�ͷţ������Ѱַ�ӻ�ģʽ��
				   ERR��������������
 *  Description:  Twi ��Ϊ������������
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
UCHAR M_TwiRcvLastData()
{
	TWIDAT=0;
	TWICON=0x40;
	while(1)
	{
	  if(TWICON&0x08)
	  {
		 if((TWISTA&0xF8)==0x58)
		 {
			return NAK;
		 }
		 else if((TWISTA&0xF8)==0x38)
		 {
		    return LOSEARBITRATION;
		 }
		 else
		 {
			return ERR;
		 }

	  }


	}

}
/*****************************************************************************************************
 *  Function Name: M_TwiSendEnd()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         
 *  Return:        				   
 *  Description:  Twi ��Ϊ�������ͽ�������
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
UCHAR M_TwiSendEnd()
{
   UCHAR i=0;
   TWICON=0x50;
   	while(1)
	{	 
		i++;
		if((TWICON&0x10)==0)
		{
		   if(TWISTA==0xF8)
		   {
		   	 return OK;
		   }
		   else
		   {
		   	 return FAIL;
		   }
		   
		}
		if(i>=250)
	    {
		   return ERR;       //�ȴ���ֹλ�����ʱ
	    }

	}
}
//TWI SLAVE
/*****************************************************************************************************
 *  Function Name: S_TwiWaitRcvCmd()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Return:        CMD_WRITE:�յ��������͵�д����
				   CMD_READ���յ��������͵Ķ�����
				   ERR�������������
 *  Description:  
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 


UCHAR S_TwiWaitRcvCmd()
{
   TWIADR=0x5A;			//�ӻ���ַ�趨
   TWICON=0x44;
   while(1)
   {
	  if(TWICON&0x08)
	  {
		 if((TWISTA&0xF8)==0x60) //�յ�SLA+W
		 {
			return CMD_WRITE;
		 }
		 else if((TWISTA&0xF8)==0xA8)	//�յ�SLA+R
		 {

			return CMD_READ;
		 }
		 else
		 {
			return ERR;
		 }
	  }

   }

}
/*****************************************************************************************************
 *  Function Name: S_TwiWaitRcvData()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Return:        ACK:�ӻ����յ�1��byte���ݣ��ѻ�ӦACK���ɼ�����������
				   ERR���������ݳ���
 *  Description:  
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
UCHAR S_TwiWaitRcvData()
{
	UCHAR i=0;
	TWIDAT=0;
	TWICON=0x44;
	while(1)
	{
	  i++;
	  if(TWICON&0x08)
	  {
	  	  if(((TWISTA&0xF8)==0x80)||((TWISTA&0xF8)==0x90)) //�յ�1��byte����
		  {
			 return ACK;
		  }
		  else
		  {
			 return ERR; 
		  }
	  }

	}
	
}
/*****************************************************************************************************
 *  Function Name: S_TwiWaitRcvLastData()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Return:        NAK:�ӻ����������1��byte���ݣ��л�����Ѱַ�ӻ�ģʽ
				   ERR���������ݳ���
 *  Description:  
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
UCHAR S_TwiWaitRcvLastData()
{
	UCHAR i=0;
	TWIDAT=0;
	TWICON=0x40;
	while(1)
	{
	  i++;
	  if(TWICON&0x08)
	  {
	  	  if(((TWISTA&0xF8)==0x88)||((TWISTA&0xF8)==0x98)) //�յ����1��byte����
		  {
			 return NAK;
		  }
		  else
		  {
			 return ERR; 
		  }
	  }

	}

}
/*****************************************************************************************************
 *  Function Name: S_TwiSendData(UCHAR byte)
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         byte
 *  Return:        ACK:�ӻ�������1��byte���ݣ��յ�������Ӧ��ACK(��ʾ�������ټ�����������)
 				   NAK���ӻ�������1��byte�����յ�������Ӧ��NAK����ʾ�������ټ����������ݣ����ӻ��л�����Ѱַģʽ��
				   ERR���������ݳ���
 *  Description:  
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
UCHAR S_TwiSendData(UCHAR byte)
{
	UCHAR i=0;
	TWIDAT=byte;
	TWICON=0x44;
	while(1)
	{
	  i++;
	  if(TWICON&0x08)
	  {
		  if((TWISTA&0xF8)==0xB8)
		  {
			 return ACK;
		  }
		  else if((TWISTA&0xF8)==0xC0) //�����������ݽ������ӻ��л�����Ѱַ״̬
		  {
			 return NAK;
		  }
		  else
		  {
			 return ERR;
		  }
	  }
	  if(i>=250)
	  {
		 return ERR;       //�ȴ��жϳ�ʱ
	  }

	}

}
/*****************************************************************************************************
 *  Function Name: S_TwiSendLastData(UCHAR byte)
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         byte
 *  Return:        ACK:�ӻ����������1��byte���ݵ������Կɼ����������ݣ��ӻ��л�����Ѱַģʽ��
 				   NAK���ӻ����������1��byte�������������ɼ����������ݣ��ӻ��л�����Ѱַģʽ��
				   ERR���������ݳ���
 *  Description:  
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
UCHAR S_TwiSendLastData(UCHAR byte)
{
	UCHAR i=0;
	TWIDAT=byte;
	TWICON=0x40;
	while(1)
	{
	  i++;
	  if(TWICON&0x08)
	  {
		  if((TWISTA&0xF8)==0xC8)	   //�ӻ��������ݽ������л�����Ѱַ״̬
		  {
			 return ACK;
		  }
		  else if((TWISTA&0xF8)==0xC0) //�����������ݽ������ӻ��л�����Ѱַ״̬
		  {
			 return NAK;
		  }
		  else
		  {
			 return ERR;
		  }
	  }
	  if(i>=250)
	  {
		 return ERR;       //�ȴ��жϳ�ʱ
	  }

	}
}