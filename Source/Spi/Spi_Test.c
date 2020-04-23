#include "SH79F328.h"
#include "intrins.h"
#include "cpu.h"
#include "api_ext.h"
#include "spi_define.h"

UCHAR rcv_data[11];
void Spi_Test()
{
	init_spi();
#ifdef SLAVE_MODE
	while(1)
	{
	SpiSlaveTran();
	}
#endif
#ifdef MASTER_MODE
	while(1)
	{
	SpiMasterTran();
	}
#endif

}

void SpiSlaveTran()
{
  UCHAR val;
  while(1)
  {
   	if(SPSTA&0x40)
	{
	  val=SPDAT;
	  SPSTA &=0xbf;
	  SPDAT=val;
	}
  }
}
void SpiMasterTran()
{
  UCHAR i=0;
  while(1)
  {
	SPDAT=0x41+i;
  	while(1)
	{
	   if(SPSTA&0x40)
	   {
		   rcv_data[i]=SPDAT;
		   SPSTA &=0xbf;
		   i++;
		   break;
		}
	}
   	if(i>=11)
	{  
	   break;
	}
  }

}