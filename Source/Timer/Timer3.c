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
#include "intrins.h"
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

#if (CPU_TYPE==CPU_SH79F3283)
    T3CON = 0x10;	  //!< div = 1/8, sysclk, T3 as I/O port
#if 1   //!< 2ms
    /**T = 8/8000000, x = 2ms = 2000 * T, TH3+TL3 = 65536-x*/
    TL3 = 0x30;
    TH3 = 0xF8;
#else
    /**T = 8/8000000, x = 5ms = 5000 * T, TH3+TL3 = 65536-x*/
    TL3 = 0x78;
    TH3 = 0xEC;
#endif
#else
    T3CON = 0x10;	  //!< div = 1/8, sysclk, T3 as I/O port
    
    /**T = 8/12000000, x = 20ms = 30000 * T*/
    TL3 = 0x30;
    TH3 = 0x75;
#endif
    TF3 = 0;    //!< clear flag
    T3CON |= 0x04;   //!< start timer

    select_bank0();
    _pop_(INSCON);

#if (CPU_TYPE==CPU_SH79F3283)
    IEN1 |= 0x10;       //!< enable timer3 isr
    IENC1 |= 0x10;      //!< enable timer3 overflow isr
#elif (CPU_TYPE==CPU_SH79F328)
    //IEN0 |= 0x20;    //!< enable timer3 isr

#endif

}