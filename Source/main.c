/***************************************************************************************************
 * Copyright (C), 2016-2023, Sinowealth Tech. Co., Ltd.
 * File name:    main.c                                                
 * Author:       Xiao Chengzhi
 * Version:      V0.0 
 * Date:         2020-04-06                                            
 * Description:                                                                                                                                                                    
 * Others:                                                                 
 * Function List:                                                                                                                                          
 * History:                                                          
*****************************************************************************************************/
/****************************Include Files Start****************************************************/


#include "api_ext.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
    #include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
    #include "SH79F328.h"
#else
    #include "SH79F328.h"
#endif

#include "task.h"
#include "wdt.h"
#include "Button/button.h"
//#include "Twi_define.h"
#include "Display/display.h"
#include "Uart/Uart2PM.h"
#include "Buzzer/buzzer.h"
#include "Light/light.h"
#include "Fan/fan.h"
#include "Motor/motor.h"
#include "Eeprom/eeprom_user.h"
/****************************Global Variables Start*************************************************/
 
/****************************Global Variables End**************************************************/

/****************************Funtion Quotation Start*************************************************/ 

/****************************Function Quotation End**************************************************/
/*****************************************************************************************************
 *  Function Name: main
 *  Created By:    Xiao Chengzhi
 *  Created Date:  2020-04-06
 *  Input:         None
 *  Output:        None
 *  Description:   main 
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/

void main()
{
    EA = 0;	
    SetClk();	    //!< sysclk init
    init_timer3();  //!< timer init as systick
    U2P_vInit();    //!< uart init 
    WDT_vInit();
    BUZZ_vInit();
    LIG_vInit();
    FAN_vInit();
    MOT_vInit();

    EA = 1;	//!< enable all interrupts

    //EEP_vSectorErase(EEP_SECTOR_0);     //!< demo test
    //EEP_vProgramBytes(EEP_SECTOR_0, EEP_USER_DATA_ADDR, ucData, 10);   //!< demo test
    //EEP_vReadBytes(EEP_SECTOR_0, 0x15, ucRead, 5);  //!< demo test

    while(1)
    {
        WDT_FEED();

        TASK_vScheduleTasks();
    }
}