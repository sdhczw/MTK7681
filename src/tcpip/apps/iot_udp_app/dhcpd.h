/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack
 *
 * @(#)$Id: dhcpc.h,v 1.3 2006/06/11 21:46:37 adam Exp $
 */
#ifndef __DHCPD_H__
#define __DHCPD_H__

#include "uip_timer.h"
//#include "pt.h"


#define DHCPC_SERVER_PORT  67
#define DHCPC_CLIENT_PORT  68

#define DHCP_MAGIC			0x63825363


#define BROADCAST_FLAG		0x8000

#define OPTION_FIELD		0
#define FILE_FIELD			1
#define SNAME_FIELD			2

/* miscellaneous defines */
#define MAC_BCAST_ADDR		(unsigned char *) "\xff\xff\xff\xff\xff\xff"

#define OPT_CODE			0
#define OPT_LEN				1
#define OPT_DATA			2



#define BOOTP_BROADCAST 0x8000

#define DHCP_REQUEST        1
#define DHCP_REPLY          2
#define DHCP_HTYPE_ETHERNET 1
#define DHCP_HLEN_ETHERNET  6
#define DHCP_MSG_LEN      236

#define DHCPDISCOVER  1
#define DHCPOFFER     2
#define DHCPREQUEST   3
#define DHCPDECLINE   4
#define DHCPACK       5
#define DHCPNAK       6
#define DHCPRELEASE   7


/* DHCP option codes (partial list) */
#define DHCP_PADDING		0x00
#define DHCP_OPTION_SUBNET_MASK   1
#define DHCP_OPTION_ROUTER        3
#define DHCP_OPTION_DNS_SERVER    6
#define DHCP_OPTION_DOMAIN_NAME  15
#define DHCP_OPTION_REQ_IPADDR   50
#define DHCP_OPTION_LEASE_TIME   51
#define DHCP_OPTION_MSG_TYPE     53
#define DHCP_OPTION_SERVER_ID    54
#define DHCP_OPTION_REQ_LIST     55
#define DHCP_OPTION_END         255
#define DHCP_OPTION_OVER		0x34


//typedef struct dhcpc_state uip_udp_appstate_t;
//#define UIP_UDP_APPCALL dhcpc_appcall



struct dhcpd 
{
	unsigned char op;
	unsigned char htype;
	unsigned char hlen;
	unsigned char hops;
	unsigned long xid;
	unsigned short secs;
	unsigned short flags;
	unsigned long ciaddr;
	unsigned long yiaddr;
	unsigned long siaddr;
	unsigned long giaddr;
	unsigned char chaddr[16];
	unsigned char sname[64];
	unsigned char file[128];
	unsigned long cookie;
	unsigned char options[308]; /* 312 - cookie */ 
};

// IPv4 support
//typedef struct in_addr {
    //UINT32  s_addr;  // IPv4 address
//} in_addr_t;

typedef UINT32 in_addr;

/**
 * Representation of dhcpd lease connection.
 */
struct uip_dhcpd_conn {
  u8_t chaddr[16];
  in_addr yiaddr;
  u8_t  flag;          /**check this whether IP usage for client or not, 0 means not usage,1 means usage* */
  struct timer timer;	//lease timer.
};

void dhcpd_init();

void dhcpd_appcall(void);

void handle_dhcpd(void);

		
#endif

