#ifndef _WDT_H
#define _WDT_H

#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif

extern void WDT_vInit(void);

#define WDT_FEED()  {RSTSTAT = 0;}



















































#endif