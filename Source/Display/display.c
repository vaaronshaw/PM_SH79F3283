#include "display.h"
static uchar DIS_ucBlinkState = 0;

void DIS_vInit(void)
{
	/**COM: P3.3 ~ P3.7*/
	P3CR |= 0xF8;	//!< output
	P3PCR &= ~0xF8;	//!< no pullup
	P3 &= ~0xF8;	//!< low

	/**Seg: P3.0 ~ P3.2*/
	P3CR |= 0x07;	//!< output
	P3PCR &= ~0x07;	//!< no pullup
	P3 &= ~0x07;	//!< low

	/**Seg: P1.0 ~ P1.4*/
	P1CR |= 0x1F;	//!< output
	P1PCR &= ~0x1F;	//!< no pullup
	P1 &= ~0x1F;	//!< low

	/**Seg: P2.0*/
	//P2CR |= 0x01;	//!< output
	//P2PCR &= ~0x01;	//!< no pullup
	//P2 &= ~0x01;	//!< low

	//test
	//P3 &= ~0xF8;	//!< 
	P3 |= 0xF8;	//!< 
	P3 |= 0x07;	//!< 
	P1 |= 0x1F;	//!< 
	//P2 |= 0x01;

	DIS_ucBlinkState = 0;
}

void DIS_vBlinkTest(void)
{
	if (DIS_ucBlinkState)
	{
		DIS_ucBlinkState = 0;
		P3 |= 0xF8;
	}
	else
	{
		DIS_ucBlinkState = 1;
		P3 &= ~0xF8;
	}
}