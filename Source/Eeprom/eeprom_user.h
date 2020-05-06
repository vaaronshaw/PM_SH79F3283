#ifndef _EEPROM_USER_H
#define _EEPROM_USER_H

#include "defines.h"
#include "cpu.h"
#if (CPU_TYPE==CPU_SH79F3283)
#include "SH79F3283.h"
#elif (CPU_TYPE==CPU_SH79F328)
#include "SH79F328.h"
#else
#include "SH79F328.h"
#endif





#define EEP_USER_DATA_ADDR		0x10	//!< demo test








enum {
	EEP_SECTOR_0 = 0,
	EEP_SECTOR_1,
	EEP_SECTOR_2,
	EEP_SECTOR_3,
	EEP_SECTOR_NUM
};









extern void EEP_vReadBytes(uchar ucSectorIndex, uchar ucAddr, uchar* pucData, uchar ucDataLen);
extern void EEP_vProgramBytes(uchar ucSectorIndex, uchar ucAddr, uchar* pucData, uchar ucDataLen);
extern void EEP_vSectorErase(uchar ucSectorIndex);

























#endif