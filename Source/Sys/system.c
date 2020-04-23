#include "intrins.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif


void Delay()
{
	UCHAR i=0x0a;
	while(i--);
}


 /*****************************************************************************************************
 *  Function Name: select_bank0
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:   �л����Ĵ���bank0 
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/

void select_bank0()
{
      INSCON=0x00;
}

 /*****************************************************************************************************
 *  Function Name: select_bank1
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:   �л����Ĵ���bank0 
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/

void select_bank1()
{
     INSCON = 0x40;
}

 /*****************************************************************************************************
 *  Function Name: power_pd
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:   ϵͳ����PDģʽ 
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/

void power_pd()
{
	 SUSLO=0x55;
	 PCON=0x02;
	 _nop_();
	 _nop_();
	 _nop_();
}

 /*****************************************************************************************************
 *  Function Name: power_idle
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:   ϵͳ����IDLEģʽ 
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/

 void power_idle()
{
	 SUSLO=0x55;
	 PCON=0x01;
	 _nop_();
	 _nop_();
	 _nop_();
}
