#include "api_ext.h"
//#include "SH79F328.h"
#include "SH79F3283.h"
#include "timer_define.h"
void Timer_Test()
{
  P0CR = 0x01;	//P0.0 as output
  CLKCON =0x08;
  Delay();
  CLKCON|=0x04;
  P0CR = 0X01;
  #ifdef TIMER3
  init_timer3();
  #endif
  #ifdef TIMER4
  init_timer4();
  #endif
  #ifdef TIMER5
  init_timer5();
  #endif
  while(1);

}