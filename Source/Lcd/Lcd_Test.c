#include "api_ext.h"
#include"cpu.h"
#define LCD_RAM_MAX		28
UCHAR xdata LCD_RAM[LCD_RAM_MAX] _at_  0x530;

/*****************************************************************************************************
 *  Function Name: void Lcd_test()
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


void display()
{
	UCHAR i;
	for(i = 0x0; i <= 0x1b; i++)
	{
		LCD_RAM[i] = 0xff;
	}


}

void Lcd_Test()
{
	init_lcd();
	display();
	while(1);
}