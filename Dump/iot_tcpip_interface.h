#ifndef  __ZC_IOT_TCPIP_INTERFACE_H__ 
#define  __ZC_IOT_TCPIP_INTERFACE_H__
//#define Printf_High printf
typedef int clock_time_t;
#define CLOCK_CONF_SECOND 1000

struct timer {
    clock_time_t start;
    clock_time_t interval;
};

void timer_set(struct timer *t, clock_time_t interval);
void timer_reset(struct timer *t);
void timer_restart(struct timer *t);
int timer_expired(struct timer *t);

typedef struct _IOT_USER_CFG_{
    u8 VendorName[32];
    u8 ProductType[32];
    u8 ProductName[32];
    u8 ProductKey[112];
    u8 CloudKey[36];
    u8 CloudAddr[20];
}IOT_USR_CFG;


typedef struct _IOT_ADAPTER{
    IOT_USR_CFG		UsrCfg;
    u8 			flash_rw_buf[256];

}IOT_ADAPTER;
#define PRODUCT_INFO_STORED				(0x23)

#define FLASH_USR_CFG_BASE				(0)

#define FLASH_USR_CFG_PRODUCT_INFO_STORED	(0x0)
#define FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN	(0x1)

#define FLASH_USR_CFG_VENDOR_NAME		(0x1)
#define FLASH_USR_CFG_VENDOR_NAME_LEN 		(0x20)  //32 Byte

#define FLASH_USR_CFG_PRODUCT_TYPE		(0x21)
#define FLASH_USR_CFG_PRODUCT_TYPE_LEN 		(0x20)  //32 Byte

#define FLASH_USR_CFG_PRODUCT_NAME		(0x41)
#define FLASH_USR_CFG_PRODUCT_NAME_LEN 		(0x20)  //32 Byte

#define FLASH_USR_CFG_RESERVE_1			(0x61)
#define FLASH_USR_CFG_RESERVE_1_LEN 		(0x0)

typedef enum _UART_FlASH_UPG_ID_{
    UART_FlASH_UPG_ID_LOADER = 1,
    UART_FlASH_UPG_ID_RECOVERY_FW,
    UART_FlASH_UPG_ID_EEPROM,
    UART_FlASH_UPG_ID_STA_FW,
    UART_FlASH_UPG_ID_STA_XIP_FW,
    UART_FlASH_UPG_ID_AP_FW,
    UART_FlASH_UPG_ID_AP_XIP_FW,
    UART_FlASH_UPG_ID_MAX
}UART_FlASH_UPG_ID; 
#endif
