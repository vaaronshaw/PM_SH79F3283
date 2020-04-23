#include "wdt.h"

void WDT_vInit(void)
{
	RSTSTAT |= 0x02;	//!< 256ms
}