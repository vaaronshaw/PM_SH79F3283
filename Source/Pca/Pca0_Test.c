#include "api_ext.h"
#include "SH79F328.h"
void Pca0_Test()
{
	IEN2 |=0x20;//‘ –ÌPCA0÷–∂œ
	IEN0 |= 0x80;
	CLKCON = 0x00;
	select_bank1();
	init_Pca0cpm0();
	select_bank0();
	while(1);
}