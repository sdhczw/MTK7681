#ifndef	__RTMP_GENERAL_PUB_H__
#define	__RTMP_GENERAL_PUB_H__

#include "types.h"
#include "stddef.h"
#include "rtmp_dot11.h"
#include "queue.h"



/* definition to support multiple BSSID */
#define BSS0                     0

#define MAX_CUSTOM_LEN           128 
#define RX_FCEINFO_SIZE          4  /* descriptor 4th DWord */
#define RXINFO_SIZE              4
#define TXINFO_SIZE              4
#define TXWI_SIZE               20
#ifdef __MT7681
#define RXWI_SIZE             	24 // 6 * 4DWORD
#endif


#define NDIS_STATUS					INT
#define NDIS_STATUS_SUCCESS     	0x00
#define NDIS_STATUS_FAILURE         0x01
#define NDIS_STATUS_INVALID_DATA	0x02
#define NDIS_STATUS_RESOURCES       0x03


/* value domain of 802.11 MGMT frame's FC.subtype, which is b7..4 of the 1st-byte of MAC header */
#define SUBTYPE_ASSOC_REQ           0
#define SUBTYPE_ASSOC_RSP           1
#define SUBTYPE_REASSOC_REQ         2
#define SUBTYPE_REASSOC_RSP         3
#define SUBTYPE_PROBE_REQ           4
#define SUBTYPE_PROBE_RSP           5
#define SUBTYPE_BEACON              8
#define SUBTYPE_ATIM                9
#define SUBTYPE_DISASSOC            10
#define SUBTYPE_AUTH                11
#define SUBTYPE_DEAUTH              12
#define SUBTYPE_ACTION              13
#define SUBTYPE_ACTION_NO_ACK       14

/* value domain of 802.11 CNTL frame's FC.subtype, which is b7..4 of the 1st-byte of MAC header */
#define SUBTYPE_WRAPPER				7
#define SUBTYPE_BLOCK_ACK_REQ       8
#define SUBTYPE_BLOCK_ACK			9
#define SUBTYPE_PS_POLL             10
#define SUBTYPE_RTS                 11
#define SUBTYPE_CTS                 12
#define SUBTYPE_ACK                 13
#define SUBTYPE_CFEND               14
#define SUBTYPE_CFEND_CFACK         15

/* value domain of 802.11 DATA frame's FC.subtype, which is b7..4 of the 1st-byte of MAC header */
#define SUBTYPE_DATA                0
#define SUBTYPE_DATA_CFACK          1
#define SUBTYPE_DATA_CFPOLL         2
#define SUBTYPE_DATA_CFACK_CFPOLL   3
#define SUBTYPE_NULL_FUNC           4
#define SUBTYPE_CFACK               5
#define SUBTYPE_CFPOLL              6
#define SUBTYPE_CFACK_CFPOLL        7
#define SUBTYPE_QDATA               8
#define SUBTYPE_QDATA_CFACK         9
#define SUBTYPE_QDATA_CFPOLL        10
#define SUBTYPE_QDATA_CFACK_CFPOLL  11
#define SUBTYPE_QOS_NULL            12
#define SUBTYPE_QOS_CFACK           13
#define SUBTYPE_QOS_CFPOLL          14
#define SUBTYPE_QOS_CFACK_CFPOLL    15

/* Reason code definitions */
#define REASON_RESERVED                 0
#define REASON_UNSPECIFY                1
#define REASON_NO_LONGER_VALID          2
#define REASON_DEAUTH_STA_LEAVING       3
#define REASON_DISASSOC_INACTIVE        4
#define REASON_DISASSPC_AP_UNABLE       5
#define REASON_CLS2ERR                  6
#define REASON_CLS3ERR                  7
#define REASON_DISASSOC_STA_LEAVING     8
#define REASON_STA_REQ_ASSOC_NOT_AUTH   9
#define REASON_INVALID_IE               13
#define REASON_MIC_FAILURE              14
#define REASON_4_WAY_TIMEOUT            15
#define REASON_GROUP_KEY_HS_TIMEOUT     16
#define REASON_IE_DIFFERENT             17
#define REASON_MCIPHER_NOT_VALID        18
#define REASON_UCIPHER_NOT_VALID        19
#define REASON_AKMP_NOT_VALID           20
#define REASON_UNSUPPORT_RSNE_VER       21
#define REASON_INVALID_RSNE_CAP         22
#define REASON_8021X_AUTH_FAIL          23
#define REASON_CIPHER_SUITE_REJECTED    24
#define REASON_DECLINED                 37

/* ================================================================ */
/* Tx /	Rx / Mgmt ring descriptor definition */
/* ================================================================ */

/* the following PID values are used to mark outgoing frame type in TXD->PID so that */
/* proper TX statistics can be collected based on these categories */
/* b3-2 of PID field - */
#define PID_MGMT				0x05
#define PID_BEACON				0x0c
#define PID_DATA_NORMALUCAST	0x02
#define PID_DATA_AMPDU	 		0x04
#define PID_DATA_NO_ACK    		0x08
#define PID_DATA_NOT_NORM_ACK	0x03
/* value domain of pTxD->HostQId (4-bit: 0~15) */
#define QID_AC_BK               1   /* meet ACI definition in 802.11e */
#define QID_AC_BE               0   /* meet ACI definition in 802.11e */
#define QID_AC_VI               2
#define QID_AC_VO               3
#define QID_HCCA                4
#define NUM_OF_TX_RING          5
#define QID_CTRL	            9
#define QID_MGMT                13
#define QID_RX                  14
#define QID_OTHER               15

//============================================================
// ASIC WCID Table definition.
//============================================================
#define BSSID_WCID			1	// in infra mode, always put bssid with this WCID 
#define MCAST_WCID			0x0
#define RESERVED_WCID		0xff

/* power status related definitions */
#define PWR_ACTIVE                      0
#define PWR_SAVE                        1
#define PWR_MMPS                        2

/* IE code */
#define IE_SSID                         0
#define IE_SUPP_RATES                   1
#define IE_FH_PARM                      2
#define IE_DS_PARM                      3
#define IE_CF_PARM                      4
#define IE_TIM                          5
#define IE_IBSS_PARM                    6
#define IE_COUNTRY                      7	/* 802.11d */
#define IE_802_11D_REQUEST              10	/* 802.11d */
#define IE_QBSS_LOAD                    11	/* 802.11e d9 */
#define IE_EDCA_PARAMETER               12	/* 802.11e d9 */
#define IE_TSPEC                        13	/* 802.11e d9 */
#define IE_TCLAS                        14	/* 802.11e d9 */
#define IE_SCHEDULE                     15	/* 802.11e d9 */
#define IE_CHALLENGE_TEXT               16
#define IE_POWER_CONSTRAINT             32	/* 802.11h d3.3 */
#define IE_POWER_CAPABILITY             33	/* 802.11h d3.3 */
#define IE_TPC_REQUEST                  34	/* 802.11h d3.3 */
#define IE_TPC_REPORT                   35	/* 802.11h d3.3 */
#define IE_SUPP_CHANNELS                36	/* 802.11h d3.3 */
#define IE_CHANNEL_SWITCH_ANNOUNCEMENT  37	/* 802.11h d3.3 */
#define IE_MEASUREMENT_REQUEST          38	/* 802.11h d3.3 */
#define IE_MEASUREMENT_REPORT           39	/* 802.11h d3.3 */
#define IE_QUIET                        40	/* 802.11h d3.3 */
#define IE_IBSS_DFS                     41	/* 802.11h d3.3 */
#define IE_ERP                          42	/* 802.11g */
#define IE_TS_DELAY                     43	/* 802.11e d9 */
#define IE_TCLAS_PROCESSING             44	/* 802.11e d9 */
#define IE_QOS_CAPABILITY               46	/* 802.11e d6 */
#define IE_HT_CAP                       45	/* 802.11n d1. HT CAPABILITY. ELEMENT ID TBD */
#define IE_AP_CHANNEL_REPORT			51	/* 802.11k d6 */
#define IE_HT_CAP2                      52	/* 802.11n d1. HT CAPABILITY. ELEMENT ID TBD */
#define IE_RSN                          48	/* 802.11i d3.0 */
#define IE_WPA2                         48	/* WPA2 */
#define IE_EXT_SUPP_RATES               50	/* 802.11g */
#define IE_TIMEOUT_INTERVAL             56  /* 802.11w */
#define IE_SUPP_REG_CLASS               59	/* 802.11y. Supported regulatory classes. */
#define IE_EXT_CHANNEL_SWITCH_ANNOUNCEMENT	60	/* 802.11n */
#define IE_ADD_HT                       61	/* 802.11n d1. ADDITIONAL HT CAPABILITY. ELEMENT ID TBD */
#define IE_ADD_HT2                      53	/* 802.11n d1. ADDITIONAL HT CAPABILITY. ELEMENT ID TBD */


/* For 802.11n D3.03 */
/*#define IE_NEW_EXT_CHA_OFFSET               62    // 802.11n d1. New extension channel offset elemet */
#define IE_SECONDARY_CH_OFFSET			62	/* 802.11n D3.03        Secondary Channel Offset element */
#define IE_WAPI							68	/* WAPI information element. Same as Bss Ac Access Dealy Element. */
#define IE_2040_BSS_COEXIST             72	/* 802.11n D3.0.3 */
#define IE_2040_BSS_INTOLERANT_REPORT   73	/* 802.11n D3.03 */
#define IE_OVERLAPBSS_SCAN_PARM         74	/* 802.11n D3.03 */
#define IE_CHANNEL_USAGE				97	/* Cisco advertises suggested channel using this IE. */
#define IE_TIME_ZONE					98	/* 802.11V */
#define IE_INTERWORKING					107 /* 802.11u */
#define IE_ADVERTISEMENT_PROTO			108 /* 802.11u */
#define IE_QOS_MAP_SET					110 /* 802.11u */
#define IE_ROAMING_CONSORTIUM			111 /* 802.11u */

#define IE_EXT_CAPABILITY               127   // 802.11n D3.03 
#define IE_VENDOR_SPECIFIC              221

/* value domain of 802.11 header FC.Tyte, which is b3..b2 of the 1st-byte of MAC header */
#define BTYPE_MGMT                  0
#define BTYPE_CNTL                  1
#define BTYPE_DATA                  2

/* Action Category */
#define CATEGORY_SPECTRUM		    0
#define CATEGORY_QOS		     	1
#define CATEGORY_DLS		    	2
#define CATEGORY_BA			        3

/* Spectrum  Action field value 802.11h 7.4.1 */
#define SPEC_MRQ	                0	/* Request */
#define SPEC_MRP	                1	/* Report */
#define SPEC_TPCRQ	                2
#define SPEC_TPCRP	                3
#define SPEC_CHANNEL_SWITCH	        4

/* BA  Action field value */
#define ADDBA_REQ	                0
#define ADDBA_RESP	                1
#define DELBA                       2

/* BA Policy subfiled value in ADDBA frame */
#define IMMED_BA 	1
#define DELAY_BA	0

#define CIPHER_NONE                 0
#define CIPHER_WEP64                1
#define CIPHER_WEP128               2
#define CIPHER_TKIP                 3
#define CIPHER_AES                  4
#define CIPHER_CKIP64               5
#define CIPHER_CKIP128              6
#define CIPHER_CKIP152          	7
#define CIPHER_SMS4					8

/* Cipher type in WPA/RSN IE */
#define IE_CIPHER_TYPE_TKIP         2
#define IE_CIPHER_TYPE_CCMP         4 


typedef struct _CIPHER_KEY {
	UCHAR   Key[16];            	// 128 bits max
	UCHAR   TxMic[8];
	UCHAR   RxMic[8];
	UCHAR   TxTsc[16];           	// TSC value. Change it from 48bit to 128bit 
	UCHAR   RxTsc[16];           	// TSC value. Change it from 48bit to 128bit
	UCHAR   CipherAlg;           	// 0:none, 1:WEP64, 2:WEP128, 3:TKIP, 4:AES, 5:CKIP64, 6:CKIP128
 	UCHAR   KeyLen; 				// Key length for each key, 0: entry is invalid
	UCHAR   BssId[6];
	UCHAR   Type;               	// Indicate Pairwise/Group when reporting MIC error
} CIPHER_KEY, *PCIPHER_KEY;



/***************************************************************************
  *	Packet Header definition
  **************************************************************************/
#define SNAP_LENGTH             6
#define UDP_HEADER_LENGTH       8
#define IP_PROTO_TCP            6
#define IP_PROTO_UDP            17
#define IP_PROTO_ICMPv6         58
#define UDP_DPORT_FIELD_OFFSET  2
#define UDP_LENGTH_FIELD_OFFSET 4
#define TCP_SPORT_FIELD_OFFSET  0
#define TCP_DPORT_FIELD_OFFSET  2
#define TCP_FLAG_FIELD_OFFSET   13
#define MAGIC_FRAME_LENGTH      102
#define UDP_MAGIC_FRAME_LENGTH  110
#define MAGIC_FRAME_MAC_TIMES   16
#define ETHER_TYPE_ARP          0x0806
#define ETHER_TYPE_IPv4         0x0800
#define ETHER_TYPE_IPv6         0x86DD
#define LENGTH_802_11           24
#define LENGTH_802_3            14
#define LENGTH_802_3_TYPE       2
#define LENGTH_802_1_H          8
#define LENGTH_EAPOL_H          4

#define PKT_TYPE_NONE           0
#define PKT_TYPE_UCAST          1
#define PKT_TYPE_MCAST          2
#define PKT_TYPE_BCAST          3

#define ARP_OPCODE_OFFSET           6
#define ARP_SA_OFFSET               8
#define ARP_SPA_OFFSET              14
#define ARP_DA_OFFSET               18
#define ARP_TPA_OFFSET              24
#define ARP_MSG_SIZE                28

#define IPV4_VER_IHL_OFFSET         0
#define IPV4_LENGTH_OFFSET			2
#define IPV4_PROTO_OFFSET           9
#define IPV4_SRCIP_OFFSET           12
#define IPV4_DSTIP_OFFSET           16
#define IPV4_HEADER_LEN_MAX         24
#define IPV6_LENGTH_OFFSET			4
#define IPV6_PROTO_OFFSET           6
#define IPV4_HEADER_SIZE  			20
#define IPV6_HEADER_SIZE  			40
#define IPV6_HEADER_EXT_SIZE		24
#define IPV6_ADDR_LEN               16

/* IPv6 next header define */
#define NEXTHDR_HOP	        0	/* Hop-by-hop option header. */
#define NEXTHDR_TCP		    6	/* TCP segment. */
#define NEXTHDR_UDP		    17	/* UDP message. */
#define NEXTHDR_IPV6		41	/* IPv6 in IPv6 */
#define NEXTHDR_ROUTING		43	/* Routing header. */
#define NEXTHDR_FRAGMENT	44	/* Fragmentation/reassembly header. */
#define NEXTHDR_ESP		    50	/* Encapsulating security payload. */
#define NEXTHDR_AUTH		51	/* Authentication header. */
#define NEXTHDR_ICMP		58	/* ICMP for IPv6. */
#define NEXTHDR_NONE		59	/* No next header */
#define NEXTHDR_DEST		60	/* Destination options header. */
#define NEXTHDR_MOBILITY	135	/* Mobility header. */

/* ICMPv6 type */
#define ICMPV6_NEIGHBOR_SOLICITATION     135
#define ICMPV6_NEIGHBOR_ADVERTISEMENT    136

/*ICMPv6 flag define */
#define ICMPV6_FLAG_SOLICITIED           0x40000000

/* TCP flag define */
#define TCP_FLAG_SYN                     0x02




/***************************************************************************
  *	Tx Path software control block related data structures
  **************************************************************************/
typedef struct _TX_BLK_
{
	// Following structure used for the characteristics of a specific packet.
	pBD_t				pPacket;
	PUCHAR				pSrcBufHeader;				// Reference to the head of sk_buff->data
	PUCHAR				pSrcBufData;				// Reference to the sk_buff->data, will changed depends on hanlding progresss
	UINT				SrcBufLen;					// Length of packet payload which not including Layer 2 header
	UCHAR				HeaderBuf[128];				// TempBuffer for TX_WI + 802.11 Header + padding + AMSDU SubHeader + LLC/SNAP

	PUCHAR				pExtraLlcSnapEncap;			// NULL means no extra LLC/SNAP is required
	UCHAR				CipherAlg;					// cipher alogrithm
	PCIPHER_KEY			pKey;
	UCHAR				KeyIdx;						// Indicate the transmit key index
	UCHAR				Wcid;						// The MAC entry associated to this packet
	UCHAR				MpduHeaderLen;				// 802.11 header length NOT including the padding
	UCHAR				HdrPadLen;					// recording Header Padding Length;
} TX_BLK, *PTX_BLK;


#ifdef __MT7681
typedef struct GNU_PACKED _RXWI_STRUC{
    /* Word 0 */
    UINT32      WCID:8;
    UINT32      KeyIdx:2;
    UINT32      BSSIdx:3;
    UINT32      UDF:3;
    UINT32      MPDUtotalByteCount:12;
    UINT32      tid:4;

    /* Word 1 */
    UINT32      FRAG:4;
    UINT32      SEQUENCE:12;
    UINT32      mcs:7;
    UINT32      bw:1;
    UINT32      sgi:1;
    UINT32      stbc:2;
    UINT32      eTxBF:1; /* eTxBF enable */
    UINT32      Sounding:1; /* Sounding enable */
    UINT32      iTxBF:1; /* iTxBF enable */
    UINT32      PHYMode:2;              /* 1: this RX frame is unicast to me */

    /*Word2 */
    UINT32      RSSI0:8;
    UINT32      RSSI1:8;
    UINT32      RSSI2:8;
    UINT32      rsv1:8;

    /*Word3 */
    UINT32      SNR0:8;
    UINT32      SNR1:8;
    UINT32      SNR2:8;
    UINT32      FOFFSET:8;

    /* For Expert Antenna */
    UINT32      RX0_RSSI:8;
    UINT32      RX1_RSSI:8;
    UINT32      STS0_SNR:8;
    UINT32      STS1_SNR:8;

    /* Word 5 */
    /* For Exper Antenna */
    UINT32      EANT_ID:8;
    UINT32      rsv4:24;
}   RXWI_STRUC, *PRXWI_STRUC;

typedef struct GNU_PACKED _TXWI_STRUC {
    /* Word 0 */
    /* ex: 00 03 00 40 means txop = 3, PHYMODE = 1 */
    UINT32      FRAG:1;     /* 1 to inform TKIP engine this is a fragment. */
    UINT32      MIMOps:1;   /* the remote peer is in dynamic MIMO-PS mode */
    UINT32      CFACK:1;
    UINT32      TS:1;

    UINT32      AMPDU:1;
    UINT32      MpduDensity:3;
    UINT32      txop:2; /*FOR "THIS" frame. 0:HT TXOP rule , 1:PIFS TX ,2:Backoff, 3:sifs only when previous frame exchange is successful. */
    UINT32      NDPSndRate:2; /* 0 : MCS0, 1: MCS8, 2: MCS16, 3: reserved */
    UINT32      NDPSndBW:1; /* NDP sounding BW */
    UINT32      Autofallback:1; /* TX rate auto fallback disable */
    UINT32      TXRPT:1;
    UINT32      rsv:1;
    UINT32      MCS:7;
    UINT32      BW:1;   /*channel bandwidth 20MHz or 40 MHz */
    UINT32      ShortGI:1;
    UINT32      STBC:2; /* 1: STBC support MCS =0-7,   2,3 : RESERVE */
    UINT32      eTxBF:1; /* eTxBF enable */
    UINT32      Sounding:1; /* Sounding enable */
    UINT32      iTxBF:1; /* iTxBF enable */
    UINT32      PHYMODE:2;
	
    /* Word1 */
    /* ex:  1c ff 38 00 means ACK=0, BAWinSize=7, MPDUtotalByteCnt = 0x38 */
    UINT32      ACK:1;
    UINT32      NSEQ:1;
    UINT32      BAWinSize:6;
    UINT32      WirelessCliID:8;
    UINT32      MPDUtotalByteCount:12;
    UINT32      PacketId:4;
    /*Word2 */
    UINT32      IV;
    /*Word3 */
    UINT32      EIV;

    /* Word 4 */
    /* For Expert Antenna */
    UINT32      EncodedAntID:8;
    UINT32      TxStreamMode:8;
    UINT32      TxPwrAdj:4;
    UINT32      CCP:1;
    UINT32      Reserved:11;
}   TXWI_STRUC, *PTXWI_STRUC;
#endif /* __MT7681 */

typedef	struct	GNU_PACKED _RXINFO_STRUC {
	UINT32		BA:1;
	UINT32		DATA:1;
	UINT32		NULLDATA:1;
	UINT32		FRAG:1;
	UINT32		U2M:1;              // 1: this RX frame is unicast to me
	UINT32		Mcast:1;            // 1: this is a multicast frame
	UINT32		Bcast:1;            // 1: this is a broadcast frame	
	UINT32		MyBss:1;  	// 1: this frame belongs to the same BSSID	
	UINT32		Crc:1;              // 1: CRC error
	UINT32		IcvErr:1;        
	UINT32		MicErr:1;        
	UINT32		AMSDU:1;		// rx with 802.3 header, not 802.11 header.
	UINT32		HTC:1;
	UINT32		RSSI:1;
	UINT32		L2PAD:1;
	UINT32		AMPDU:1;		// To be moved
	UINT32		Decrypted:1;
	UINT32		Bssidx3:1;
	UINT32		WapiKeyId:1;
	UINT32		PaddingLen:3;
	UINT32		Resrv:6;
	UINT32		TcpChksumPass:1;	
	UINT32		IpChksumPass:1;
	UINT32		TcpChksumErr:1;	
	UINT32		IpChksumErr:1;	
}	RXINFO_STRUC, *PRXINFO_STRUC;

/***************************************************************************
  *	Rx Path software control block related data structures
  **************************************************************************/
typedef struct _RX_BLK_
{
	PRXINFO_STRUC	    pRxINFO;
	PRXWI_STRUC			pRxWI;
	PHEADER_802_11		pHeader;
	void*				pRxPacket;
	UCHAR				*pData;
	USHORT				DataSize;
	USHORT				Flags;
	UCHAR				UserPriority;	// for calculate TKIP MIC using
} RX_BLK;

#define NdisMoveMemory(Destination, Source, Length) memmove(Destination, Source, Length)
//#define NdisCopyMemory(Destination, Source, Length) memcpy(Destination, Source, Length)
#define NdisZeroMemory(Destination, Length)         memset(Destination, 0, Length)
#define NdisFillMemory(Destination, Length, Fill)   memset(Destination, Fill, Length)
#define NdisEqualMemory(Source1, Source2, Length)   (!memcmp(Source1, Source2, Length))


#define	CONV_ARRARY_TO_UINT16(_V)	((_V[0]<<8) | (_V[1]))
#define	SET_UINT16_TO_ARRARY(_V, _LEN)		\
{											\
	_V[0] = ((UINT16)_LEN) >> 8;			\
	_V[1] = ((UINT16)_LEN & 0xFF);					\
}

#define	INC_UINT16_TO_ARRARY(_V, _LEN)			\
{												\
	UINT16	var_len;							\
												\
	var_len = (_V[0]<<8) | (_V[1]);				\
	var_len += _LEN;							\
												\
	_V[0] = (var_len & 0xFF00) >> 8;			\
	_V[1] = (var_len & 0xFF);					\
}
#define MAKE_802_3_HEADER(_p, _pMac1, _pMac2, _pType)                   \
{                                                                       \
    NdisMoveMemory(_p, _pMac1, MAC_ADDR_LEN);                           \
    NdisMoveMemory((_p + MAC_ADDR_LEN), _pMac2, MAC_ADDR_LEN);          \
    NdisMoveMemory((_p + MAC_ADDR_LEN * 2), _pType, LENGTH_802_3_TYPE); \
}

/* Endian byte swapping codes */
#define SWAP16(x) \
    ((UINT16)( \
    (((UINT16)(x) & (UINT16) 0x00ffU) << 8) | \
    (((UINT16)(x) & (UINT16) 0xff00U) >> 8) ))

#define SWAP32(x) \
    ((UINT32)( \
    (((UINT32)(x) & (UINT32) 0x000000ffUL) << 24) | \
    (((UINT32)(x) & (UINT32) 0x0000ff00UL) <<  8) | \
    (((UINT32)(x) & (UINT32) 0x00ff0000UL) >>  8) | \
    (((UINT32)(x) & (UINT32) 0xff000000UL) >> 24) ))

#define SWAP64(x) \
    ((UINT64)( \
    (UINT64)(((UINT64)(x) & (UINT64) 0x00000000000000ffULL) << 56) | \
    (UINT64)(((UINT64)(x) & (UINT64) 0x000000000000ff00ULL) << 40) | \
    (UINT64)(((UINT64)(x) & (UINT64) 0x0000000000ff0000ULL) << 24) | \
    (UINT64)(((UINT64)(x) & (UINT64) 0x00000000ff000000ULL) <<  8) | \
    (UINT64)(((UINT64)(x) & (UINT64) 0x000000ff00000000ULL) >>  8) | \
    (UINT64)(((UINT64)(x) & (UINT64) 0x0000ff0000000000ULL) >> 24) | \
    (UINT64)(((UINT64)(x) & (UINT64) 0x00ff000000000000ULL) >> 40) | \
    (UINT64)(((UINT64)(x) & (UINT64) 0xff00000000000000ULL) >> 56) ))

#define ROUND_UP(__x, __y) \
	(((ULONG)((__x)+((__y)-1))) & ((ULONG)~((__y)-1)))

#ifdef RT_BIG_ENDIAN

#define cpu2le64(x) SWAP64((x))
#define le2cpu64(x) SWAP64((x))
#define cpu2le32(x) SWAP32((x))
#define le2cpu32(x) SWAP32((x))
#define cpu2le16(x) SWAP16((x))
#define le2cpu16(x) SWAP16((x))
#define cpu2be64(x) ((UINT64)(x))
#define be2cpu64(x) ((UINT64)(x))
#define cpu2be32(x) ((UINT32)(x))
#define be2cpu32(x) ((UINT32)(x))
#define cpu2be16(x) ((UINT16)(x))
#define be2cpu16(x) ((UINT16)(x))
    
#else /* Little_Endian */
    
#define cpu2le64(x) ((UINT64)(x))
#define le2cpu64(x) ((UINT64)(x))
#define cpu2le32(x) ((UINT32)(x))
#define le2cpu32(x) ((UINT32)(x))
#define cpu2le16(x) ((UINT16)(x))
#define le2cpu16(x) ((UINT16)(x))
#define cpu2be64(x) SWAP64((x))
#define be2cpu64(x) SWAP64((x))
#define cpu2be32(x) SWAP32((x))
#define be2cpu32(x) SWAP32((x))
#define cpu2be16(x) SWAP16((x))
#define be2cpu16(x) SWAP16((x))
    
#endif /* RT_BIG_ENDIAN */

#endif /* __RTMP_GENERAL_PUB_H__ */ 

