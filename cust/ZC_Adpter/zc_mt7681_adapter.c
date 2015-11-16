/**
******************************************************************************
* @file     MT7681Adapter.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Event
******************************************************************************
*/
#include <zc_protocol_controller.h>
#include <uiplib.h>
#include <uip.h>
#include <Random.h>
#include <xip_ovly.h>
#include <uip_timer.h>
#include <zc_timer.h>
#include <iot_api.h>
#include <zc_module_interface.h>
#include <ac_api.h>



extern PTC_ProtocolCon  g_struProtocolController;
PTC_ModuleAdapter g_struMt7681Adapter;

MSG_Buffer g_struRecvBuffer;
MSG_Buffer g_struRetxBuffer;
MSG_Buffer g_struClientBuffer;


MSG_Queue  g_struRecvQueue;
MSG_Buffer g_struSendBuffer[MSG_BUFFER_SEND_MAX_NUM];
MSG_Queue  g_struSendQueue;

u8 g_u8MsgBuildBuffer[MSG_BULID_BUFFER_MAXLEN];
u8 g_u8ClientSendLen = 0;

struct timer g_struMtTimer[ZC_TIMER_MAX_NUM];
u32 g_u32BcSleepCount = 0;
u8 g_u8UseWifiDevFlag = 0;

u16 g_u16TcpMss;
extern IOT_ADAPTER   	IoTpAd;
extern IOT_USR_CFG Usr_Cfg;

u16 g_u16LocalPort;
u16 g_u16LocalListenPort;
extern char ATCmdPrefixAT[];
extern MLME_STRUCT *pIoTMlme;


#ifndef ZC_OFF_LINETEST
/*************************************************
* Function: rand
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
int rand()
{    
    return apiRand();
}
#endif

/*************************************************
* Function: HF_WriteDataToFlash
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_WriteDataToFlash(u8 *pu8Data, u16 u16Len)
{
   memcpy(&IoTpAd.UsrCfg,pu8Data,u16Len);
   memcpy(&Usr_Cfg, &IoTpAd.UsrCfg, sizeof(IOT_USR_CFG));            
   reset_usr_cfg(TRUE);
}

/*************************************************
* Function: HF_WriteDataToFlash
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_ReadDataFromFlash()
{

	load_usr_cfg();

    if (ZC_MAGIC_FLAG == *((u32*)IoTpAd.UsrCfg.ConfigInfor))
    {   
        memcpy((char *)(&g_struZcConfigDb), &IoTpAd.UsrCfg, sizeof(ZC_ConfigDB));
    }
    else
    {
        ZC_Printf("no para, use default\n");
    }
}

/*************************************************
* Function: MT_GetLocalPortNum
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u16 MT_GetLocalPortNum()
{
    return g_u16LocalPort;
}

/*************************************************
* Function: MT_GetLocalListenPortNum
* Description: 
* Author: zw 
* Returns: 
* Parameter: 
* History:
*************************************************/
u16 MT_GetLocalListenPortNum()
{
    return g_u16LocalListenPort;
}

/*************************************************
* Function: MT_TimerExpired
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_TimerExpired()
{
    u8 u8Index;
    u8 u8Status;
    for (u8Index = 0; u8Index < ZC_TIMER_MAX_NUM; u8Index++)
    {   
        TIMER_GetTimerStatus(u8Index, &u8Status);
        if (ZC_TIMER_STATUS_USED == u8Status)
        {
            if (timer_expired(&g_struMtTimer[u8Index]))
            {
                TIMER_StopTimer(u8Index);
                TIMER_TimeoutAction(u8Index);
            }
        }
    }

    if ((g_struProtocolController.u8MainState >= PCT_STATE_WAIT_ACCESSRSP)
    && (PCT_INVAILD_SOCKET != g_struProtocolController.struCloudConnection.u32Socket))
    {
        uip_poll_conn(&uip_conns[g_struProtocolController.struCloudConnection.u32Socket]);
        if (uip_len > 0) 
        {
            ZC_Printf("pull have data %d\n", uip_len);
            uip_arp_out();
            mt76xx_dev_send();
        }
    }
    
}

/*************************************************
* Function: MT_SetTimer
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_SetTimer(u8 u8Type, u32 u32Interval, u8 *pu8TimeIndex)
{
    u8 u8TimerIndex;
    u32 u32Retval;
    u32Retval = TIMER_FindIdleTimer(&u8TimerIndex);
    if (ZC_RET_OK == u32Retval)
    {
        TIMER_AllocateTimer(u8Type, u8TimerIndex, (u8*)&g_struMtTimer[u8TimerIndex]);
        timer_set(&g_struMtTimer[u8TimerIndex], u32Interval);
        *pu8TimeIndex = u8TimerIndex;
    }
    return u32Retval;
}


/*************************************************
* Function: MT_SendDataToMoudle
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_SendDataToMoudle(u8 *pu8Data, u16 u16DataLen)
{
#ifdef ZC_MODULE_DEV    
    AC_RecvMessage((ZC_MessageHead *)pu8Data);
#else  
    u8 u8MagicFlag[4] = {0x02,0x03,0x04,0x05};
    IoT_uart_output(u8MagicFlag, 4);
    IoT_uart_output(pu8Data, u16DataLen);
#endif
    return ZC_RET_OK;
}

/*************************************************
* Function: MT_FirmwareUpdateFinish
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_FirmwareUpdateFinish(u32 u32TotalLen)
{
    spi_flash_CopyApToSta(u32TotalLen); 
    return ZC_RET_OK;       
}
/*************************************************
* Function: MT_FirmwareUpdate
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_FirmwareUpdate(u8 *pu8FileData, u32 u32Offset, u32 u32DataLen)
{
    u8 u8RetVal;
    u32 u32HeadLen = 128;
    u32 u32WritLen = 0;
    u32 u32DataStartOffset = 0;
    u32 u32FlashStartOffset = 0;
    /*use ap flash as backup*/
    if ((u32Offset + u32DataLen) <= u32HeadLen)
    {
        ZC_Printf("%d,%d,%d\n",u32Offset,u32DataLen,u32HeadLen);
        return ZC_RET_OK;
    }
    else if (((u32Offset + u32DataLen) > u32HeadLen) && (u32Offset < u32HeadLen))
    {
        ZC_Printf("1 %d,%d,%d\n",u32Offset,u32DataLen,u32HeadLen);
        u32WritLen =  u32DataLen - (u32HeadLen - u32Offset);  
        u32DataStartOffset = u32HeadLen - u32Offset;
        u32FlashStartOffset = 0;
    }
    else
    {
    
        ZC_Printf("2 %d,%d,%d\n",u32Offset,u32DataLen,u32HeadLen);
        u32WritLen = u32DataLen;  
        u32DataStartOffset = 0;
        u32FlashStartOffset = (u32Offset - u32HeadLen);
    }

    u32HeadLen = 128;
    while(u32WritLen >= u32HeadLen)
    {
        u8RetVal = spi_flash_update_fw(UART_FlASH_UPG_ID_AP_FW, u32FlashStartOffset, pu8FileData + u32DataStartOffset, u32HeadLen);

        ZC_Printf("1 ret = %d, %d, %d,\n", u8RetVal,u32FlashStartOffset,u32DataStartOffset);
        if (0 != u8RetVal)
        {
            return ZC_RET_ERROR;
        }
        
        u32WritLen -= u32HeadLen;
        u32DataStartOffset += u32HeadLen;
        u32FlashStartOffset += u32HeadLen;
    }

    if (u32WritLen > 0)
    {
        u8RetVal = spi_flash_update_fw(UART_FlASH_UPG_ID_AP_FW, u32FlashStartOffset, pu8FileData + u32DataStartOffset, u32WritLen);
        ZC_Printf("2 ret = %d, %d, %d,\n", u8RetVal,u32FlashStartOffset,u32DataStartOffset);
        if (0 != u8RetVal)
        {
            return ZC_RET_ERROR;
        }
    }
    
    return ZC_RET_OK;
}
/*************************************************
* Function: MT_Rest
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_Rest()
{
    g_struZcConfigDb.struSwitchInfo.u32ServerAddrConfig = 0;            
    MT_WriteDataToFlash((u8 *)&g_struZcConfigDb, sizeof(ZC_ConfigDB));
    pIoTMlme->ATSetSmnt = TRUE;
    wifi_state_chg(WIFI_STATE_INIT, 0);                 
}
/*************************************************
* Function: MT_SendDataToNet
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_SendData(u32 u32Fd, u8 *pu8Data, u16 u16DataLen, ZC_SendParam *pstruParam)
{
    if (0 == pstruParam->u8NeedPoll)
    {
        uip_send(pu8Data, u16DataLen);    
    }
    else
    {
        uip_poll_conn(&uip_conns[u32Fd]);

        if (uip_len > 0) 
        {
            ZC_Printf("pull have data %d to client\n", uip_len);
            uip_arp_out();
            mt76xx_dev_send();
        }
    }
}

/*************************************************
* Function: MT_StopTimer
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_StopTimer(u8 u8TimerIndex)
{
}

/*************************************************
* Function: MT_GetMac
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_GetMac(u8 *pu8Mac)
{
    u8 mac[6] = {0};
    spi_flash_read(0x17004, mac, 6);
    ZC_HexToString(pu8Mac,mac,6);
}

/*************************************************
* Function: MT_ConnectToCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_ConnectToCloud(PTC_Connection *pstruConnection)
{
    struct uip_conn *conn=NULL;
    
	//UIP_UDP_CONN *udp_conn=NULL;
    uip_ipaddr_t ip;
    //uip_ipaddr_t broadcastip;
    u16  u16Port;
    u16_t  *ipaddr = NULL;
    ipaddr = &ip;


    if (1 == g_struZcConfigDb.struSwitchInfo.u32ServerAddrConfig)
    {
        ZC_Printf("Connect ip=0x%x,port =%d \n",g_struZcConfigDb.struSwitchInfo.u32ServerIp,g_struZcConfigDb.struSwitchInfo.u16ServerPort); 
        u16Port = g_struZcConfigDb.struSwitchInfo.u16ServerPort;
        u8 *ipaddr =(u8 *) &(g_struZcConfigDb.struSwitchInfo.u32ServerIp);
        uip_ipaddr(&ip,ipaddr[3],ipaddr[2],ipaddr[1],ipaddr[0]);
    }
    else
    { 
       u16Port = pstruConnection->u16Port;
       ipaddr =  resolv_lookup(g_struZcConfigDb.struCloudInfo.u8CloudAddr);
       if(NULL == ipaddr)
       {
           return ZC_RET_ERROR;
       } 
       ZC_Printf("Connect %s\n",g_struZcConfigDb.struCloudInfo.u8CloudAddr);
    }


    if (ZC_CONNECT_TYPE_TCP == pstruConnection->u8ConnectionType)
    {
      	conn = uip_connect((uip_ipaddr_t *)ipaddr, ZC_HTONS(u16Port));
    	if (NULL == conn) 
    	{
    		return ZC_RET_ERROR;
    	}

        if (conn) {
            conn->lport = (u16)rand();
        }

    	pstruConnection->u32Socket = conn->fd;
    	g_u16LocalPort = ZC_HTONS(conn->lport);

    	ZC_Printf("Connection Sokcet = %d, conn->lport = %d\n",conn->fd, g_u16LocalPort);
    }
    else
    {

    }
    return ZC_RET_OK;
}
/*************************************************
* Function: MT_ListenClient
* Description: 
* Author: zw 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_ListenClient(PTC_Connection *pstruConnection)
{
    
	UIP_UDP_CONN *udp_conn=NULL;
    uip_ipaddr_t broadcastip;
    
    ZC_Printf("Listen \n");
    
    if (ZC_CONNECT_TYPE_TCP == pstruConnection->u8ConnectionType)
    {
        uip_listen(ZC_HTONS(pstruConnection->u16Port));
        
    	g_u16LocalListenPort = pstruConnection->u16Port;

    	ZC_Printf("Tcp Listen Port = %d\n", pstruConnection->u16Port);


  	    uip_ipaddr(broadcastip, 255,255,255,255);
        udp_conn = uip_udp_new(&broadcastip, ZC_HTONS(ZC_MOUDLE_BROADCAST_PORT));
        if(udp_conn != NULL) {
            ZC_Printf("setup Bc channel\n");
            uip_udp_bind(udp_conn, ZC_HTONS(ZC_MOUDLE_PORT));
        }
    }
    else
    {

    }
    return ZC_RET_OK;
}

/*************************************************
* Function: MT_Init
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_Init()
{
    g_struMt7681Adapter.pfunConnectToCloud = MT_ConnectToCloud;
    g_struMt7681Adapter.pfunListenClient = MT_ListenClient;
    g_struMt7681Adapter.pfunSendTcpData = MT_SendData;   
    g_struMt7681Adapter.pfunSendUdpData = MT_SendData; 
    g_struMt7681Adapter.pfunRest = MT_Rest;
    g_struMt7681Adapter.pfunUpdate = MT_FirmwareUpdate;  
    g_struMt7681Adapter.pfunUpdateFinish = MT_FirmwareUpdateFinish;
    g_struMt7681Adapter.pfunSendToMoudle = MT_SendDataToMoudle;  

    g_struMt7681Adapter.pfunSetTimer = MT_SetTimer;  
    g_struMt7681Adapter.pfunStopTimer = MT_StopTimer;
    g_struMt7681Adapter.pfunWriteFlash = MT_WriteDataToFlash;
    g_struMt7681Adapter.pfunGetMac = MT_GetMac;

    g_struMt7681Adapter.pfunReboot = Sys_reboot;
    g_struMt7681Adapter.pfunPrintf = Printf_High;
    g_struMt7681Adapter.pfunMalloc = malloc;
    g_struMt7681Adapter.pfunFree = free;
    g_u16TcpMss = UIP_TCP_MSS;
    PCT_Init(&g_struMt7681Adapter);
    MT_ReadDataFromFlash();
    ZC_Printf("MT Init\n");
}

/*************************************************
* Function: MT_BroadcastAppCall
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_BroadcastAppCall()
{
    u16 u16Len;
    
    if (uip_poll())
    {
        ZC_SendBc();
    }

    if (uip_newdata())
    {
        ZC_SendClientQueryReq((char *)uip_appdata, uip_datalen());
    }
}
/*************************************************
* Function: MT_CloudAppCall
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_CloudAppCall()
{
    u32 u32Timer = 0;

    u32Timer = rand();
    u32Timer = (PCT_TIMER_INTERVAL_RECONNECT) * (u32Timer % 10 + 1);
    
    /*Connect Cloud */
    if(uip_connected()) 
    {
        /*Send Access to Cloud*/
        ZC_Printf("Connect to cloud\n");
        ZC_Rand(g_struProtocolController.RandMsg);
        PCT_SendCloudAccessMsg1(&g_struProtocolController);
        return;
    }

    /*Connect Time Out */
    if(uip_timedout() || uip_closed() || uip_aborted()) 
    {
        if(g_struProtocolController.struCloudConnection.u32ConnectionTimes++>20)
        {
           g_struZcConfigDb.struSwitchInfo.u32ServerAddrConfig = 0;
        }
        ZC_Printf("uip flag = %d, timer = %d, Close Connection\n",uip_flags,u32Timer);
        PCT_ReconnectCloud(&g_struProtocolController, u32Timer);
        PCT_SendNotifyMsg(ZC_CODE_CLOUD_DISCONNECTED);
        uip_abort();
    }

    if(uip_acked()) 
    {
    }
    
    if(uip_newdata()) 
    {
        MSG_RecvDataFromCloud((char *)uip_appdata, uip_datalen());
    }
    
    if(uip_poll()) 
    {
        if (PCT_STATE_DISCONNECT_CLOUD == g_struProtocolController.u8MainState)
        {
            ZC_Printf("disconnect, timer = %d\n", g_struProtocolController.u8MainState, u32Timer);
            uip_abort();
            PCT_ReconnectCloud(&g_struProtocolController, u32Timer);
        }
        else
        {
            MSG_SendDataToCloud(&g_struProtocolController.struCloudConnection);
        }
    }
}

/*************************************************
* Function: MT_ClientAppCall
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_ClientAppCall()
{
    if(uip_connected()) 
    {
        ZC_ClientConnect(uip_conn->fd);
    }

    if(uip_newdata()) 
    {
        ZC_RecvDataFromClient(uip_conn->fd, (char *)uip_appdata, uip_datalen());
    }
    
    if(uip_poll()) 
    {
        if (g_u8ClientSendLen > 0)
        {
            uip_send(g_u8MsgBuildBuffer, g_u8ClientSendLen);
        }
    }

    if(uip_closed())
    {
        ZC_ClientDisconnect(uip_conn->fd);
    }
}

/*************************************************
* Function: MT_WakeUp
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_WakeUp()
{
    PCT_WakeUp();
    ZC_StartClientListen();
}
/*************************************************
* Function: MT_Sleep
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_Sleep()
{
    PCT_Sleep();
}

/*************************************************
* Function: MT_Run
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_Run()
{
    if(PCT_STATE_WAIT_ACCESS!=g_struProtocolController.u8MainState)
    {
        PCT_Run();
    }
}

/******************************* FILE END ***********************************/


