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
  TWIBR=0x02;		 //配置发送波特率，禁止总线超时判断	   f=fsys/(16+2*CR*TWIBR)
  TWISTA=0x0;		  //64分频
  TWICON = 0x40;	//ENTWI	，禁止高电平超时
  TWTFREE = 0xff;	//最大超时配置

}
//TWI Master
/*****************************************************************************************************
 *  Function Name: M_TwiSendStart()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         
 *  Return:         FAIL:发送起始条件过程中出错  OK:发送起始条件成功
 *  Description:  TWI作为主机发送开始条件；
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
bit M_TwiSendStart()
{
  TWICON |=0x20;     //总线空闲时发送起始条件
  //TWICON &= 0xdf;
  while(1)  //等待TWI中断
  {

	 if(TWICON&0x08)
	 {
	   if(((TWISTA&0xF8)==0x08)||((TWISTA&0xF8)==0x10))
	   {
		 return OK;		 //成功发送开始条件或重复开始条件
	   }
	   else
	   {
		 return FAIL;
	   }
	 }
	 if(0x02 == (0x02 & TWICON))
	 {
	     TWICON &= 0xdf;
		 return FAIL;       //等待中断超时
	 }
  }

}
/*****************************************************************************************************
 *  Function Name: M_TwiSendCmd(UCHAR addr,UCHAR cmd)
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         addr：从机地址,cmd：命令(0:写，0xFF:读)
 *  Return:         ACK
                    NAK：从机没有响应当前地址命令，主机应发送：重复开始条件/终止条件
					LOSEARBITRATION：主机发送SLA+R/W时，失去仲裁，（清TWINT后，总线会被释放，进入非寻址从机模式	）
					RCVCMDWRITE：作为主机发送SLA+R/W失去仲裁后，收到主机发送的SLA+W，已回ACK
					RCVCMDREAD：作为主机发送SLA+R/W失去仲裁后，收到主机发送的SLA+R，已回ACK
					RCVADDRESS0：作为主机发送SLA+R/W失去仲裁后，收到主机发送的通用地址， 已回ACK
					ERR：		 主机发送SLA+R/W时，出现其他错误
 *  Description:  Twi 作为主机发送从机地址+读/写
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
	 	SlaveAddr=((addr&0x7f)<<1)|0x01;         //读

	}
	else
	{
		SlaveAddr=((addr&0x7f)<<1);               //写
	}
	TWICON &= 0xf7;		//清除中断标志
	TWIDAT=SlaveAddr;
	while(1)  //等待TWI中断
	{
		i++;
		if(TWICON&0x08)
		{
		   if(cmd)
		   {
			   if((TWISTA&0xF8)==0x40)
			   {
				 return ACK;		 //成功发送SLA+R
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
				 return ACK;		 //成功发送SLA+W
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
		     return ERR;       //等待中断超时
		}
	}

}
/*****************************************************************************************************
 *  Function Name: M_TwiSendData(UCHAR byte)
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         byte
 *  Return:         ACK：从机回应ACK ，表示从机尚可继续接收数据
					NAK：从机回应NAK,
					LOSEARBITRATION：主机发送数据过程中失去仲裁，（清TWINT后，总线会被释放，进入非寻址从机模式）
 *  Description:  Twi 作为主机发送数据
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
		 return ERR;       //等待中断超时
	   }
	}

}
/*****************************************************************************************************
 *  Function Name: M_TwiRcvData()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         
 *  Return:        ACK：主机收到数据后已回应ACK，可继续接收数据
				   LOSEARBITRATION：主机接收数据过程中失去仲裁，清TWINT后，总线会被释放，进入非寻址从机模式，
				   ERR：出现其他错误
 *  Description:  Twi 作为主机接收数据
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
 *  Return:        NAK：主机收到数据后已回应NAK，表示不能再继续接收数据
				   LOSEARBITRATION：主机接收数据过程中失去仲裁，清TWINT后，总线会被释放，进入非寻址从机模式，
				   ERR：出现其他错误
 *  Description:  Twi 作为主机接收数据
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
 *  Description:  Twi 作为主机发送结束条件
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
		   return ERR;       //等待终止位清除超时
	    }

	}
}
//TWI SLAVE
/*****************************************************************************************************
 *  Function Name: S_TwiWaitRcvCmd()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Return:        CMD_WRITE:收到主机发送的写命令
				   CMD_READ：收到主机发送的读命令
				   ERR：接收命令出错
 *  Description:  
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 


UCHAR S_TwiWaitRcvCmd()
{
   TWIADR=0x5A;			//从机地址设定
   TWICON=0x44;
   while(1)
   {
	  if(TWICON&0x08)
	  {
		 if((TWISTA&0xF8)==0x60) //收到SLA+W
		 {
			return CMD_WRITE;
		 }
		 else if((TWISTA&0xF8)==0xA8)	//收到SLA+R
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
 *  Return:        ACK:从机接收到1个byte数据，已回应ACK，可继续接收数据
				   ERR：接收数据出错
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
	  	  if(((TWISTA&0xF8)==0x80)||((TWISTA&0xF8)==0x90)) //收到1个byte数据
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
 *  Return:        NAK:从机接收完最后1个byte数据，切换至非寻址从机模式
				   ERR：接收数据出错
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
	  	  if(((TWISTA&0xF8)==0x88)||((TWISTA&0xF8)==0x98)) //收到最后1个byte数据
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
 *  Return:        ACK:从机发送完1个byte数据，收到主机回应的ACK(表示主机可再继续接收数据)
 				   NAK：从机发送完1个byte数据收到主机回应的NAK（表示主机不再继续接收数据），从机切换至非寻址模式；
				   ERR：发送数据出错
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
		  else if((TWISTA&0xF8)==0xC0) //主机接收数据结束，从机切换至非寻址状态
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
		 return ERR;       //等待中断超时
	  }

	}

}
/*****************************************************************************************************
 *  Function Name: S_TwiSendLastData(UCHAR byte)
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         byte
 *  Return:        ACK:从机发送完最后1个byte数据但主机仍可继续接收数据，从机切换至非寻址模式；
 				   NAK：从机发送完最后1个byte数据且主机不可继续接收数据，从机切换至非寻址模式；
				   ERR：发送数据出错
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
		  if((TWISTA&0xF8)==0xC8)	   //从机发送数据结束，切换至非寻址状态
		  {
			 return ACK;
		  }
		  else if((TWISTA&0xF8)==0xC0) //主机接收数据结束，从机切换至非寻址状态
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
		 return ERR;       //等待中断超时
	  }

	}
}