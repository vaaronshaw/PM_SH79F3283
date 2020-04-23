#include "api_ext.h"
#include"cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif
/*****************************************************************************************************
 *  Function Name: SysclkTest()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:    
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
void Sysclk_Test()
{
    P0CR |= 0x01;    //!< config Port bit0~bit2 as output
    P0_0 = 0;       //!< set P0_0~P0_2 output low
    //SetClk(); 

    while(1)	      //!< test clk
    {
        P0_0=~P0_0;
    }
}
