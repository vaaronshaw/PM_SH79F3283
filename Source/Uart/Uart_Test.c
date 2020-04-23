#include "api_ext.h"
#include "uart_define.h"
#include"cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif
/*****************************************************************************************************
 *  Function Name: UART_TEST
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:   串口发送和接收
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
 void Uart_Test()
 {
 #ifdef Enable_uart_TX_test

   init_uart();
   /*循环发送数据96 */
	while(1)
	{
	    SBUF=0x96;
		while(!TI);
	    TI=0;
	  
	}

 #endif

 #ifdef Enable_uart_RX_test
	UCHAR i;
	UCHAR buffer[10];
	init_uart();
   /*等待接收10个数据*/
	for(i=0;i<10;i++)
	{
	  while(!RI);
	  buffer[i]=SBUF;
	  RI=0;
	}

    while(1);
 #endif

 }
