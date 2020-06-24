#include "api_ext.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif
#include "timer_define.h"
#include <intrins.h>
/*****************************************************************************************************
 *  Function Name: init_timer3
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-16
 *  Input:         None
 *  Output:        None
 *  Description:   ��ʼ����ʱ����������Ӧ��ģʽ��
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
void init_timer3()
{
    _push_(INSCON);
    select_bank1();

    T3CON = 0x0;	  //!< div = 1/1, sysclk, T3 as I/O port
    /**Fsys = 12M, T = 1/Fsys, x = 2ms = 2 / T, TH3+TL3 = 65536-x*/

    TL3 = 0x40;
    TH3 = 0xA2;


    TF3 = 0;    //!< clear flag
    T3CON |= 0x04;   //!< start timer

    select_bank0();
    _pop_(INSCON);

    IEN1 |= 0x10;       //!< enable timer3 isr
    IENC1 |= 0x10;      //!< enable timer3 overflow isr
}