#include "Uart2PM.h"
#include "ringbuffer.h"
#include "api_ext.h"
#include "sw_timer.h"
#include "buzzer.h"
#include "light.h"
#include "fan.h"
#include "hood.h"
#include "motor.h"






static ring_buffer_t U2P_tRingBuffer;
static Tu2pMessage U2P_tMessageToSend;
static Tu2pMessage U2P_tAckToSend;
static Tu2pMessage U2P_tMessageToReceive;
static uchar* U2P_pucNextCharToWrite;
static uchar U2P_ucCharRemainToWrite = 0;   //!< for transmission
static uchar U2P_ucCharRemainToCheck = 0;   //!< for reception

static ulong U2P_ulPropertyUploadFlag = 0;   //!< 0: property upload, >1: property query
//static ulong U2P_ulPropertySetFlag = 0;   

static TU2PDllTaskIndex U2P_tTaskState = (TU2PDllTaskIndex)0;
static uchar U2P_ucDataIndex = 0;
static uchar U2P_ucCrc = 0;

static uchar U2P_ucMsgSeqValue = 0;
static uchar U2P_ucMsgSeqValueLast = 0;

static uchar U2P_ucTransAckFirst = 0;
static uchar U2P_ucRetryCounter = 0;

#define U2P_TASK_NUM    10
static uchar U2P_ucTransmitTaskQueue[U2P_TASK_NUM] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  //!< max 10 tasks in queue
static uchar U2P_ucNextTaskIndex = 0;
static uchar U2P_ucLatestTaskIndex = 0;

//static uchar U2P_ucMessageIndex = 0;
/**static functions**/

#define U2P_CRC_INIT  	0x0      //!< Initial value of crc.
#define U2P_CRC_XOrOut	0x0
#define U2P_CRC_POLY	0x31


static void U2P_vCrcInit(void);
static void U2P_vAddByteToCrcMAXIM(uchar ucByte);
static uchar U2P_ucGetCalculatedCrc(void);

/**receive functions*/
static void U2P_vRecvAck(Tu2pMessage* tMsgObject);
static void U2P_vRecvSWInfoSync(Tu2pMessage* tMsgObject);
static void U2P_vRecvProdInfoSync(Tu2pMessage* tMsgObject);
static void U2P_vRecvHeartbeat(Tu2pMessage* tMsgObject);
static void U2P_vRecvStatusGet(Tu2pMessage* tMsgObject);
static void U2P_vRecvStatusSet(Tu2pMessage* tMsgObject);

/**transmission functions*/
static void U2P_vTransAck(Tu2pMessage* tMsgObject);
static void U2P_vTransSWInfoSync(Tu2pMessage* tMsgObject);
static void U2P_vTransProdInfoSync(Tu2pMessage* tMsgObject);
static void U2P_vTransHeartbeat(Tu2pMessage* tMsgObject);
static void U2P_vTransStatusGet(Tu2pMessage* tMsgObject);
static void U2P_vTransStatusSet(Tu2pMessage* tMsgObject);



static void U2P_vPropertyUpload(Tu2pMessage* tMsgObject);
/**Property Get Functions*/
static void U2P_vGetMachineState(Tu2pMessage* tMsgObject);
static void U2P_vGetWindVolume(Tu2pMessage* tMsgObject);
//static void U2P_vGetDelayTime(Tu2pMessage* tMsgObject);
static void U2P_vGetLightStatus(Tu2pMessage* tMsgObject);
static void U2P_vGetDoorCtrlState(Tu2pMessage* tMsgObject);
static void U2P_vGetDoorStatus(Tu2pMessage* tMsgObject);
static void U2P_vGetDoorCtrlState2(Tu2pMessage* tMsgObject);
static void U2P_vGetDoorStatus2(Tu2pMessage* tMsgObject);

/**Property Set Functions*/
static void U2P_vSetMachineState(Tu2pMessage* tMsgObject);
static void U2P_vSetWindVolume(Tu2pMessage* tMsgObject);
//static void U2P_vSetDelayTime(Tu2pMessage* tMsgObject);
static void U2P_vSetLightStatus(Tu2pMessage* tMsgObject);
static void U2P_vSetBuzzer(Tu2pMessage* tMsgObject);
static void U2P_vSetDoorCtrlState(Tu2pMessage* tMsgObject);
static void U2P_vSetDoorCtrlState2(Tu2pMessage* tMsgObject);

static void U2P_vCheckTransmitTask(void);
static void U2P_vTransmitAck(void);

static const Tu2pMessageObject U2P_tReceiveMsgTbl[] = {
    /**Msg ID,                      Ack require,                Data length,                        Msg service function*/
    {U2P_MSG_ID_ACK,                U2P_NO_ACK,                 U2P_MSG_DATALEN_ACK,                U2P_vRecvAck},
    {U2P_MSG_ID_SW_INFO_SYNC,       U2P_ACK,                    U2P_MSG_DATALEN_SW_INFO_SYNC,       U2P_vRecvSWInfoSync},
    {U2P_MSG_ID_PROD_INFO_SYNC,     U2P_ACK,                    U2P_MSG_DATALEN_PROD_INFO_SYNC,     U2P_vRecvProdInfoSync},
    {U2P_MSG_ID_HEART_BEAT,         U2P_NO_ACK,                 U2P_MSG_DATALEN_HEART_BEAT,         U2P_vRecvHeartbeat},
    {U2P_MSG_ID_STATUS_GET,         U2P_ACK,                    U2P_MSG_DATALEN_STATUS_GET,         U2P_vRecvStatusGet},
    {U2P_MSG_ID_STATUS_SET,         U2P_ACK,                    U2P_MSG_DATALEN_STATUS_SET,         U2P_vRecvStatusSet}
};

static const Tu2pMessageObject U2P_tTransmitMsgTbl[] = {
    /**Msg ID,                      Ack require,                Data length,                        Msg service function*/
    {U2P_MSG_ID_ACK,                U2P_NO_ACK,                 U2P_MSG_DATALEN_ACK,                U2P_vTransAck},
    {U2P_MSG_ID_SW_INFO_SYNC,       U2P_ACK,                    U2P_MSG_DATALEN_SW_INFO_SYNC,       U2P_vTransSWInfoSync},
    {U2P_MSG_ID_PROD_INFO_SYNC,     U2P_ACK,                    U2P_MSG_DATALEN_PROD_INFO_SYNC,     U2P_vTransProdInfoSync},
    {U2P_MSG_ID_HEART_BEAT,         U2P_NO_ACK,                 U2P_MSG_DATALEN_HEART_BEAT,         U2P_vTransHeartbeat},
    {U2P_MSG_ID_STATUS_GET,         U2P_ACK,                    U2P_MSG_DATALEN_STATUS_GET,         U2P_vTransStatusGet},
    //{U2P_MSG_ID_STATUS_SET,         U2P_ACK,                    U2P_MSG_DATALEN_STATUS_SET,         U2P_vTransStatusSet}
};

#define U2P_RECEPTION_MSG_TBL_SIZE      sizeof(U2P_tReceiveMsgTbl) / sizeof(U2P_tReceiveMsgTbl[0])
#define U2P_TRANSMIT_MSG_TBL_SIZE       sizeof(U2P_tTransmitMsgTbl) / sizeof(U2P_tTransmitMsgTbl[0])


/**Propertys Configuration Table*/
static const TpropertyConfig U2P_tPropertyTable[] = {
    /**PIID,            PIID value length,          Property Get Funtion,           Property Set Function*/
    {1,                 1,                          U2P_vGetMachineState,           U2P_vSetMachineState},      //!< Machine State
    {2,                 1,                          U2P_vGetWindVolume,             U2P_vSetWindVolume},        //!< wind volume
    {3,                 1,                          U2P_vGetLightStatus,            U2P_vSetLightStatus},     //!< light 
    //{4,                 1},     //!< wind pressure
    //{5,                 1},     //!< Turbo State
    //{6,                 1},     //!< air control
    //{7,                 1},     //!< air quality
    //{8,                 2,                          U2P_vGetDelayTime,              U2P_vSetDelayTime},     //!< delay time (s)
    //{9,                 1},     //!< delay speed
    //{10,                1},     //!< boost time
    //{11,                1},     //!< power feedback
    //{12,                1},     //!< heat oil control
    //{13,                1},     //!< steam wash control
    //{14,                1},     //!< steam wash temp
    {15,                1,                          U2P_vGetDoorCtrlState,          U2P_vSetDoorCtrlState},     //!< left door control
    {16,                1,                          U2P_vGetDoorStatus,             NULL},     //!< left door state
    //{17,                1},     //!< door resistance value
    //{18,                1},     //!< motor temp
    //{19,                2},     //!< detection temp1
    //{20,                2},     //!< detection temp2
    {21,                1,                          NULL,                           U2P_vSetBuzzer},     //!< buzzer control
    //{22,                1},     //!< voice control
    //{23,                1},     //!< IR control
    //{24,                1},     //!< interlink control
    //{25,                1},     //!< clean remind
    {26,                1,                          U2P_vGetDoorCtrlState2,          U2P_vSetDoorCtrlState2},     //!< right door control
    {27,                1,                          U2P_vGetDoorStatus2,             NULL},     //!< right door state

    //{201,               2},     //!< key value
    //{202,               2},     //!< indicator light control value
    //{203,               1},     //!< gesture control
    {0xFF,              1,                          NULL,                           NULL}     //!< error
};

static uchar U2P_ucGetPropertyIndex(uchar ucPIID);

/**reception functions*/
static void U2P_vRecvAck(Tu2pMessage* tMsgObject)
{
    /**no response, and clear retry counter to stop re-transmission*/
    U2P_ucRetryCounter = 0;
    TMR_uiTimer[TMR_U2P_MSG_NO_ACK_TIMEOUT] = 0;
}

static void U2P_vRecvSWInfoSync(Tu2pMessage* tMsgObject)
{
    /**data len 0 means info sync query*/
    if (tMsgObject->tMsgHeader.ucDataLen == 0)
    {
        U2P_vTransmitMessage(U2P_MSG_ID_SW_INFO_SYNC);
    }
}

static void U2P_vRecvProdInfoSync(Tu2pMessage* tMsgObject)
{
    /**data len 0 means info sync query*/
    if (tMsgObject->tMsgHeader.ucDataLen == 0)
    {
        U2P_vTransmitMessage(U2P_MSG_ID_PROD_INFO_SYNC);
    }
}

static void U2P_vRecvHeartbeat(Tu2pMessage* tMsgObject)
{

}

static void U2P_vRecvStatusGet(Tu2pMessage* tMsgObject)
{
    /**data len 0 means status upload query*/
    if (tMsgObject->tMsgHeader.ucDataLen == 0)
    {
        U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);
    }
}

static uchar U2P_ucGetPropertyIndex(uchar ucPIID)
{
    uchar i = 0;

    for (i=0; i<sizeof(U2P_tPropertyTable)/sizeof(U2P_tPropertyTable[0]); i++)
    {
        if (ucPIID == U2P_tPropertyTable[i].ucPIID)
        {
            return i;
        }
    }

    return 0xFF;    //!< not found
}

static void U2P_vRecvStatusSet(Tu2pMessage* tMsgObject)
{
    /**set properties' status*/
    uchar ucIndex = 0;
    uchar ucPIID = 0;
    uchar ucPiidValueLen = 0;
    uchar ucTotalCount = tMsgObject->tMsgHeader.ucDataLen;  //!< init value

    tMsgObject->tMsgHeader.ucDataLen = 0;   //!< reset value

    while (tMsgObject->tMsgHeader.ucDataLen < ucTotalCount)
    {
        ucPIID = tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++];
        ucPiidValueLen = tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++];

        ucIndex = U2P_ucGetPropertyIndex(ucPIID);

        if (ucIndex != 0xFF)
        {
            if (U2P_tPropertyTable[ucIndex].pPropSetFunc != NULL)
            {
                U2P_tPropertyTable[ucIndex].pPropSetFunc(tMsgObject);
            }
        }

        tMsgObject->tMsgHeader.ucDataLen += ucPiidValueLen; //!< add data length
    }
}


/**transmission functions*/
static void U2P_vTransAck(Tu2pMessage* tMsgObject)
{
    //tMsgObject->tMsgHeader.ucDataLen = 0;
}

static void U2P_vTransSWInfoSync(Tu2pMessage* tMsgObject)
{
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x11;  //!< demo test
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x22;  //!< demo test
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x33;  //!< demo test
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x44;  //!< demo test
}

static void U2P_vTransProdInfoSync(Tu2pMessage* tMsgObject)
{
#define PRODUCT_TYPE_LEN     4
#define PRODUCT_SN_LEN       4

    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = PRODUCT_TYPE_LEN;
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x01;  //!< demo test
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x02;  //!< demo test
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x03;  //!< demo test
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x04;  //!< demo test

    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = PRODUCT_SN_LEN;
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x01;  //!< demo test
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x02;  //!< demo test
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x03;  //!< demo test
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x04;  //!< demo test
}

static void U2P_vTransHeartbeat(Tu2pMessage* tMsgObject)
{
    //*pucDataLen = 0;
}

static void U2P_vTransStatusGet(Tu2pMessage* tMsgObject)
{
    if (0 == U2P_ulPropertyUploadFlag)
    {
        /**no propertys' value upload needed, require receiver to upload propertys*/
        ///*pucDataLen = 0;
    }
    else
    {
        /**transmit propertys value*/
        U2P_vPropertyUpload(tMsgObject);
    }
}

void U2P_vSetPropertyUploadFlag(uchar ucPIID)
{
    DEF_SET(U2P_ulPropertyUploadFlag, 1 << ucPIID);
}



static void U2P_vTransStatusSet(Tu2pMessage* tMsgObject)
{
    //uchar i = 0;

    //for (i = 0; i < 32, U2P_ulPropertySetFlag; i++)
    //{
    //    if (DEF_TEST(U2P_ulPropertySetFlag, 1 << i))
    //    {
    //        DEF_RES(U2P_ulPropertySetFlag, 1 << i);

    //        tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = U2P_tPropertyTable[i].ucPIID;
    //        tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = U2P_tPropertyTable[i].ucPiidValueLen;

    //        if (U2P_tPropertyTable[i].pPropSetFunc != NULL)
    //        {
    //            U2P_tPropertyTable[i].pPropSetFunc(tMsgObject);
    //        }
    //    }
    //}
}

/**propertys function*/
static void U2P_vPropertyUpload(Tu2pMessage* tMsgObject)
{
    uchar i = 0;

    for (i=0; i<32, U2P_ulPropertyUploadFlag; i++)
    {
        if (DEF_TEST(U2P_ulPropertyUploadFlag, 1<<i))
        {
            DEF_RES(U2P_ulPropertyUploadFlag, 1 << i);

            tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = U2P_tPropertyTable[i].ucPIID;
            tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = U2P_tPropertyTable[i].ucPiidValueLen;

            if (U2P_tPropertyTable[i].pPropGetFunc != NULL)
            {
                U2P_tPropertyTable[i].pPropGetFunc(tMsgObject);
            }
        }
    }
}


static void U2P_vGetMachineState(Tu2pMessage* tMsgObject)
{
    /**read property value*/
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = HOOD_tGetWorkingState(); 
}

static void U2P_vGetWindVolume(Tu2pMessage* tMsgObject)
{
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = FAN_tGetCurrentSpeed();  
}

static void U2P_vGetDelayTime(Tu2pMessage* tMsgObject)
{
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x01;      //!< demo
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = 0x22;      //!< demo
}

static void U2P_vGetLightStatus(Tu2pMessage* tMsgObject)
{
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = LIG_tGetLightStatus(); //!< ToDo: test
}

static void U2P_vGetDoorCtrlState(Tu2pMessage* tMsgObject)
{
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = MOT_tGetMotorCtrlState(MOT_INDEX_LEFT);    //!< ToDo: test
}

static void U2P_vGetDoorStatus(Tu2pMessage* tMsgObject)
{
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = MOT_tGetMotorStatus(MOT_INDEX_LEFT);   //!< ToDo: test
}

static void U2P_vGetDoorCtrlState2(Tu2pMessage* tMsgObject)
{
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = MOT_tGetMotorCtrlState(MOT_INDEX_RIGHT);    //!< ToDo: test
}

static void U2P_vGetDoorStatus2(Tu2pMessage* tMsgObject)
{
    tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen++] = MOT_tGetMotorStatus(MOT_INDEX_RIGHT);   //!< ToDo: test
}

/**************************************PIID Set Functions*********************************************/
static void U2P_vSetMachineState(Tu2pMessage* tMsgObject)
{
    uchar ucValue = 0;

    ucValue = tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen];

    /**check sequence validation and value*/
    if (!U2P_bIsSameSeqValue() && (ucValue <= HOOD_STATE_FACTORY_MODE))
    {
        HOOD_vSetWorkingState((THoodStateDef)ucValue);
    }
}

static void U2P_vSetWindVolume(Tu2pMessage* tMsgObject)
{
    uchar ucValue = 0;

    ucValue = tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen];

    /**check sequence validation and value*/
    if (!U2P_bIsSameSeqValue() && (ucValue <= FAN_SPEED_4))
    {
        FAN_vSetTargetSpeed((TFanSpeedDef)ucValue);

    }
}

static void U2P_vSetDelayTime(Tu2pMessage* tMsgObject)
{

}

static void U2P_vSetLightStatus(Tu2pMessage* tMsgObject)
{
    uchar ucValue = 0;

    ucValue = tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen];

    /**check sequence validation and value*/
    if (!U2P_bIsSameSeqValue() && (ucValue <= LIG_STATE_ON))
    {
        LIG_vSetTargetState((TLigStateDef)ucValue);
    }
}

static void U2P_vSetBuzzer(Tu2pMessage* tMsgObject)
{
    uchar ucValue = 0;

    ucValue = tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen];

    /**check sequence validation and value*/
    if (!U2P_bIsSameSeqValue() && (ucValue<BUZZ_RHYTHM_NUM))
    {
        BUZZ_vSetBuzzAlarm((TBuzzerRhythmIndex)ucValue);
    }
}

static void U2P_vSetDoorCtrlState(Tu2pMessage* tMsgObject)
{
    uchar ucValue = 0;

    ucValue = tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen];

    /**check sequence validation and value*/
    if (!U2P_bIsSameSeqValue() && (ucValue <= MOT_STATE_OPEN))
    {
        MOT_vSetMotorCtrlState(MOT_INDEX_LEFT, ucValue);
    }
}

static void U2P_vSetDoorCtrlState2(Tu2pMessage* tMsgObject)
{
    uchar ucValue = 0;

    ucValue = tMsgObject->ucData[tMsgObject->tMsgHeader.ucDataLen];

    /**check sequence validation and value*/
    if (!U2P_bIsSameSeqValue() && (ucValue <= MOT_STATE_OPEN))
    {
        MOT_vSetMotorCtrlState(MOT_INDEX_RIGHT, ucValue);
    }
}

/**
 Initialisation of the crc calculation (resets the present value of the crc). The initialisation
 must be called before starting a new crc calculation, hence every time a new message is received
 (start of the message reception).


\return
 \b type: void
*/
static void U2P_vCrcInit(void)
{
    U2P_ucCrc = U2P_CRC_INIT;
}

static void U2P_vAddByteToCrcMAXIM(uchar ucByte)
{
    uchar  i = 0;

    U2P_ucCrc ^= ucByte;  /* 每次先与需要计算的数据异或,计算完指向下一数据 */

    //for (i=8; i>0; --i)   /* 下面这段计算过程与计算一个字节crc一样 */
    //{
    //    if (U2P_ucCrc & 0x80)
    //    {
    //        U2P_ucCrc = (U2P_ucCrc << 1) ^ U2P_CRC_POLY;
    //    }
    //    else
    //    {
    //        U2P_ucCrc = (U2P_ucCrc << 1);
    //    }
    //}
    
    for (i = 8; i > 0; --i)
    {
        if (U2P_ucCrc & 0x01)
        {
            U2P_ucCrc = (U2P_ucCrc >> 1) ^ 0x8C;
        }
        else
        {
            U2P_ucCrc = (U2P_ucCrc >> 1);
        }
    }
}



static uchar U2P_ucGetCalculatedCrc(void)
{
    return (U2P_ucCrc);
}

static uchar U2P_ucGetTransmitMessageIndex(uchar ucMsgID)
{
    uchar i;

    for (i = 0; i < sizeof(U2P_tTransmitMsgTbl) / sizeof(U2P_tTransmitMsgTbl[0]); i++)
    {
        if(ucMsgID == U2P_tTransmitMsgTbl[i].ucMsgID)
        {
            return i;
        }
    }

    return 0xFF;
}

static uchar U2I_ucGetReceiveMessageIndex(uchar ucMsgID)
{
    uchar i;

    for (i = 0; i < sizeof(U2P_tReceiveMsgTbl) / sizeof(U2P_tReceiveMsgTbl[0]); i++)
    {
        if (ucMsgID == U2P_tReceiveMsgTbl[i].ucMsgID)
        {
            return i;
        }
    }

    return 0xFF;
}

static void U2P_vEnterIdleState(void)
{
    U2P_tTaskState = U2P_TASK_IDLE;
}


/**Call this to check if message is repeated*/
BOOL U2P_bIsSameSeqValue(void)
{
    return (U2P_ucMsgSeqValueLast == U2P_ucMsgSeqValue);
}

void U2P_vDllTaskHandler(void)
{
    /**disable rx interrupt*/

    switch (U2P_tTaskState)
    {
        default:
        case U2P_TASK_INIT:
        {
            ring_buffer_init(&U2P_tRingBuffer);	 //!< initialize ring buffer
            U2P_tMessageToSend.tMsgHeader.ucMsgSeqValue = 0;
            U2P_ucMsgSeqValueLast = 0;

            U2P_tTaskState = U2P_TASK_IDLE;

            //U2P_vTransmitAck(); //!< test
            //U2P_vTransmitMessage(U2P_MSG_ID_STATUS_GET);

            break;
        }

        case U2P_TASK_IDLE:
        {
            break;
        }

        case U2P_TASK_RECEIVE_ID:
        case U2P_TASK_RECEIVE_SEQ_NUM:
        case U2P_TASK_RECEIVE_DATALEN:
        case U2P_TASK_RECEIVE_DATA:
        {
            if (TMR_uiTimer[TMR_U2P_RECEIVE_TIME_OUT] == 0)
            {
                /**timeout, return to idle state*/
                U2P_vEnterIdleState();
            }

            break;
        }

        case U2P_TASK_SEND_DATA:
        {
            if (TMR_uiTimer[TMR_U2P_TRANSMIT_TIME_OUT] == 0)
            {
                /**timeout, return to idle state*/
                U2P_vEnterIdleState();

                if (U2P_ucTransAckFirst)
                {
                    U2P_ucTransAckFirst = 0;
                    TMR_uiTimer[TMR_U2P_TRANSMIT_TIME_OUT] = TMR_TIME_MS2TICKS(U2P_BYTE_INTERVAL_TIME); //!< delay 20ms to transmit next message
                }
            }

            break;
        }
    }
    /**enable rx interrupt*/
}

static void U2P_vCheckReceptionTask(void)
{
    uchar ucData;
    uchar i = 0;
    uchar ucMessageIndex = 0;

    if (U2P_tTaskState == U2P_TASK_SEND_DATA)
    {
        return;
    }

    for (; ring_buffer_dequeue(&U2P_tRingBuffer, (char*)&ucData) > 0;)
    {
        TMR_uiTimer[TMR_U2P_RECEIVE_TIME_OUT] = TMR_TIME_MS2TICKS(U2P_BYTE_INTERVAL_TIME);  //!< set timeout time

        switch (U2P_tTaskState)
        {
            case U2P_TASK_IDLE:
            {
                /**check head*/
                if ((U2P_MSG_HEAD_DEVICE == ucData) || (U2P_MSG_HEAD_FORWARD == ucData))
                {
                    U2P_tMessageToReceive.tMsgHeader.ucHead = ucData;
                    U2P_tTaskState = U2P_TASK_RECEIVE_SEQ_NUM;
                    U2P_vCrcInit();
                    U2P_vAddByteToCrcMAXIM(ucData);
                    break;
                }

                break;
            }

            case U2P_TASK_RECEIVE_SEQ_NUM:
            {
                U2P_tMessageToReceive.tMsgHeader.ucMsgSeqValue = ucData;
                //U2P_ucMsgSeqValue = ucData;
                U2P_tTaskState = U2P_TASK_RECEIVE_ID;
                U2P_vAddByteToCrcMAXIM(ucData);	//!< update calculated CRC

                break;
            }

            case U2P_TASK_RECEIVE_ID:
            {
                U2P_tMessageToReceive.tMsgHeader.ucMsgID = ucData;
                U2P_tTaskState = U2P_TASK_RECEIVE_DATALEN;
                U2P_vAddByteToCrcMAXIM(ucData);	//!< update calculated CRC

                break;
            }

            case U2P_TASK_RECEIVE_DATALEN:
            {
                U2P_tMessageToReceive.tMsgHeader.ucDataLen = ucData;
                U2P_ucCharRemainToCheck = ucData;
                U2P_vAddByteToCrcMAXIM(ucData);	//!< update calculated CRC
                U2P_ucDataIndex = 0;
                U2P_tTaskState = U2P_TASK_RECEIVE_DATA;

                break;
            }

            case U2P_TASK_RECEIVE_DATA:
            {
                if (U2P_ucCharRemainToCheck > 0)	//!< receive data until crc value
                {
                    U2P_ucCharRemainToCheck--;

                    U2P_tMessageToReceive.ucData[U2P_ucDataIndex++] = ucData;
                    U2P_vAddByteToCrcMAXIM(ucData);	    //!< update calculated crc
                }
                else
                {
                    U2P_tMessageToReceive.ucData[U2P_ucDataIndex++] = ucData;
                    U2P_ucCrc = U2P_ucGetCalculatedCrc();

                    U2P_vEnterIdleState();

                    /**receive low byte of CRC*/
                    if (U2P_ucCrc == ucData)
                    {
                        U2P_ucMsgSeqValueLast = U2P_ucMsgSeqValue;  //!< mark last sequence value
                        U2P_ucMsgSeqValue = U2P_tMessageToReceive.tMsgHeader.ucMsgSeqValue; //!< update sequence value

                        /**Message check pass, call function*/
                        ucMessageIndex = U2I_ucGetReceiveMessageIndex(U2P_tMessageToReceive.tMsgHeader.ucMsgID);

                        /**call service function*/
                        if (ucMessageIndex < U2P_RECEPTION_MSG_TBL_SIZE)    
                        {
                            if (U2P_ACK == U2P_tReceiveMsgTbl[ucMessageIndex].ucAckRequire)
                            {
                                U2P_vTransmitAck(); //!< transmit ack first
                            }

                            U2P_tReceiveMsgTbl[ucMessageIndex].pServiceFunc(&U2P_tMessageToReceive);
                        }
                    }
                }  

                break;
            }

            default:
            {
                U2P_vEnterIdleState();

                break;
            }
        }
    }
}

void U2P_vPLTaskHandler(void)
{
    U2P_vCheckReceptionTask();

    /**transmit ack first*/
    if (U2P_ucTransAckFirst)
    {
        if (U2P_TASK_IDLE == U2P_tTaskState)	//!< Rx is not ongoing
        {
            U2P_pucNextCharToWrite = (uchar*)&U2P_tAckToSend;	//!< init pointer for transmission
            U2P_ucCharRemainToWrite = U2P_tAckToSend.tMsgHeader.ucDataLen + 4;	//!< include header

            TMR_uiTimer[TMR_U2P_TRANSMIT_TIME_OUT] = TMR_TIME_MS2TICKS(U2P_BYTE_INTERVAL_TIME);     //!< set timeout for transmit

            U2P_vCrcInit();	//!<reset the crc calculation

            SBUF = *U2P_pucNextCharToWrite;     //!< first byte
            U2P_vAddByteToCrcMAXIM(*U2P_pucNextCharToWrite);

            U2P_pucNextCharToWrite++;
            U2P_ucCharRemainToWrite--;
            U2P_tTaskState = U2P_TASK_SEND_DATA;
        }
        else
        {
            //!< wait for idle
        }
    }
    else if (U2P_ucRetryCounter)
    {
        if (U2P_TASK_IDLE == U2P_tTaskState)	//!< Rx is not ongoing
        {
            if (0 == TMR_uiTimer[TMR_U2P_MSG_NO_ACK_TIMEOUT])
            {
                TMR_uiTimer[TMR_U2P_MSG_NO_ACK_TIMEOUT] = TMR_TIME_MS2TICKS(U2P_NO_ACK_TIME);
                U2P_ucRetryCounter--;

                U2P_pucNextCharToWrite = (uchar*)&U2P_tMessageToSend;	//!< init pointer for transmission
                U2P_ucCharRemainToWrite = U2P_tMessageToSend.tMsgHeader.ucDataLen + 4;	//!< include header

                TMR_uiTimer[TMR_U2P_TRANSMIT_TIME_OUT] = TMR_TIME_MS2TICKS(U2P_BYTE_INTERVAL_TIME);     //!< set timeout for transmit

                U2P_vCrcInit();	//!<reset the crc calculation

                SBUF = *U2P_pucNextCharToWrite;     //!< first byte
                U2P_vAddByteToCrcMAXIM(*U2P_pucNextCharToWrite);

                U2P_pucNextCharToWrite++;
                U2P_ucCharRemainToWrite--;
                U2P_tTaskState = U2P_TASK_SEND_DATA;
            }

        }
        else
        {
            //!< wait for idle
        }
    }
    else
    {
        /**idle state ?*/
        U2P_vCheckTransmitTask();
    }
}

static void U2P_vCheckTransmitTask(void)
{
    uchar ucMessageIndex;
    uchar ucMsgID = 0;

    if (U2P_ucNextTaskIndex != U2P_ucLatestTaskIndex)   //!< queue not empty
    {
        if (U2P_ucTransmitTaskQueue[U2P_ucNextTaskIndex] != 0)  //!< check validation
        {
            ucMsgID = U2P_ucTransmitTaskQueue[U2P_ucNextTaskIndex]; //!< read task from queue
            U2P_ucTransmitTaskQueue[U2P_ucNextTaskIndex] = 0;   //!< reset value
            U2P_ucNextTaskIndex = (++U2P_ucNextTaskIndex) % U2P_TASK_NUM;

            /**init task*/
            ucMessageIndex = U2P_ucGetTransmitMessageIndex(ucMsgID);
            if (ucMessageIndex >= U2P_TRANSMIT_MSG_TBL_SIZE)
            {
                return; //!< no message found
            }

            /**head*/
            U2P_tMessageToSend.tMsgHeader.ucHead = U2P_MSG_HEAD_DEVICE;
            /**sequence */
            U2P_tMessageToSend.tMsgHeader.ucMsgSeqValue = (ucMsgID != U2P_MSG_ID_ACK) ? (++U2P_ucMsgSeqValue) : U2P_ucMsgSeqValue;
            /**msg id*/
            U2P_tMessageToSend.tMsgHeader.ucMsgID = ucMsgID;
            /**data length & data*/
            U2P_tMessageToSend.tMsgHeader.ucDataLen = 0;    //!< reset value
            U2P_tTransmitMsgTbl[ucMessageIndex].pServiceFunc(&U2P_tMessageToSend);

            /**set retry counter based on ack flag */
            if (U2P_ACK == U2P_tTransmitMsgTbl[ucMessageIndex].ucAckRequire)
            {
                U2P_ucRetryCounter = U2P_NO_ACK_RETRY_TIMES + 1;
            }
            else
            {
                U2P_ucRetryCounter = 1;
            }

            TMR_uiTimer[TMR_U2P_MSG_NO_ACK_TIMEOUT] = 0;
        }
        else
        {
            U2P_ucNextTaskIndex = (++U2P_ucNextTaskIndex) % U2P_TASK_NUM;   //!< skip empty task
        }
    }
}

void U2P_vTransmitMessage(uchar ucMsgID)
{
    /**add task into queue*/
    U2P_ucTransmitTaskQueue[U2P_ucLatestTaskIndex] = ucMsgID;
    U2P_ucLatestTaskIndex = (++U2P_ucLatestTaskIndex) % U2P_TASK_NUM;   //!< point to next index
}

static void U2P_vTransmitAck(void)
{
    /**head*/
    U2P_tAckToSend.tMsgHeader.ucHead = U2P_MSG_HEAD_DEVICE;
    /**sequence */
    U2P_tAckToSend.tMsgHeader.ucMsgSeqValue = U2P_ucMsgSeqValue;
    /**msg id*/
    U2P_tAckToSend.tMsgHeader.ucMsgID = U2P_MSG_ID_ACK;
    /**data length & data*/
    U2P_tAckToSend.tMsgHeader.ucDataLen = 0;    //!< reset value
    U2P_tTransmitMsgTbl[0].pServiceFunc(&U2P_tAckToSend);

    TMR_uiTimer[TMR_U2P_MSG_NO_ACK_TIMEOUT] = 0;
    U2P_ucTransAckFirst = 1;
}

void U2P_vInit(void)
{
#if (CPU_TYPE==CPU_SH79F3283)
    SCON = 0x50;  //!< mode1, Receive enable
    TI = 0; //!< clear flag
    RI = 0; //!< clear flag

    /**Config Timr2 as baudrate clk generator*/
    T2CON = 0x30;   //!< RCLK = 1, TCLk = 1
    T2MOD = 0x00;   //!< Fsys as timer2 clk source, counter increase mode
    /**reload register*/
    /**9600 = Fsys / (32 * (65536-[RCAP2H, RCAP2L]))*/
    RCAP2L = 0xE6;
    RCAP2H = 0xFF;

    T2CON |= 0x04;  //!< start timer2

    IEN0 = 0x10;  //!< enable EUART0 isr
#elif (CPU_TYPE==CPU_SH79F328)
    /**Baudrate = 9600*/
    /** Fomula: (int)X=FSY/(16*Baudrate) = 156.25 ;  SBRT=32768-X = 32612 = 0x7F64;   SFINE=(FSY/Baudrate)-16*X = 4;   FSY=24M*/
    SBRTH = 0x64;
    SBRTL = 0x7F;
    //SFINE = 0x04;
    SCON = 0x50;  //!< mode1, Receive enable, 
    IEN0 = 0x10;  //!< enable EUART0 isr
#else

#endif
}
									   
void U2P_vTxEventHandler(void)
{
    TMR_uiTimer[TMR_U2P_TRANSMIT_TIME_OUT] = TMR_TIME_MS2TICKS(U2P_BYTE_INTERVAL_TIME); //!< set timeout for transmit

    if (U2P_TASK_SEND_DATA == U2P_tTaskState)
    {
        if (U2P_ucCharRemainToWrite > 0)
        {
            SBUF = *U2P_pucNextCharToWrite;

            U2P_vAddByteToCrcMAXIM(*U2P_pucNextCharToWrite);	//!< update calculated CRC

            U2P_pucNextCharToWrite++;
            U2P_ucCharRemainToWrite--;
            return;
        }
        else
        {
            /**send CRC*/
            U2P_ucCrc = U2P_ucGetCalculatedCrc();
            *U2P_pucNextCharToWrite = U2P_ucCrc;
            SBUF = *U2P_pucNextCharToWrite;

            if (U2P_ucTransAckFirst)
            {
                U2P_ucTransAckFirst = 0;
                TMR_uiTimer[TMR_U2P_MSG_NO_ACK_TIMEOUT] = TMR_TIME_MS2TICKS(U2P_BYTE_INTERVAL_TIME);
            }


            /**transmit finished, return to idle state*/
            U2P_vEnterIdleState();
        }
    }
}                     

void U2P_vRxEventHandler(uchar ucData)
{
    uchar ucReceivedCharacter;

    ucReceivedCharacter = ucData;
    ring_buffer_queue(&U2P_tRingBuffer, ucReceivedCharacter);	//!< add elements to buffer, one at a time
}   