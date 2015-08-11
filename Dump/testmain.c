#include <zc_common.h>
#include <uip.h>
#include <uiplib.h>
#include <zc_message_queue.h>
#include <iot_tcpip_interface.h>
#include <zc_protocol_interface.h>
#include <zc_protocol_controller.h>
#include <windows.h>
#include <zc_sec_engine.h>
#include "rsa_genkey.h"

u8 g_u8DumpCloudMsg[102400];
extern MSG_Buffer g_struRecvBuffer;
extern MSG_Queue  g_struRecvQueue;
IOT_ADAPTER   	IoTpAd;

void SimRecvMsg(u8 u8Code, u8 *pu8msg, u16 u16Datalen)
{
    ZC_SecHead *pstruMsg;
    ZC_MessageHead *pstruHead;
    u32 u32Index;

    pstruMsg = (ZC_SecHead *)uip_appdata;
    pstruMsg->u8SecType = ZC_SEC_ALG_NONE;
    pstruMsg->u16TotalMsg = ZC_HTONS(u16Datalen + sizeof(ZC_MessageHead));
    
    pstruHead = (ZC_MessageHead*)(pstruMsg + 1);
    pstruHead->MsgCode = u8Code;

    pstruHead->Payloadlen = ZC_HTONS(u16Datalen);
    memcpy(pstruHead + 1, pu8msg, u16Datalen);
    
    uip_flags = UIP_NEWDATA;
    uip_len = u16Datalen + sizeof(ZC_MessageHead) + sizeof(ZC_SecHead);
}

void logicTest()
{
    u32 u32AccessOk = 0;
    MT_Init();


    while(1)
    {
        ZC_Printf("u32Access status = %d\n", u32AccessOk);
        if (1 == u32AccessOk)
        {
            MT_WakeUp();
            
        }
        else if (0 == u32AccessOk)
        {
           
        }
        else
        {
            
        }
        MT_TimerExpired();
        
        if (5 == u32AccessOk)
        {
            uip_flags = UIP_TIMEDOUT;
        }
        /*else if (8 == u32AccessOk)
        {
            uip_flags = UIP_CONNECTED;
        }
        else if (u32AccessOk == 9)
        {
            SimRecvMsg(ZC_CODE_HANDSHAKE_2);
        }
        else if (u32AccessOk == 10)
        {
            SimRecvMsg(ZC_CODE_HANDSHAKE_4);
        }*/
        if (g_struProtocolController.u8MainState == PCT_STATE_WAIT_ACCESSRSP)
        {
            SimRecvMsg(ZC_CODE_HANDSHAKE_2, g_struProtocolController.RandMsg, sizeof(ZC_HandShakeMsg2));
        }
        if (g_struProtocolController.u8MainState == PCT_STATE_WAIT_MSG4)
        {
            //SimRecvMsg(ZC_CODE_HANDSHAKE_4, g_struProtocolController.RandMsg, sizeof(ZC_HandShakeMsg4));
        }
        MT_CloudAppCall();
        MT_Run();
        u32AccessOk++;
    }

}

void testqueue()
{
    u8 u8TestMsg[100];
    u32 u32Index;
    u32 u32RetVal;
    u8 *pu8Msg;
    
    MSG_Queue struTestqueue;
    
    for (u32Index = 0; u32Index < 100; u32Index++)
    {
        u8TestMsg[u32Index] = u32Index;
    }
    MSG_InitQueue(&struTestqueue);
    
    for (u32Index = 0; u32Index < 3; u32Index++)
    {
        pu8Msg = MSG_PopMsg(&struTestqueue);
        if (NULL == pu8Msg)
        {
            ZC_Printf("No Msg\n", u32Index);
        }
        else
        {
            ZC_Printf("msg %d is pop\n", *pu8Msg);
        }
    }
    
    for (u32Index = 0; u32Index < 1; u32Index++)
    {
        u32RetVal = MSG_PushMsg(&struTestqueue, &u8TestMsg[u32Index]);
        if (ZC_RET_ERROR == u32RetVal)
        {
            //Printf_High("Msg Queue is full, Num %d drop\n", u32Index);
        }
        else
        {
            ZC_Printf("msg %d is pushed\n", u32Index);
        }
    }
    
    for (u32Index = 0; u32Index < 2; u32Index++)
    {
        pu8Msg = MSG_PopMsg(&struTestqueue);
        if (NULL == pu8Msg)
        {
            //Printf_High("No Msg\n", u32Index);
        }
        else
        {
            ZC_Printf("msg %d is pop\n", *pu8Msg);
        }
    }
    
    for (u32Index = 0; u32Index < 100; u32Index++)
    {
        u32RetVal = MSG_PushMsg(&struTestqueue, &u8TestMsg[u32Index]);
        if (ZC_RET_ERROR == u32RetVal)
        {
            //Printf_High("Msg Queue is full, Num %d drop\n", u32Index);
        }
        else
        {
            ZC_Printf("msg %d is pushed\n", u32Index);
        }
    }

    for (u32Index = 0; u32Index < 3; u32Index++)
    {
        pu8Msg = MSG_PopMsg(&struTestqueue);
        if (NULL == pu8Msg)
        {
            //Printf_High("No Msg\n", u32Index);
        }
        else
        {
            ZC_Printf("msg %d is pop\n", *pu8Msg);
        }
    }
} 
void testsendcloud()
{
    u32 u32Index;
    ZC_SecHead struHead;
    MT_Init();
    
    for (u32Index = 0; u32Index < 8; u32Index++)
    {
        g_u8DumpCloudMsg[u32Index] = u32Index;
    }
    struHead.u8SecType = ZC_SEC_ALG_NONE;
    struHead.u16TotalMsg = ZC_HTONS(8);
    PCT_SendMsgToCloud(&struHead, g_u8DumpCloudMsg);
  
    struHead.u8SecType = ZC_SEC_ALG_NONE;
    struHead.u16TotalMsg = ZC_HTONS(52);
    PCT_SendMsgToCloud(&struHead, g_u8DumpCloudMsg);

    MT_SendDataToCloud(&g_struProtocolController.struCloudConnection);
    MT_SendDataToCloud(&g_struProtocolController.struCloudConnection);

}

void testrecvbuffer()
{
    ZC_SecHead *pstruMsg;
    ZC_MessageHead *pstruHead;
    u32 u32Index;
    u16 u16Len;

    MT_Init();
    
    pstruMsg = (ZC_SecHead *)g_u8DumpCloudMsg;
    pstruHead = (ZC_MessageHead*)(pstruMsg+1);
    
    u16Len = 68;
    pstruMsg->u16TotalMsg  = ZC_HTONS(u16Len+sizeof(ZC_MessageHead));
    pstruHead->Payloadlen = ZC_HTONS(u16Len);
    
    for (u32Index = 0; u32Index < u16Len; u32Index++)
    {
        g_u8DumpCloudMsg[u32Index + sizeof(ZC_MessageHead) + sizeof(ZC_SecHead)] = u32Index;
    }
    
    for (u32Index = 0; u32Index < 210; u32Index++)
    {
        MT_RecvDataFromCloud((u8 *)(g_u8DumpCloudMsg + 10 * u32Index), 1000);
    }
    
    ZC_Printf("status = %d, len = %d\n",g_struRecvBuffer.u8Status, g_struRecvBuffer.u32Len);
    
    for (u32Index = 0; u32Index < g_struRecvBuffer.u32Len; u32Index++)
    {
        ZC_Printf("%02X ", g_struRecvBuffer.u8MsgBuffer[u32Index]);
    }
    
    ZC_Printf("\n");
    
    g_struProtocolController.u8keyRecv = 1;
    
    PCT_HandleEvent(&g_struProtocolController);
    
    while(1)
    {
        MT_TimerExpired();
        if (g_struProtocolController.u8ReSendMoudleNum == 2)
        {
            PCT_HandleMoudleEvent(0, 0, (u8 *)(g_u8DumpCloudMsg), 100);
        }
        
    }
}


void readcfg()
{
    u8 *pu8Test;
    MT_Init();
    testread();
}
extern IOT_USR_CFG Usr_Cfg;
void TestSendRegisterMsg()
{
    u32 u32Index;
    ZC_MessageHead *pstruMsg;
    ZC_RegisterReq *pstruRegisterReq;
    u8 u8DeviceId[8] = { 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, };
    u8 u8Domain[8] = { 0, 0, 0, 0, 0, 110, 0, 122 };
    u8 u8ATCmd[1024] = {0,};

    u8ATCmd[0] = 0x02;
    u8ATCmd[1] = 0x03;
    u8ATCmd[2] = 0x04;
    u8ATCmd[3] = 0x05;
    for (u32Index = 0; u32Index < sizeof(ZC_MessageHead); u32Index++)
    {
        u8ATCmd[4 + u32Index] = u32Index;
    }
    pstruMsg = (ZC_MessageHead*)(u8ATCmd + 4);
    pstruMsg->Payloadlen = ZC_HTONS(sizeof(ZC_RegisterReq));
    pstruMsg->MsgCode = 7;
    pstruMsg->OptNum = 0;

    pstruRegisterReq = (ZC_RegisterReq*)(pstruMsg + 1);

    memcpy(pstruRegisterReq->u8Domain, u8Domain, 8);
    memcpy(pstruRegisterReq->u8DeviceId, u8DeviceId, 8);

    ZC_RecvDataFromMoudle(u8ATCmd + 4, sizeof(ZC_MessageHead)+sizeof(ZC_RegisterReq));
    PCT_SendCloudAccessMsg1(&g_struProtocolController);
}

void testupdate()
{
    u8 u8Buffer[1024];
    extern u8 u8File[1024];
    u32 u32Index;
    for (u32Index = 0; u32Index < 180; u32Index++)
    {
        u8Buffer[u32Index] = (u8)u32Index;
    }
    MT_FirmwareUpdate(u8Buffer, 0, 60);
    MT_FirmwareUpdate(u8Buffer + 60, 60, 60);
    MT_FirmwareUpdate(u8Buffer + 120, 120, 60);
    
    ZC_TraceData(u8File, 180);
    
}

void testparseropt()
{
    u8 u8TestData[] = {
        0x00, 0x00, 0x00, 0x02,
        0x00, 0x78, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x08,
        0x09, 0x09, 0x09, 0x09,
        0x09, 0x09, 0x09, 0x09,
        0x00, 0x01, 0x00, 0x04,
        0x00, 0x00, 0x00, 0x01,
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b,
        0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b,
        0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23,
        0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b,
        0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33,
        0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b,
        0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x41, 0x42, 0x43,
        0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4a, 0x4b,
        0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53,
        0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5a, 0x5b,
        0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63
    };

    ZC_RecvDataFromMoudle(u8TestData, 128);
}
void main()
{
/*
    u16 padding;
    memcpy(&IoTpAd.UsrCfg , &Usr_Cfg , sizeof(IOT_USR_CFG));

    rsaoffline();
*/
    MT_Init();

    testparseropt();
}