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
void PWM_Test()
{
  CLKCON &= 0x9F;
  init_pwm0();
  init_pwm1();
  while(1);


}