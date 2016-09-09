#ifndef NETPACKET_H_
#define NETPACKET_H_

#include <types.h>
#include <net.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/dhcp.h>
#include <net/ethernet.h>
#include <net/netpacket.h>



struct netpacket {
	u16 len; /* len of ethernet-> */
	u8 hw_dependent[2]; /* for enc28j60 SPI tranfer */
	struct ethernet_frame ethernet;
	union {
		struct arp_packet arp;
		struct {
			struct ip_packet ip;
			struct udp_packet udp;
			struct dhcp_packet dhcp;
		};
	};
} __packed;

int netpacket_alloc_and_prepare_udp(struct netpacket **_packet, const ip_t dest, u16 sport, u16 dport, u16 len);
struct netpacket *netpacket_alloc(unsigned len);
void netpacket_alloc_discard(void);
int netpacket_send(struct netpacket *packet);

struct netpacket* netpacket_getdata(void);
void netpacket_free(struct netpacket *packet);

void netpacket_print(struct netpacket *packet);


int netpacket_work(void);
int netpacket_handle(struct netpacket *packet);

#endif
