/*
*********************************************************************************************************
*                                         SH88F6161 Demo code 
*
*                                        (c) Copyright 2014-11-21
*                                           All Rights Reserved
*
* File    : CPU.H
* By      : JACK
* Version : V0.0
*********************************************************************************************************
*/
#ifndef _CPU_H 
#define _CPU_H




#ifndef CPU_SH79F328
#define CPU_SH79F328	1
#endif

#ifndef CPU_SH79F3283
#define CPU_SH79F3283	2
#endif

#define CPU_TYPE CPU_SH79F3283
/**********************************************************************************************************
*                    定义与编译器无关的数据类型
**********************************************************************************************************/

typedef unsigned char  UCHAR;                   /* 无符号8位整型变量   */
typedef unsigned int   UINT16;                  /* 无符号16位整型变量 */
typedef signed   int   INT16;                  /* 有符号16位整型变量  */

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0x0
#endif

#ifndef NULL
#define NULL  0
#endif

#ifndef ON
#define ON  1
#endif

#ifndef OFF
#define OFF  0
#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
#endif