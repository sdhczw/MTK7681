/*
 * Copyright (c) 2001, Adam Dunkels.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Adam Dunkels.
 * 4. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: main.c,v 1.16 2006/06/11 21:55:03 adam Exp $
 *
 */

#include "uip.h"
#include "uip_arp.h"
#include "mt76xx_dev.h"
#include "uip_timer.h"
#include "iot_api.h"
#include "dhcpc.h"
#include "tcpip_main.h"
#include <zc_configuration.h>

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

extern IOT_ADAPTER   	IoTpAd;
struct timer periodic_timer, arp_timer, cli_timer;
int cli_fd = -1;
static clock_time_t clk = CLOCK_SECOND;
/*---------------------------------------------------------------------------*/
int
tcpip_init(void)
{
	int ret = 0;
	
    if (overlayload(9))
    {
        ret = _tcpip_init();
        overlayunload(9);
    }

	return ret;
}

int
_tcpip_init(void)
{
  uip_ipaddr_t ipaddr;



  timer_set(&periodic_timer, CLOCK_SECOND/2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);
  timer_set(&cli_timer, CLOCK_SECOND);
  mt76xx_dev_init();
  uip_init();

  //Printf_High("Tcp INIT \n");
#ifdef CONFIG_SOFTAP
	uip_ipaddr(ipaddr, 192,168,81,1);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, 192,168,81,1);
	uip_setdraddr(ipaddr);
	uip_ipaddr(ipaddr, 255,255,255,0);
	uip_setnetmask(ipaddr);
//	Printf_High("TcpIP IniT===\n");
	uip_gethostaddr(ipaddr); 
#endif
#if 0 
{
  u16_t tempipaddr[2];

  uip_ipaddr(tempipaddr, 0,0,0,0);
  uip_gethostaddr(ipaddr); 
  Printf_High("IP:%d.%d.%d.%d\n",
      uip_ipaddr1(ipaddr), uip_ipaddr2(ipaddr),
      uip_ipaddr3(ipaddr), uip_ipaddr4(ipaddr));

  if(!uip_ipaddr_cmp(ipaddr,tempipaddr))
  {
      Printf_High("ws_got_ip\n");

    ws_got_ip();
  }
}
#endif

  iot_udp_app_init();
  iot_tcp_app_init();


#ifndef CONFIG_SOFTAP
  if (IoTpAd.ComCfg.Use_DHCP!=1) {
	uip_ipaddr(ipaddr, IoTpAd.ComCfg.STATIC_IP[0],IoTpAd.ComCfg.STATIC_IP[1],
				IoTpAd.ComCfg.STATIC_IP[2],IoTpAd.ComCfg.STATIC_IP[3]);
	uip_sethostaddr(ipaddr);

	uip_ipaddr(ipaddr,IoTpAd.ComCfg.SubnetMask_IP[0], IoTpAd.ComCfg.SubnetMask_IP[1], 
				IoTpAd.ComCfg.SubnetMask_IP[2], IoTpAd.ComCfg.SubnetMask_IP[3]);
	uip_setnetmask(ipaddr);

	uip_ipaddr(ipaddr, IoTpAd.ComCfg.GateWay_IP[0], IoTpAd.ComCfg.GateWay_IP[1], 
				IoTpAd.ComCfg.GateWay_IP[2], IoTpAd.ComCfg.GateWay_IP[3]);
	uip_setdraddr(ipaddr);

#if CFG_SUPPORT_DNS
	uip_ipaddr(ipaddr, IoTpAd.ComCfg.DNS_IP[0],IoTpAd.ComCfg.DNS_IP[1],
				IoTpAd.ComCfg.DNS_IP[2],IoTpAd.ComCfg.DNS_IP[3]);
	resolv_conf(ipaddr);
	resolv_query(g_struZcConfigDb.struCloudInfo.u8CloudAddr);
#endif
	dhcpc_set_state(STATE_CONFIG_DONE);
  }
#endif
  cli_fd = -1;
  return 0;
} 

/*
	p: pointer to data buffer, the buffer should start with eth header.
	len: buffer len
 */
#include "../uip/uip.h"

int netif_rx(u8_t *p, u16_t len)
{
	/*avoid Rx packet length > UIP buffer size  and cause UIP buffer overflow*/
	if(len <= UIP_BUFSIZE + 2)
	{
		mt76xx_dev_read(p, len);
	}
	else 
	{
		Printf_High("netif_rx length error: %d > %d\n", len, UIP_BUFSIZE + 2);
		return -1;
	}

	
	if(uip_len > 0) {
		if(BUF->type == htons(UIP_ETHTYPE_IP)) {
			uip_arp_ipin();
			uip_input();
			/* If the above function invocation resulted in data that
			should be sent out on the network, the global variable
			uip_len is set to a value > 0. */
			if(uip_len > 0) {
				uip_arp_out();
				mt76xx_dev_send();
			}
		} else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
			uip_arp_arpin();
			/* If the above function invocation resulted in data that
			should be sent out on the network, the global variable
			uip_len is set to a value > 0. */
			if(uip_len > 0) {
				mt76xx_dev_send();
			}
		}

	}
	return 0;
}

void tcpip_periodic_timer()
{
	int i;
    MT_TimerExpired();

	if(timer_expired(&periodic_timer)) {
		timer_reset(&periodic_timer);
		for(i = 0; i < UIP_CONNS; i++) {
			uip_periodic(i);
			/* If the above function invocation resulted in data that
			should be sent out on the network, the global variable
			uip_len is set to a value > 0. */
			if(uip_len > 0) {
				uip_arp_out();
				mt76xx_dev_send();
			}
		}

#if UIP_UDP
		for(i = 0; i < UIP_UDP_CONNS; i++) {
			uip_udp_periodic(i);
			/* If the above function invocation resulted in data that
			should be sent out on the network, the global variable
			uip_len is set to a value > 0. */
			if(uip_len > 0) {
				uip_arp_out();
				mt76xx_dev_send();
			}
		}
#endif /* UIP_UDP */

		/* Call the ARP timer function every 10 seconds. */
		if(timer_expired(&arp_timer)) {
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
    }
#if 0
	if(timer_expired(&cli_timer)) {
		clk = (clk > (CLOCK_SECOND*60))?clk:(clk*2);
		timer_set(&cli_timer, clk);
		
		if ((cli_fd == -1) && 
			memcmp(uip_hostaddr, 0x00000000, sizeof(uip_hostaddr))) {
			struct uip_conn *conn = NULL;
			uip_ipaddr_t srv_ip;
			
			uip_ipaddr(srv_ip, IoTpAd.ComCfg.IoT_ServeIP[0], IoTpAd.ComCfg.IoT_ServeIP[1],
					IoTpAd.ComCfg.IoT_ServeIP[2], IoTpAd.ComCfg.IoT_ServeIP[3]);
			conn = uip_connect(&srv_ip, HTONS(IoTpAd.ComCfg.IoT_TCP_Srv_Port));
			if(conn) {
				conn->lport = HTONS(7682);
				cli_fd = conn->fd;
			} else {
				printf("connect fail\n");
			}	
		}
	}
#endif	
}
/*---------------------------------------------------------------------------*/
void
uip_log(char *m)
{
	Printf_High("UIP log: %s\n", m);
}

#ifdef __DHCPC_H__
void
dhcpc_configured(const struct dhcpc_state *s)
{
  uip_sethostaddr(s->ipaddr);
  uip_setnetmask(s->netmask);
  uip_setdraddr(s->default_router);
#if CFG_SUPPORT_DNS
  resolv_conf(s->dnsaddr);
  resolv_query(g_struZcConfigDb.struCloudInfo.u8CloudAddr);
#endif
}
#endif /* __DHCPC_H__ */
