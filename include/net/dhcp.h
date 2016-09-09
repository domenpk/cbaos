#ifndef NET_DHCP_H_
#define NET_DHCP_H_

#include <types.h>
#include <compiler.h>
#include <net.h>


/* application layer */
struct dhcp_packet {
	u8 op;
	u8 htype;
	u8 hlen;
	u8 hops;
	u8 xid[4];
	u8 secs[2];
	u8 flags[2];
	ip_t ciaddr;
	ip_t yiaddr;
	ip_t siaddr;
	ip_t giaddr;
	union {
		mac_t cmac;
		u8 chaddr[16];
	};
	u8 bootp_legacy[192];
	u8 magic[4];
	u8 options[];
};

#define DHCP_OPT_YIADDR		50
#define DHCP_OPT_IP_LEASE_TIME	51
#define DHCP_OPT_OVERLOAD	52
#define DHCP_OPT_DHCP_MESSAGE	53
#define DHCP_OPT_SIADDR		54
#define DHCP_OPT_END		0xff

#define DHCP_MESSAGE_DISCOVER	1
#define DHCP_MESSAGE_OFFER	2
#define DHCP_MESSAGE_REQUEST	3
#define DHCP_MESSAGE_ACK	5


/* this needs to be called occasionally, so it can request and renew IP */
void dhcp_work(void);

int dhcp_init(void);

#endif
