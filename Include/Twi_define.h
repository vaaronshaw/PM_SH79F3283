/*******************************SH79F326*************************************************************************/
#ifndef _TWI_DEFINE_H 
#define _TWI_DEFINE_H
//����ģʽ�궨��
#define MT_MODE	   //  ����326��������������ģʽ
//#define MR_MODE			//	����326��������������ģʽ
//#define	SLAVE_MODE	 //����326�����ڴӻ�ģʽ

//����ֵ�궨��
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