#include "SH79F328.h"
#include"cpu.h"
extern void init_Crc();

void Crc_test()
{
  UCHAR i;
  init_Crc();
  while(1)
  {
  	i ++;
  };
}	
