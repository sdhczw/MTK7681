#include <stdio.h>
#include <string.h>

#include "iot_udp_app.h"
#include "dhcpc.h"
#include "uip.h"
#include "uiplib.h"
#include "iot_api.h"
//#include "sample_resolv_usage.h"
//#include "sample_udp_client.h"
#include "sample_udp_server.h"

#if CFG_SUPPORT_DNS
#include "resolv.h"
#endif
#include "zc_common.h"

extern UCHAR gCurrentAddress[];
extern IOT_ADAPTER   	IoTpAd;
struct iot_udp_app_state udp_app_state;

void
iot_udp_app_init(void)
{
#ifdef CONFIG_SOFTAP
	dhcpd_init();
#else
	dhcpc_init(gCurrentAddress, 6);
#endif

#if CFG_SUPPORT_DNS
	/* DNS client. */
	resolv_init();
#endif
	/* Customer APP start. */

	/* udp_client_init(); */
	//udp_server_init();
	
	/* Customer APP end. */
	return;
}

void
iot_udp_appcall(void)
{
	UIP_UDP_CONN *udp_conn = uip_udp_conn;
	u16_t lport, rport;

	lport=HTONS(udp_conn->lport);
	rport=HTONS(udp_conn->rport);

	if(lport == DHCPC_CLIENT_PORT) {
		handle_dhcp();
#if CFG_SUPPORT_DNS
	} else if (rport == DNS_SERVER_PORT) {
		handle_resolv();
#endif
	} else if (lport == ZC_MOUDLE_PORT) {
        MT_BroadcastAppCall();
	}
#ifdef CONFIG_SOFTAP
	else if (DHCPC_SERVER_PORT == lport)
	{
		handle_dhcpd();
	}
#endif

	return;
}

