#ifndef _LIGHT_H
#define _LIGHT_H

#include "defines.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif





#define LIG_SET_ON()	(P1 |= 0x02)
#define LIG_SET_OFF()	(P1 &= ~0x02)


extern void LIG_vInit(void);






























#endif