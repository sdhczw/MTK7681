#include <zc_common.h>
#include <uip.h>
#include <uiplib.h>
#include <iot_tcpip_interface.h>
#include <time.h>
u8 uip_appdata[10240];
struct uip_conn g_DumpConn;
struct uip_conn g_uip_conn;
struct uip_conn *uip_conn = &g_uip_conn;
uip_ipaddr_t uip_hostaddr;
UIP_UDP_CONN g_DmupUdpConn;
u8 uip_flags = 0;
u16 uip_len = 0;
u8 g_u8Ipaddr[4]={127,0,0,1};
extern u8 g_u8DumpCloudMsg[102400];
char ATCmdPrefixAT[] = "AT#";
MLME_STRUCT g_IoTMlme;
MLME_STRUCT *pIoTMlme = &g_IoTMlme;

int iot_send(u8 fd, u8 *buf, u16 len)
{
    uip_flags = UIP_NEWDATA;
    return len;
}

int iot_udp_send(u8 fd, u8 *buf, u16 len, u8 *rip, u16 rport)
{
    uip_flags = UIP_NEWDATA;
    return len;
}

int IoT_uart_output(u8 *msg, u16 count)
{
    return count;
}

struct uip_conn * uip_connect(uip_ipaddr_t *ripaddr, u16 rport)
{
    static unsigned int ConnectTime = 0;

    ConnectTime++;

    if (2 == ConnectTime)
    {
        uip_flags = UIP_CONNECTED;
    }

    
    return &g_DumpConn;
}

UIP_UDP_CONN *uip_udp_new(uip_ipaddr_t *ripaddr, u16 rport)
{
    return &g_DmupUdpConn;
}
void uip_send(const void *data, u16_t len)
{

}

u16 *resolv_lookup(char *name)
{
    return (u16*)g_u8Ipaddr;    
}

void AES_CBC_Encrypt(
    u8 PlainText[],
    u32 PlainTextLength,
    u8 Key[],
    u32 KeyLength,
    u8 IV[],
    u32 IVLength,
    u8 CipherText[],
    u32 *CipherTextLength)
{
}

void AES_CBC_Decrypt(
    u8 PlainText[],
    u32 PlainTextLength,
    u8 Key[],
    u32 KeyLength,
    u8 IV[],
    u32 IVLength,
    u8 CipherText[],
    u32 *CipherTextLength)
{
}

u32 clock_time()
{
    return clock();
}
void
timer_set(struct timer *t, clock_time_t interval)
{
    t->interval = interval;
    t->start = clock_time();
}

void
timer_reset(struct timer *t)
{
    t->start += t->interval;
}

void
timer_restart(struct timer *t)
{
    t->start = clock_time();
}

int
timer_expired(struct timer *t)
{
    return (clock_time_t)(clock_time() - t->start) >= (clock_time_t)t->interval;
}
void
uip_arp_out(void)
{}
void
mt76xx_dev_send(void)
{}
u32 spi_flash_write(u32 addr, u8 *data, u16 len)
{
    memcpy(g_u8DumpCloudMsg + addr, data, len);
    return 0;
}
u32 spi_flash_read(u32 addr, u8 *data, u16 len)
{
    memcpy(data, g_u8DumpCloudMsg + addr, len);
    return 0;
}
u16 IoT_parse_ATcommand(u8 *pcmd_buf, u16 at_cmd_len)
{
    u16 ret_code = 0;   

    pcmd_buf[at_cmd_len] = '\0';
    
    printf("%s\n", pcmd_buf);
    return 1;
}
u8 u8File[1024];
u8 spi_flash_update_fw(u8 type, u32 offset, u8 *pdata, u16 len)
{
    printf("offset = %d, len = %d, pdata = %d\n", offset, len, pdata);
    memcpy(u8File + offset, pdata, len);
    
    return 0;
}
void IoT_Xmodem_Update_FW_Start()
{

}
void IoT_Xmodem_Update_FW_Stop()
{

}
u32 spi_flash_CopyApToSta(u32 len)
{
        return 0;
}
void wifi_state_chg(u8 iStateMachine, u8 iSubState)
{}
void uip_listen(u16 port)
{}
unsigned short crc16_ccitt(const unsigned char *buf, unsigned int len)
{
    return 100;
}
