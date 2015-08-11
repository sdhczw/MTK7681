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

u8 u8publicBuf[1024];

u8 u8TestBuf[1024];
u8 u8PlainBuf[1024];
u8 u8DeplainBuf[1024];

extern u8 g_u8DumpCloudMsg[102400];
extern IOT_ADAPTER   	IoTpAd;
#define DEFAULT_VENDOR_NEME			"Mediatek"
#define DEFAULT_PRODUCT_TYPE    	"IoT 1"
#define DEFAULT_PRODUCT_NAME        "MT7681"
#define DEFAULT_CLOUD_ADDR          "www.baidu.com"

#define DEFAULT_IOT_CLOUD_KEY {\
    0xb0, 0x7e, 0xab, 0x09, \
    0x73, 0x4e, 0x78, 0x12, \
    0x7e, 0x8c, 0x54, 0xcd, \
    0xbb, 0x93, 0x3c, 0x16, \
    0x96, 0x23, 0xaf, 0x7a, \
    0xfc, 0xd2, 0x8b, 0xd1, \
    0x43, 0xa2, 0xbb, 0xc8, \
    0x77, 0xa0, 0xca, 0xcd, \
    0x01, 0x00, 0x01\
}
#define DEFAULT_IOT_PRIVATE_KEY {\
    0xb0, 0x7e, 0xab, 0x09, \
    0x73, 0x4e, 0x78, 0x12, \
    0x7e, 0x8c, 0x54, 0xcd, \
    0xbb, 0x93, 0x3c, 0x16, \
    0x96, 0x23, 0xaf, 0x7a, \
    0xfc, 0xd2, 0x8b, 0xd1, \
    0x43, 0xa2, 0xbb, 0xc8, \
    0x77, 0xa0, 0xca, 0xcd, \
    0xef, 0x28, 0x66, 0xbd, \
    0x44, 0xc1, 0x27, 0x58, \
    0x3f, 0x71, 0xe3, 0x03, \
    0xcf, 0x11, 0x69, 0xf1, \
    0xbc, 0xec, 0x8f, 0xcd, \
    0xb5, 0x88, 0xab, 0x50, \
    0x5d, 0xb3, 0xf1, 0xd3, \
    0xbb, 0x9d, 0xf2, 0x9d, \
    0xcd, 0x04, 0xff, 0x7e, \
    0x45, 0x90, 0xa8, 0x1f, \
    0xf8, 0xd3, 0xb2, 0xdf, \
    0x33, 0x06, 0x24, 0xa1, \
    0x93, 0x57, 0x4b, 0xaf, \
    0xfb, 0x6c, 0x63, 0x6f, \
    0x82, 0x24, 0xdc, 0xed, \
    0x6c, 0xdd, 0x7a, 0x61, \
    0x9a, 0xd2, 0x29, 0x32, \
    0xdc, 0x4a, 0x86, 0x20, \
    0x6c, 0x98, 0x16, 0xce, \
    0xfd, 0x31, 0x50, 0xd6\
}

IOT_USR_CFG Usr_Cfg = {
    DEFAULT_VENDOR_NEME,
    DEFAULT_PRODUCT_TYPE,
    DEFAULT_PRODUCT_NAME,
    DEFAULT_IOT_PRIVATE_KEY,
    DEFAULT_IOT_CLOUD_KEY,
    DEFAULT_CLOUD_ADDR
};

void rsaoffline()
{
    u32 u32len;
    u32 testlen;
    u32 u32Index;
    extern u32 g_u32TraceSwitch;
    g_u32TraceSwitch = 1;

    testlen = 64;
    for (u32Index = 0; u32Index < testlen; u32Index++)
    {
        u8PlainBuf[u32Index] = u32Index;
    }
    u32len = 0;
    for (u32Index = 0; u32Index < 2; u32Index++)
    {

        ZC_Printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        ZC_TraceData(u8PlainBuf, testlen);
        ZC_Printf("+++++++++++++++++++++++++++++++++++\n");

        SEC_EncryptTextByRsa(g_u8DumpCloudMsg, u8PlainBuf, testlen, &u32len);
        //rsa_pkcs1_encrypt(rsa1, RSA_PUBLIC, testlen, u8PlainBuf, g_u8DumpCloudMsg);

        ZC_TraceData(g_u8DumpCloudMsg, u32len);
        ZC_Printf("+++++++++++++++++++++++++++++++++++\n");
        SEC_DecryptTextByRsa(g_u8DumpCloudMsg, u8DeplainBuf, u32len, &testlen);
        //rsa_pkcs1_decrypt(&rsa, RSA_PRIVATE, &s32len, g_u8DumpCloudMsg, u8DeplainBuf, testlen);
        ZC_TraceData(u8DeplainBuf, testlen);

        ZC_Printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    }
}
void newrsa()
{
    int ret;
    rsa_context rsa;
    rsa_context newRsa;
    havege_state hs;
    u32 u32Index;
    s32 s32len;
    s32 sum = 0;
    s32 testlen;
    extern u32 g_u32TraceSwitch;
    g_u32TraceSwitch = 1;
    MT_Init();

    havege_init( &hs );

    printf( " ok\n  . Generating the RSA key [ %d-bit ]...\n", KEY_SIZE );

    rsa_init( &rsa, RSA_PKCS_V15, 0, havege_rand, &hs );
    if( ( ret = rsa_gen_key( &rsa, KEY_SIZE, EXPONENT ) ) != 0 )
    {
        printf( " failed\n  ! rsa_gen_key returned %d\n\n", ret );
    }
    
    s32len = mpi_size(&rsa.N);
    mpi_write_binary(&rsa.N, u8publicBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.E);
    mpi_write_binary(&rsa.E, u8publicBuf+sum, s32len);
    sum +=s32len;
    ZC_TraceData(u8publicBuf, sum);

    ZC_Printf("+++++++++++++++++++++++++++++++++++\n");
    sum = 0;
    s32len = mpi_size(&rsa.N);
    mpi_write_binary(&rsa.N, u8TestBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.P);
    mpi_write_binary(&rsa.P, u8TestBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.Q);
    mpi_write_binary(&rsa.Q, u8TestBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.DP);
    mpi_write_binary(&rsa.DP, u8TestBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.DQ);
    mpi_write_binary(&rsa.DQ, u8TestBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.QP);
    mpi_write_binary(&rsa.QP, u8TestBuf+sum, s32len);
    sum +=s32len;
    ZC_TraceData(u8TestBuf, sum);
    ZC_Printf("+++++++++++++++++++++++++++++++++++\n");

    testlen = 20;
    for (u32Index = 0; u32Index < testlen; u32Index++)
    {
        u8PlainBuf[u32Index] = u32Index;
    }
    s32len = 32;
    for (u32Index = 0; u32Index < 2; u32Index++)
    {

        ZC_Printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
                    rsa_context *rsa1;

        ZC_TraceData(u8PlainBuf, testlen);
        ZC_Printf("+++++++++++++++++++++++++++++++++++\n");
        
        SEC_EncryptTextByRsa(g_u8DumpCloudMsg, u8PlainBuf, testlen, &s32len);
        //rsa_pkcs1_encrypt(rsa1, RSA_PUBLIC, testlen, u8PlainBuf, g_u8DumpCloudMsg);

        ZC_TraceData(g_u8DumpCloudMsg, s32len);
        ZC_Printf("+++++++++++++++++++++++++++++++++++\n");
        SEC_DecryptTextByRsa(g_u8DumpCloudMsg, u8DeplainBuf, s32len, &testlen);
        //rsa_pkcs1_decrypt(&rsa, RSA_PRIVATE, &s32len, g_u8DumpCloudMsg, u8DeplainBuf, testlen);
        ZC_TraceData(u8DeplainBuf, testlen);

        ZC_Printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    }

}

void testcloud()
{
    mpi X;  
    s32 s32len;
    u8 u8PublicKey[2048];
    u8 u8CiperBuf[1000] = {
        //108, 240, 118, 120,
        //21, 134, 166, 113,
        //26, 172, 175, 71,
        //107, 24, 164, 147,
        //44, 204, 219, 36,
        //168, 29, 114, 160,
        //227, 137, 152, 196,
        //20, 185, 27, 188
        26, 230, 100, 126, 
        46, 27, 102, 152, 
        113, 174, 229, 144, 
        193, 247, 20, 190, 
        153, 10, 239, 200, 
        179, 18, 141, 80, 
        109, 242, 231, 242, 
        200, 59, 150, 195
    };
    u16 u16CiperLen = 32;
    u32 u32Retval;
    u8 u8PlainBuf[1000];
    u8 u8Key[] = {"80138512665003396643737838315916663972728479914654754587175091902061894104953"};
    mpi_init(&X); 
    
    //mpi_read_binary(&X, Usr_Cfg.CloudKey, ZC_SEC_RSA_KEY_LEN >> 3);
    //s32len = 2048;
    //mpi_write_string(&X, 10, u8PublicKey, &s32len);
    //ZC_Printf("public N = %s\n",u8PublicKey);


    //MT_Init();
    //u32Retval = SEC_DecryptTextByRsa(u8CiperBuf, u8PlainBuf, u16CiperLen, &s32len);
    //ZC_TraceData(u8PlainBuf, s32len);


    mpi_read_string(&X, 10, u8Key);
    s32len = mpi_size(&X);
    mpi_write_binary(&X, Usr_Cfg.CloudKey, s32len);
    ZC_TraceData(Usr_Cfg.CloudKey, s32len);

    //memcpy(&IoTpAd.UsrCfg, &Usr_Cfg, sizeof(IOT_USR_CFG));
    //TestSec();

}

BOOLEAN reset_usr_cfg(BOOLEAN bUpFlash)
{
    u16_t CfgLen;
    u16_t FlashLen;
    u16_t ReadLen;
    u32 u32FlashBase;

    memset(IoTpAd.flash_rw_buf ,0xff, sizeof(IoTpAd.flash_rw_buf));

    memcpy(&IoTpAd.UsrCfg , &Usr_Cfg , sizeof(IOT_USR_CFG));

    if(bUpFlash == TRUE)
    {
        IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_INFO_STORED] = PRODUCT_INFO_STORED;

        FlashLen = sizeof(IoTpAd.flash_rw_buf);
        ReadLen = 0;
        CfgLen = sizeof(IOT_USR_CFG);
        if ((CfgLen + FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN) <= FlashLen)
        {
            memcpy( &IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME],    
                IoTpAd.UsrCfg.VendorName,
                CfgLen);
            spi_flash_write(FLASH_USR_CFG_BASE, IoTpAd.flash_rw_buf, FlashLen);
        }
        else
        {
            memcpy( &IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME],    
                IoTpAd.UsrCfg.VendorName,
                FlashLen - FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN);

            spi_flash_write(FLASH_USR_CFG_BASE, IoTpAd.flash_rw_buf, FlashLen);

            ReadLen += FlashLen - FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN;
            u32FlashBase = FLASH_USR_CFG_BASE + FlashLen;
            while (ReadLen + FlashLen <= CfgLen)
            {
                memcpy( IoTpAd.flash_rw_buf,    
                    IoTpAd.UsrCfg.VendorName + ReadLen,
                    FlashLen);

                spi_flash_write(u32FlashBase, IoTpAd.flash_rw_buf, FlashLen);

                ReadLen += FlashLen;
                u32FlashBase += FlashLen;
            }

            if (ReadLen < CfgLen)
            {
                memcpy(IoTpAd.flash_rw_buf,    
                    IoTpAd.UsrCfg.VendorName + ReadLen,
                    CfgLen - ReadLen);
                spi_flash_write(u32FlashBase, IoTpAd.flash_rw_buf, FlashLen);
            }
        }
    }

    return TRUE;
}

BOOLEAN load_usr_cfg(VOID)
{
    u16_t CfgLen;
    u16_t FlashLen;
    u16_t ReadLen;
    u32 u32FlashBase;
    spi_flash_read(FLASH_USR_CFG_BASE, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));

    if (IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_INFO_STORED] == PRODUCT_INFO_STORED)
    {
        FlashLen = sizeof(IoTpAd.flash_rw_buf);
        ReadLen = 0;
        CfgLen = sizeof(IOT_USR_CFG);
        if ((CfgLen + FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN) <= FlashLen)
        {
            memcpy((IoTpAd.UsrCfg.VendorName + ReadLen), 
                IoTpAd.flash_rw_buf + FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN, 
                CfgLen);
        }
        else
        {
            memcpy((IoTpAd.UsrCfg.VendorName + ReadLen), 
                IoTpAd.flash_rw_buf + FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN, 
                FlashLen - FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN);

            ReadLen += FlashLen - FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN;
            u32FlashBase = FLASH_USR_CFG_BASE + FlashLen;
            while (ReadLen + FlashLen <= CfgLen)
            {
                spi_flash_read(u32FlashBase, IoTpAd.flash_rw_buf, FlashLen);

                memcpy((IoTpAd.UsrCfg.VendorName + ReadLen), IoTpAd.flash_rw_buf, FlashLen);

                ReadLen += FlashLen;
                u32FlashBase += FlashLen;
            }

            if (ReadLen < CfgLen)
            {
                spi_flash_read(u32FlashBase, IoTpAd.flash_rw_buf, FlashLen);
                memcpy((IoTpAd.UsrCfg.VendorName + ReadLen), IoTpAd.flash_rw_buf, CfgLen - ReadLen);
            }
        }
    }
    else
    {
        reset_usr_cfg(TRUE);
    }


    return TRUE;
}


void testread()
{
    memset(g_u8DumpCloudMsg, 0xcc, 102400);
    load_usr_cfg();
    
    load_usr_cfg();
}