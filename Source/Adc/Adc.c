#include "SH79F328.h"
#include "intrins.h"
#include "cpu.h"
//#define ADC_ONE
#define ADC_ARRAY

void init_adc()
{
	UINT16 ADC_res[8];
	UCHAR i;
	ADCON1 = 0x80;	//ADON, VDD as vref, no trigger

/***************************************************************************************
24M SYSCLK, 500ksps
****************************************************************************************/
 	ADT=0x21;

	#ifdef ADC_ONE
	ADCON2 = 0x0;	//1-channel
	ADCH1 = 0x01;	//AN0
/*******************************************************************************************
 启动AD转换
********************************************************************************************/
	ADCON1 |= 0x01;   //ADC GO
	while(ADCON1 & 0x01);	//check go/done
	ADC_res[i] = ((ADDXH << 4) + (ADDXL >> 4));
	#endif

	#ifdef ADC_ARRAY
	ADCON2 = 0x72;	//8-channels,time of gap:4Tad
	ADCH1 = 0xff;	//P3.7~P3.4, P4.3~P4.0,AN7~AN0channel as adc input
//configure order of channals transaction: AN7,AN6, AN5, AN4, AN3, AN2, AN1, AN0
	SEQCON = 0x0;	//configure SEQCH0
	SEQCHX = 0x7;
	SEQCON = 0x01;	//configure SEQCH1
	SEQCHX = 0x06;
	SEQCON = 0x02;	//configure SEQCH2
	SEQCHX = 0x05;
	SEQCON = 0x03;	//configure SEQCH3
	SEQCHX = 0x04;
	SEQCON = 0x04;	//configure SEQCH4
	SEQCHX = 0x03;
	SEQCON = 0x05;	//configure SEQCH5
	SEQCHX = 0x02;
	SEQCON = 0x06;	//configure SEQCH6
	SEQCHX = 0x01;
	SEQCON = 0x07;	//configure SEQCH7
	SEQCHX = 0x0;

	SEQCON &= 0x7f;	//result left-align
/*******************************************************************************************
 启动AD转换
********************************************************************************************/
	ADCON1 |= 0x01;   //ADC GO
	while(ADCON1 & 0x01);	//check go/done
	//get result.
	for(i = 0; i < 8; i++)
	{
		SEQCON = i;
		ADC_res[i] = ((ADDXH << 4) + (ADDXL >> 4));
	}
	#endif
}