#ifndef  __ZC_UIPDMUP_H__ 
#define  __ZC_UIPDMUP_H__

typedef unsigned short u16_t;
struct uip_conn
{
    unsigned int fd;
    unsigned int lport;
};

typedef struct
{
    unsigned int fd;
    unsigned int lport;
}UIP_UDP_CONN;

typedef unsigned short uip_ip4addr_t[2];
typedef uip_ip4addr_t uip_ipaddr_t;

extern u8 uip_appdata[10240];
extern u8 uip_flags;
extern u16 uip_len;
#define UIP_TCP_MSS 546
#define TRUE 1
#define WIFI_STATE_INIT 1
extern struct uip_conn *uip_conn;   
extern uip_ipaddr_t uip_hostaddr;
typedef struct _MLME_STRUCT {
    int ATSetSmnt;
}MLME_STRUCT;
#define uip_ipaddr1(addr) (ZC_HTONS(((u16_t *)(addr))[0]) >> 8)
#define uip_ipaddr2(addr) (ZC_HTONS(((u16_t *)(addr))[0]) & 0xff)
#define uip_ipaddr3(addr) (ZC_HTONS(((u16_t *)(addr))[1]) >> 8)
#define uip_ipaddr4(addr) (ZC_HTONS(((u16_t *)(addr))[1]) & 0xff)

#endif

