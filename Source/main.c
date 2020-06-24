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
#include "sw_timer.h"
/****************************Global Variables Start*************************************************/
 
/****************************Global Variables End**************************************************/

/****************************Funtion Quotation Start*************************************************/ 
static void MAIN_vNaGpioInit(void)
{
#define P0_NA_BIT       (0x80 | 0x40)
#define P1_NA_BIT       (0x00)
#define P2_NA_BIT       (0x00)
#define P3_NA_BIT       (0x00)
#define P4_NA_BIT       (0x10 | 0x08 | 0x04 | 0x02| 0x01)
#define P5_NA_BIT       (0x08 | 0x02 | 0x01)

    /**P0 unused pin*/
    P0CR |= P0_NA_BIT;	    //!< output
    P0PCR &= ~P0_NA_BIT;	//!< no pullup
    P0 &= ~P0_NA_BIT;	    //!< low

    /**P1 unused pin*/
    P1CR |= P1_NA_BIT;	    //!< output
    P1PCR &= ~P1_NA_BIT;	//!< no pullup
    P1 &= ~P1_NA_BIT;	    //!< low

    /**P2 unused pin*/
    P2CR |= P2_NA_BIT;	    //!< output
    P2PCR &= ~P2_NA_BIT;	//!< no pullup
    P2 &= ~P2_NA_BIT;	    //!< low

    /**P3 unused pin*/
    P3CR |= P3_NA_BIT;	    //!< output
    P3PCR &= ~P3_NA_BIT;	//!< no pullup
    P3 &= ~P3_NA_BIT;	    //!< low

    /**P4 unused pin*/
    P4CR |= P4_NA_BIT;	    //!< output
    P4PCR &= ~P4_NA_BIT;	//!< no pullup
    P4 &= ~P4_NA_BIT;	    //!< low

    /**P5 unused pin*/
    _push_(INSCON);
    select_bank1();

    P5CR |= P5_NA_BIT;	    //!< output
    P5PCR &= ~P5_NA_BIT;	//!< no pullup
    P5 &= ~P5_NA_BIT;	    //!< low

    select_bank0();
    _pop_(INSCON);
}
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
    MAIN_vNaGpioInit(); //!< unused pin 
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
    TMR_uiTimer[TMR_POWER_ON_INIT] = TMR_TIME_MS2TICKS(500);

    while(1)
    {
        WDT_FEED();

        TASK_vScheduleTasks();
    }
}