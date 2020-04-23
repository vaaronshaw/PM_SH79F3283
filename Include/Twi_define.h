/*******************************SH79F326*************************************************************************/
#ifndef _TWI_DEFINE_H 
#define _TWI_DEFINE_H
//工作模式宏定义
#define MT_MODE	   //  配置326工作在主机传送模式
//#define MR_MODE			//	配置326工作在主机接收模式
//#define	SLAVE_MODE	 //配置326工作在从机模式

//返回值宏定义
#define ERR               0
#define ACK               1
#define	NAK    	          2
#define RCVCMDREAD        3
#define RCVCMDWRITE       4
#define RCVADDRESS0       5
#define LOSEARBITRATION	  6
#define CMD_WRITE		  7
#define	CMD_READ		  8

#define FAIL              0
#define OK                1
#endif