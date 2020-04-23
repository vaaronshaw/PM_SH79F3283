#include "cpu.h"
extern UCHAR EEPromByteRead(UCHAR nAddrH,UCHAR nAddrL); 
extern void EEPromByteProgram(UCHAR nAddrH,UCHAR nAddrL, UCHAR nData);
extern void EEPromSectorErase(UCHAR nAddrH);