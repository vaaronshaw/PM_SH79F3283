#include "intrins.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif
#include "api_ext.h"
#include "spi_define.h"
void  init_spi()
{
	CLKCON =0x08;
  	Delay();
  	CLKCON|=0x04; 
#ifdef  SLAVE_MODE		//Slave Mode	
/**************************************************************************************************
	1.MSB优先发送
	2.配置SPI作为从属设备
	3.SCK周期的第二沿采集数据
	4.在Idle状态下SCK处于低电平
	5.关闭SS引脚
	6.选择外部设备时钟频率为fsys/2

***************************************************************************************************/
	SPCON=0x28;
	SPSTA |=0x80;      //使能SPI模块
#endif
#ifdef  MASTER_MODE		//Master Mode
/**************************************************************************************************
	1.MSB优先发送
	2.配置SPI作为主设备
	3.SCK周期的第二沿采集数据
	4.在Idle状态下SCK处于低电平
	5.关闭SS引脚
	6.选择外部设备时钟频率为fsys/2

***************************************************************************************************/
	SPCON=0x68;
	SPSTA |=0x80;      //使能SPI模块	
#endif
}
