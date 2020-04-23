#include "SH79F328.h"
#include "intrins.h"
#include "cpu.h"

#include "eeprom_ext.h"
void Eeprom_Test()
{
  UCHAR val;
  IEN0 &=0x7F;//关总中断
  EEPromSectorErase(0);
  EEPromByteProgram(0x0,0x0,0xA5);
  val=EEPromByteRead(0x0,0x0);
  IEN0 |=0x80; //开总中断
}