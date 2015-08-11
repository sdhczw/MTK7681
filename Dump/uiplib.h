#ifndef  __ZC_UIPLIBDMUP_H__ 
#define  __ZC_UIPLIBDMUP_H__

#define uip_udp_bind(conn, port) (conn)->lport = port

#define uip_ipaddr(addr, addr0,addr1,addr2,addr3) do { \
    ((u16_t *)(addr))[0] = ZC_HTONS(((addr0) << 8) | (addr1)); \
    ((u16_t *)(addr))[1] = ZC_HTONS(((addr2) << 8) | (addr3)); \
                  } while(0)

#define uip_ip6addr(addr, addr0,addr1,addr2,addr3,addr4,addr5,addr6,addr7) do { \
    ((u16_t *)(addr))[0] = ZC_HTONS((addr0)); \
    ((u16_t *)(addr))[1] = ZC_HTONS((addr1)); \
    ((u16_t *)(addr))[2] = ZC_HTONS((addr2)); \
    ((u16_t *)(addr))[3] = ZC_HTONS((addr3)); \
    ((u16_t *)(addr))[4] = ZC_HTONS((addr4)); \
    ((u16_t *)(addr))[5] = ZC_HTONS((addr5)); \
    ((u16_t *)(addr))[6] = ZC_HTONS((addr6)); \
    ((u16_t *)(addr))[7] = ZC_HTONS((addr7)); \
                  } while(0)

#define UIP_ACKDATA   1     /* Signifies that the outstanding data was
			       acked and the application should send
                   out new data instead of retransmitting
                   the last data. */
#define UIP_NEWDATA   2     /* Flags the fact that the peer has sent
                   us new data. */
#define UIP_REXMIT    4     /* Tells the application to retransmit the
                   data that was last sent. */
#define UIP_POLL      8     /* Used for polling the application, to
                   check if the application has data that
                   it wants to send. */
#define UIP_CLOSE     16    /* The remote host has closed the
                   connection, thus the connection has
                   gone away. Or the application signals
                   that it wants to close the
                   connection. */
#define UIP_ABORT     32    /* The remote host has aborted the
                   connection, thus the connection has
                   gone away. Or the application signals
                   that it wants to abort the
                   connection. */
#define UIP_CONNECTED 64    /* We have got a connection from a remote
                   host and have set up a new connection
                   for it, or an active connection has
                   been successfully established. */

#define UIP_TIMEDOUT  128   /* The connection has been aborted due to
                   too many retransmissions. */
                   
#define uip_connected() (uip_flags & UIP_CONNECTED)
#define uip_timedout()    (uip_flags & UIP_TIMEDOUT)
#define uip_acked()   (uip_flags & UIP_ACKDATA)
#define uip_newdata()   (uip_flags & UIP_NEWDATA)
#define uip_rexmit()     (uip_flags & UIP_REXMIT)
#define uip_poll()       (uip_flags & UIP_POLL)
#define uip_closed()    (uip_flags & UIP_CLOSE)
#define uip_aborted()    (uip_flags & UIP_ABORT)

#define uip_datalen()       uip_len
#define uip_close()         (uip_flags = UIP_CLOSE)

#define uip_abort()         (uip_flags = UIP_ABORT)

#define UIP_CONF_BYTE_ORDER      UIP_LITTLE_ENDIAN

#ifdef UIP_CONF_BYTE_ORDER
#define UIP_BYTE_ORDER     UIP_CONF_BYTE_ORDER
#else /* UIP_CONF_BYTE_ORDER */
#define UIP_BYTE_ORDER     UIP_LITTLE_ENDIAN
#endif /* UIP_CONF_BYTE_ORDER */

#ifndef UIP_LITTLE_ENDIAN
#define UIP_LITTLE_ENDIAN  3412
#endif /* UIP_LITTLE_ENDIAN */
#ifndef UIP_BIG_ENDIAN
#define UIP_BIG_ENDIAN     1234
#endif /* UIP_BIG_ENDIAN */
void MT_CloudAppCall();
#define uip_poll_conn(conn) do { uip_flags = UIP_POLL; \
    MT_CloudAppCall();\
    } while (0)
#if 0
#ifndef HTONS
#   if UIP_BYTE_ORDER == UIP_BIG_ENDIAN
#      define HTONS(n) (n)
#   else /* UIP_BYTE_ORDER == UIP_BIG_ENDIAN */
#      define HTONS(n) (u16_t)((((u16_t) (n)) << 8) | (((u16_t) (n)) >> 8))
#   endif /* UIP_BYTE_ORDER == UIP_BIG_ENDIAN */
#else
#error "HTONS already defined!"
#endif /* HTONS */

#define htons(n) (n)
#endif
#endif
