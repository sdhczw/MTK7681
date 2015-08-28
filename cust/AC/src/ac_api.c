/**
******************************************************************************
* @file     ZC_api.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief   
******************************************************************************
*/

#include <zc_common.h>
#include <zc_protocol_interface.h>
#include <zc_protocol_controller.h>
#include <ac_hal.h>
#include <zc_module_interface.h>

u8 g_u8DevMsgBuildBuffer[300];
/*************************************************
* Function: AC_BuildOption
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_BuildOption(AC_OptList *pstruOptList, u8 *pu8OptNum, u8 *pu8Buffer, u16 *pu16Len)
{
    //可选字段组包函数
    ZC_MessageOptHead *pstruOpt;
    u8 u8OptNum = 0;
    u8 u16OptLen = 0;
    
    *pu16Len = u16OptLen;
    *pu8OptNum = u8OptNum;
    
    
    if (NULL == pstruOptList)
    {
        return;
    }
    
    pstruOpt = (ZC_MessageOptHead *)pu8Buffer;

    /*add opt, if it exist*/
    if (NULL != pstruOptList->pstruTransportInfo)
    {
        pstruOpt->OptCode = ZC_HTONS(ZC_OPT_TRANSPORT);
        pstruOpt->OptLen = ZC_HTONS(sizeof(ZC_TransportInfo));
        memcpy((u8*)(pstruOpt + 1), (u8*)pstruOptList->pstruTransportInfo, sizeof(ZC_TransportInfo));

        u8OptNum++;
        u16OptLen += sizeof(ZC_MessageOptHead) + sizeof(ZC_TransportInfo);
        pstruOpt = (ZC_MessageOptHead *)(pu8Buffer + u16OptLen);        
    }
    

    if (NULL != pstruOptList->pstruSsession)
    {
        pstruOpt = (ZC_MessageOptHead *)pu8Buffer;
        pstruOpt->OptCode = ZC_HTONS(ZC_OPT_SSESSION);
        pstruOpt->OptLen = ZC_HTONS(sizeof(ZC_SsessionInfo));
        memcpy((u8*)(pstruOpt + 1), (u8*)pstruOptList->pstruSsession, sizeof(ZC_SsessionInfo));

        u8OptNum++;
        u16OptLen += sizeof(ZC_MessageOptHead) + sizeof(ZC_SsessionInfo);
        pstruOpt = (ZC_MessageOptHead *)(pu8Buffer + u16OptLen);        
    }    

    
    *pu16Len = u16OptLen;
    *pu8OptNum = u8OptNum;
    return;
}

/*************************************************
* Function: AC_BuildMessage
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_BuildMessage(u8 u8MsgCode, u8 u8MsgId, 
    u8 *pu8Payload, u16 u16PayloadLen,
    AC_OptList *pstruOptList,
    u8 *pu8Msg, u16 *pu16Len)
{
    //协议组包函数
    ZC_MessageHead *pstruMsg = NULL;
    u16 u16OptLen = 0;
    u16 crc = 0;
    
    pstruMsg = (ZC_MessageHead *)pu8Msg;
    pstruMsg->MsgCode = u8MsgCode;
    pstruMsg->MsgId = u8MsgId;  

    pstruMsg->Version = 0;

    AC_BuildOption(pstruOptList, &pstruMsg->OptNum, (pu8Msg + sizeof(ZC_MessageHead)), &u16OptLen);
    memcpy((pu8Msg + sizeof(ZC_MessageHead) + u16OptLen), pu8Payload, u16PayloadLen);

    /*updata len*/
    pstruMsg->Payloadlen = ZC_HTONS(u16PayloadLen + u16OptLen);

    /*calc crc*/
    crc = crc16_ccitt((pu8Msg + sizeof(ZC_MessageHead)), (u16PayloadLen + u16OptLen));
    pstruMsg->TotalMsgCrc[0]=(crc&0xff00)>>8;
    pstruMsg->TotalMsgCrc[1]=(crc&0xff);


    *pu16Len = (u16)sizeof(ZC_MessageHead) + u16PayloadLen + u16OptLen;
}

/*************************************************
* Function: AC_SendDeviceStart
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_SendDeviceStart()
{
    //设备启动消息
    u16 u16DateLen;

    AC_BuildMessage(ZC_CODE_EQ_BEGIN, 0, 
        NULL, 0,        /*payload+payload len*/
        NULL,
        g_u8MsgBuildBuffer, &u16DateLen);
    
    AC_SendMessage(g_u8MsgBuildBuffer, u16DateLen);
}

/*************************************************
* Function: AC_SendMessage
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/

void AC_SendMessage(u8 *pu8Msg, u16 u16DataLen)
{
    ZC_RecvDataFromMoudle( pu8Msg,  u16DataLen);
}

/*************************************************
* Function: AC_SendRestMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_SendRestMsg(AC_OptList *pstruOptList)
{
    //wifi密码重置
    u16 u16DateLen;
    AC_BuildMessage(ZC_CODE_REST, 0, 
        NULL, 0,        /*payload+payload len*/
        pstruOptList,
        g_u8MsgBuildBuffer, &u16DateLen);
    
    AC_SendMessage(g_u8MsgBuildBuffer, u16DateLen);
}

/*************************************************
* Function: AC_ParseOption
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_ParseOption(ZC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u16 *pu16OptLen)
{
    //解析Option
    u8 u8OptNum;
    ZC_MessageOptHead *pstruOptHead;
    u16 u16Offset;

    u16Offset = sizeof(ZC_MessageHead);
    pstruOptHead = (ZC_MessageOptHead *)((u8*)pstruMsg + u16Offset);
    *pu16OptLen = 0;

    for (u8OptNum = 0; u8OptNum < pstruMsg->OptNum; u8OptNum++)
    {
        switch (ZC_HTONS(pstruOptHead->OptCode))
        {
            case ZC_OPT_TRANSPORT:
                pstruOptList->pstruTransportInfo = (ZC_TransportInfo *)(pstruOptHead + 1);
                break;
            case ZC_OPT_SSESSION:
                pstruOptList->pstruSsession = (ZC_SsessionInfo *)(pstruOptHead + 1);            
                break;
        }
        *pu16OptLen += sizeof(ZC_MessageOptHead) + ZC_HTONS(pstruOptHead->OptLen);
        pstruOptHead = (ZC_MessageOptHead *)((u8*)pstruOptHead + sizeof(ZC_MessageOptHead) + ZC_HTONS(pstruOptHead->OptLen));

    }
}

/*************************************************
* Function: AC_SendDeviceRegsiterWithMac
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_SendDeviceRegsiterWithMac(u8 *pu8EqVersion, u8 *pu8ModuleKey, u64 u64Domain)
{
    //统一入库，设备注册请求，设备id无效，使用wifi的mac地址作为设备id，所有固件使用一个私钥
    ZC_ExtRegisterReq struExtReg;
    u16 u16DateLen;

    struExtReg.struExtMessageHead.ExtMsgCode = ZC_CODE_EXT_REGSITER;
    memcpy(struExtReg.struRegReq.u8EqVersion, pu8EqVersion, ZC_EQVERSION_LEN);
    memcpy(struExtReg.struRegReq.u8ModuleKey, pu8ModuleKey, ZC_MODULE_KEY_LEN); 
    memcpy(struExtReg.struRegReq.u8Domain, &u64Domain, ZC_DOMAIN_LEN); 

    AC_BuildMessage(ZC_CODE_EXT, 0, 
        (u8*)&struExtReg, sizeof(ZC_ExtRegisterReq),   /*payload+payload len*/
        NULL,
        g_u8MsgBuildBuffer, &u16DateLen);
    
    AC_SendMessage(g_u8MsgBuildBuffer, u16DateLen);
}

/*************************************************
* Function: AC_RecvMessage
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_RecvMessage(ZC_MessageHead *pstruMsg)
{
    //串口收到消息后，需要调用该接口处理消息。
    AC_OptList struOptList;
    u16 u16OptLen = 0;
    u8 *pu8Playload = NULL;

    struOptList.pstruSsession = NULL;
    struOptList.pstruTransportInfo = NULL;
    
    /*Parser Option*/
    AC_ParseOption(pstruMsg, &struOptList, &u16OptLen);
    pu8Playload = (u8*)pstruMsg + u16OptLen + sizeof(ZC_MessageHead);
    switch(pstruMsg->MsgCode)
    {
        //事件通知类消息
        case ZC_CODE_EQ_DONE:
        case ZC_CODE_WIFI_CONNECTED:
        case ZC_CODE_WIFI_DISCONNECTED:
        case ZC_CODE_CLOUD_CONNECTED:
        case ZC_CODE_CLOUD_DISCONNECTED:
            AC_DealNotifyMessage(pstruMsg, &struOptList, pu8Playload);
            break;
        //设备事件类消息    
        default:
            if (pstruMsg->MsgCode >= ZC_CODE_EVENT_BASE)
            {
                AC_DealEvent(pstruMsg, &struOptList, pu8Playload);
            }
            break;            
    }
}




