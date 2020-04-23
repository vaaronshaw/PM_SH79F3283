#include "cpu.h"
void TwiInit();
bit M_TwiSendStart();
UCHAR M_TwiSendCmd(UCHAR addr,UCHAR cmd);
UCHAR M_TwiSendData(UCHAR byte);
UCHAR M_TwiRcvData();
UCHAR M_TwiRcvLastData();
UCHAR M_TwiSendEnd();
UCHAR S_TwiWaitRcvCmd();
UCHAR S_TwiWaitRcvData();
UCHAR S_TwiWaitRcvLastData();
UCHAR S_TwiSendData(UCHAR byte);
UCHAR S_TwiSendLastData(UCHAR byte);

bit M_SendDataToSlave();
bit M_RcvDataFromSlave();
bit SlaveTransfer();