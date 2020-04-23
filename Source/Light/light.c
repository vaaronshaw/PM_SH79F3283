#include "light.h"



void LIG_vInit(void)
{
	/**io init
	*P1.1: light
	*/
	P1CR |= 0x02;	//!< output
	P1PCR &= ~0x02;	//!< no pullup
	P1 &= ~0x02;	//!< low
}