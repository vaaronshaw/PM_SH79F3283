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

#ifdef    MT_MODE	 //	配置328工作在主机传送模式
/*****************************************************************************************************
 *  Function Name: M_SendDataToSlave()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         
 *  Return:         0:fail（主机发送数据过程中失去仲裁或出错）  1:success（主机成功发送完数据）
 *  Description:  TWI作为主机向从机连续发送数据，直至从机返回NAK表示不再接收数据，主机停止发送数据
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 

bit M_SendDataToSlave()
{
	UCHAR ret,i=0;
	TwiInit();
	H_Timeout_check; //使能总线超时，SCL总线高电平超时判断
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
				 return 1;        //数据发送结束	
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
 *  Description:  TWI作为主机接收10个byte数据
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
#ifdef   MR_MODE		  //配置6161工作在主机接收模式
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
	
				   return 0;        //接收数据失败
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
 *  Description:  TWI作为从机接收10个byte数据；TWI作为从机发送10个byte数据；
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
#ifdef   SLAVE_MODE		//配置328工作在从机模式
bit SlaveTransfer()
{
    UCHAR* rcv_data,i=0,ret;
	TwiInit();
	ret=S_TwiWaitRcvCmd();
	if(ret==CMD_WRITE)
	{

	   while(1)
	   {
		 if(S_TwiWaitRcvData()==ACK)//成功接收到1个byte
	     {
		  	i++;
		    rcv_data[i-1]=TWIDAT;
			if(i>=9)
			{
			   if(S_TwiWaitRcvLastData()==NAK)
			   {
				  rcv_data[i]=TWIDAT;
				  return 1; 		//接收数据结束
			   }
			   else
			   {
				  return 0; 	    //接收最后1个byte数据过程中出错
			   }
			}
	     }
	     else
	     {
		    return 0;        //接收数据失败
	     }

	   }
	   

	}
	else if(ret==CMD_READ)		  
	{
		while(1)
		{
			ret=S_TwiSendData(0x40+i);
			if(ret==NAK)  //主机接收数据结束
			{
			   return 1;
			}
			else if(ret==ACK) //主机还可继续接收数据
			{
			  i++;
			  if(i>=9)
			  {
				if(S_TwiSendLastData(0x49)==ERR)
				{
				 	return 0;		 //发送最后1个byte数据过程中出错
				}
				else
				{
				    return 1;         //成功发送最后1个byte数据
				}
			  }
			}
			else
			{
			   return 0; //发送数据出错
			}
	    }

     }
	 else
	 {
	 	return 0;
	 }
}

#endif