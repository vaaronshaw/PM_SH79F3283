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
#include "Twi_ext.h"
#include "api_ext.h"
UCHAR rcv_data[10];
#define H_Timeout_check	    TWITOUT |= 0xc0;  TWISTA |=0x01;TWICON |=0X01; 					 
                                
void Twi_Test()
{
	#ifdef    MT_MODE
	M_SendDataToSlave();
	#endif
    #ifdef   MR_MODE
	M_RcvDataFromSlave();
	#endif
	#ifdef   SLAVE_MODE
	SlaveTransfer();
	#endif
	while(1);
}

#ifdef    MT_MODE	 //	����328��������������ģʽ
/*****************************************************************************************************
 *  Function Name: M_SendDataToSlave()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         
 *  Return:         0:fail�������������ݹ�����ʧȥ�ٲû����  1:success�������ɹ����������ݣ�
 *  Description:  TWI��Ϊ������ӻ������������ݣ�ֱ���ӻ�����NAK��ʾ���ٽ������ݣ�����ֹͣ��������
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 

bit M_SendDataToSlave()
{
	UCHAR ret,i=0;
	TwiInit();
	H_Timeout_check; //ʹ�����߳�ʱ��SCL���߸ߵ�ƽ��ʱ�ж�
	ret=M_TwiSendStart();
	if(ret==OK)	//send-STA OK
	{
	  if(M_TwiSendCmd(0x2D,0x0)==ACK)
	  {
		 while(i < 0xff)
		 {
			 ret=M_TwiSendData(0x40+i);
			 i++;
			 if(ret==NAK)
			 {
				 return 1;        //���ݷ��ͽ���	
			 }
			 else if((ret==LOSEARBITRATION)||(ret==ERR))
			 {
			     return 0;
			 }
		 }
	  }

	}
	else	   //err process
	{
	   while(1);

	}


}
#endif

/*****************************************************************************************************
 *  Function Name: M_RcvDataFromSlave()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         
 *  Return:         0:fail  1:success
 *  Description:  TWI��Ϊ��������10��byte����
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
#ifdef   MR_MODE		  //����6161��������������ģʽ
bit M_RcvDataFromSlave()
{
	UCHAR i=0;
	TwiInit();
	H_Timeout_check;
	if(M_TwiSendStart()==OK)
	{
		if(M_TwiSendCmd(0x2D,0xFF)==ACK)
		{
		   while(1)
		   {
			   if(M_TwiRcvData()==ACK)
			   {
				  i++;
			      rcv_data[i-1]=TWIDAT;
				  if(i>=9)
				  {
					 if(M_TwiRcvLastData()==NAK)
					 {
						 rcv_data[i]=TWIDAT;
						 return 1;
					 }
					 else
					 {
					     return 0;
					 }
				  }
	
			   }
			   else
			   {
	
				   return 0;        //��������ʧ��
			   }

		   }
		}
		else		               //deal special case
		{


		}


	}
	else	                      //err process
	{

	}

   return 0;


}
#endif

/*****************************************************************************************************
 *  Function Name: SlaveTransfer()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         
 *  Return:         0:fail  1:success
 *  Description:  TWI��Ϊ�ӻ�����10��byte���ݣ�TWI��Ϊ�ӻ�����10��byte���ݣ�
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
#ifdef   SLAVE_MODE		//����328�����ڴӻ�ģʽ
bit SlaveTransfer()
{
    UCHAR* rcv_data,i=0,ret;
	TwiInit();
	ret=S_TwiWaitRcvCmd();
	if(ret==CMD_WRITE)
	{

	   while(1)
	   {
		 if(S_TwiWaitRcvData()==ACK)//�ɹ����յ�1��byte
	     {
		  	i++;
		    rcv_data[i-1]=TWIDAT;
			if(i>=9)
			{
			   if(S_TwiWaitRcvLastData()==NAK)
			   {
				  rcv_data[i]=TWIDAT;
				  return 1; 		//�������ݽ���
			   }
			   else
			   {
				  return 0; 	    //�������1��byte���ݹ����г���
			   }
			}
	     }
	     else
	     {
		    return 0;        //��������ʧ��
	     }

	   }
	   

	}
	else if(ret==CMD_READ)		  
	{
		while(1)
		{
			ret=S_TwiSendData(0x40+i);
			if(ret==NAK)  //�����������ݽ���
			{
			   return 1;
			}
			else if(ret==ACK) //�������ɼ�����������
			{
			  i++;
			  if(i>=9)
			  {
				if(S_TwiSendLastData(0x49)==ERR)
				{
				 	return 0;		 //�������1��byte���ݹ����г���
				}
				else
				{
				    return 1;         //�ɹ��������1��byte����
				}
			  }
			}
			else
			{
			   return 0; //�������ݳ���
			}
	    }

     }
	 else
	 {
	 	return 0;
	 }
}

#endif