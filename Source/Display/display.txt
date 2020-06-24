#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "defines.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif








extern void DIS_vInit(void);
extern void DIS_vBlinkTest(void);










































#endif