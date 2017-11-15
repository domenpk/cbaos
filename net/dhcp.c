/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <endianess.h>
#include <sched.h>
#include <timekeeping.h>
#include <types.h>
#include <net/dhcp.h>
#include <net/netpacket.h>


/*
 * DHCP summary
 * client: DISCOVER	01 01 06 00  XID  0000 0000  4x00000000  CMAC(16B)  0(192B)
 * 			63825363(MAGIC) dhcp options: 53 1 1 0xff
 * 			(UDP: src=0, sport=68, dest=255, dport=67)
 * server: OFFER	02 01 06 00  XID  0000 0000  0 YIA(your IP) SIA(server IP) GIA  CMAC(16B)  0(192B)
 * 			63825363(MAGIC) dhcp options: 53 1 2... 0xff
 * 			(UDP: src=<ip>, sport=67, dest=255, dport=68)
 * client: REQUEST	01 01 06 00  XID  0000 0000  0 0 SIA(server IP) GIA CMAC(16B)  (192B)
 * 			63825363(MAGIC) dhcp options: 53 1 3, 54 4 SIA, 50 4 YIA, 0xff
 * 			(UDP: src=0, sport=68, dest=255, dport=67)
 * server: ACK		02 01 06 00  XID  0000 0000  0 YIA(your IP) SIA(server IP) GIA  CMAC(16B) 0(192B)
 * 			63825363(MAGIC) dhcp options: 53 1 5... 0xff
 * 			(UDP: src=<ip>, sport=67, dest=255, dport=68)
 */

#define DHCP_STATE_INACTIVE	0
#define DHCP_STATE_DISCOVER	1
#define DHCP_STATE_REQUEST	2
#define DHCP_STATE_FINISHED	3

#define DHCP_MAGIC	0x63825363


static struct {
	int state;
	u32 xid;
	u32 last_op_time;
	u32 renewal_time;
} dhcp_state;

static int dhcp_negotiate_discover(void)
{
	const int len = sizeof(struct dhcp_packet)+4; /* 4B for options */
	struct netpacket *packet;
	int ret = netpacket_alloc_and_prepare_udp(&packet, ip_bcast, 68, 67, len);
	if (ret < 0)
		return ret;
	struct dhcp_packet *dhcp = &packet->dhcp;

	memset(&dhcp->op, 0, offsetof(struct dhcp_packet, magic));
	dhcp->op = 1;
	dhcp->htype = 1;
	dhcp->hlen = 6;
	dhcp_state.xid = get_be32(&netconfig.mac[2]); // should be random per dhcp session
	put_be32(dhcp->xid, dhcp_state.xid);
	memcpy(dhcp->cmac, netconfig.mac, sizeof(mac_t));
	put_be32(dhcp->magic, DHCP_MAGIC);

	dhcp->options[0] = DHCP_OPT_DHCP_MESSAGE;
	dhcp->options[1] = 1;
	dhcp->options[2] = DHCP_MESSAGE_DISCOVER;
	dhcp->options[3] = DHCP_OPT_END;

	printf("DHCP: sending DISCOVER (state: %d)\n", dhcp_state.state);
	dhcp_state.last_op_time = time();
	ret = netpacket_send(packet);

	if (ret == 0)
		dhcp_state.state = DHCP_STATE_DISCOVER;
	return ret;
}

static int dhcp_negotiate_request(ip_t siaddr, ip_t yiaddr)
{
	const int len = sizeof(struct dhcp_packet)+16; /* 3+6+6+1 B for options */
	struct netpacket *packet;
	int ret = netpacket_alloc_and_prepare_udp(&packet, ip_bcast, 68, 67, len);
	if (ret < 0)
		return ret;
	struct dhcp_packet *dhcp = &packet->dhcp;

	memset(&dhcp->op, 0, offsetof(struct dhcp_packet, magic));
	dhcp->op = 1;
	dhcp->htype = 1;
	dhcp->hlen = 6;
	put_be32(dhcp->xid, dhcp_state.xid);
	memcpy(dhcp->siaddr, siaddr, sizeof(ip_t));
	memcpy(dhcp->cmac, netconfig.mac, sizeof(mac_t));
	put_be32(dhcp->magic, DHCP_MAGIC);

	int pos = 0;
	dhcp->options[pos++] = DHCP_OPT_DHCP_MESSAGE;
	dhcp->options[pos++] = 1;
	dhcp->options[pos++] = DHCP_MESSAGE_REQUEST;

	dhcp->options[pos++] = DHCP_OPT_SIADDR;
	dhcp->options[pos++] = sizeof(ip_t);
	memcpy(&dhcp->options[pos], siaddr, sizeof(ip_t));
	pos += sizeof(ip_t);

	dhcp->options[pos++] = DHCP_OPT_YIADDR;
	dhcp->options[pos++] = sizeof(ip_t);
	memcpy(&dhcp->options[pos], yiaddr, sizeof(ip_t));
	pos += sizeof(ip_t);

	dhcp->options[pos++] = DHCP_OPT_END;

	printf("DHCP: sending REQUEST\n");
	dhcp_state.last_op_time = time();
	ret = netpacket_send(packet);

	if (ret == 0)
		dhcp_state.state = DHCP_STATE_REQUEST;
	return ret;
}

static int dhcp_handle(u8 *buf, size_t len)
{
	struct dhcp_packet *dhcp = (struct dhcp_packet *)buf;

	if (len < sizeof(*dhcp)+3) /* at least for DHCP_MESSAGE option */
		return -EINVAL;
	if (get_be32(dhcp->xid) != dhcp_state.xid)
		return -EINVAL;
	if (get_be32(dhcp->magic) != DHCP_MAGIC)
		return -EINVAL;
	if (dhcp->options[0] != DHCP_OPT_DHCP_MESSAGE || dhcp->options[1] != 1)
		return -EINVAL;

	if (dhcp->options[2] == DHCP_MESSAGE_OFFER && dhcp_state.state == DHCP_STATE_DISCOVER) {
#if 0
		ip_t *siaddr = NULL;
		ip_t *yiaddr = NULL;

		/* option parsing */
		pos = 3;
		while (sizeof(*dhcp)+pos < len && dhcp->options[pos] != DHCP_OPT_END) {
			if (dhcp->options[pos] == DHCP_OPT_SIADDR)
				siaddr = &dhcp->options[pos+2];
			if (dhcp->options[pos] == DHCP_OPT_YIADDR)
				yiaddr = &dhcp->options[pos+2];
			pos += 2+dhcp->options[pos+1];
		}

		if (!siaddr || !yiaddr) {
			printf("error: dhcpd did not provide yia or sia\n");
			dhcp_state.state = DHCP_STATE_INACTIVE;
			return -EINVAL;
		}
		return dhcp_negotiate_request(*siaddr, *yiaddr);
#endif
		printf("DHCP: received OFFER\n");
		return dhcp_negotiate_request(dhcp->siaddr, dhcp->yiaddr);
	} else
	if (dhcp->options[2] == DHCP_MESSAGE_ACK && dhcp_state.state == DHCP_STATE_REQUEST) {
		int renew = 6*3600;
#if 1
		/* option parsing */
		int pos = 3;
		while (sizeof(*dhcp)+pos < len && dhcp->options[pos] != DHCP_OPT_END) {
			if (dhcp->options[pos] == DHCP_OPT_IP_LEASE_TIME)
				renew = get_be32(&dhcp->options[pos+2]) / 2;
			pos += 2+dhcp->options[pos+1];
		}
#endif
		memcpy(netconfig.ip, dhcp->yiaddr, sizeof(ip_t));
		memcpy(netconfig.server_ip, netconfig.ip, sizeof(ip_t)); // XXX hack
		netconfig.server_ip[3] = 2;

		printf("DHCP: received ACK, my IP:%d.%d.%d.%d, renew in %ds\n", netconfig.ip[0], netconfig.ip[1], netconfig.ip[2], netconfig.ip[3], renew);
		dhcp_state.renewal_time = renew+time();
		dhcp_state.state = DHCP_STATE_FINISHED;
		netconfig.state = NET_IP; // XXX never becomes reset
		return 0;
	}
	return -EINVAL;
}

void dhcp_work(void)
{
	u32 now = time();

	if (dhcp_state.state == DHCP_STATE_INACTIVE || (dhcp_state.state != DHCP_STATE_FINISHED && time_after_eq(now, dhcp_state.last_op_time+10))) {
		printf("DHCP get ip; last:%d, now:%d\n", dhcp_state.last_op_time, now);
		dhcp_negotiate_discover();
		return;
	}

	if (dhcp_state.state == DHCP_STATE_FINISHED && time_after_eq(now, dhcp_state.renewal_time)) {
		printf("DHCP renew\n");
		dhcp_negotiate_discover();
		return;
	}
}



static struct udp_handler udp_handler_dhcp = {
	.list = LIST_INIT(udp_handler_dhcp.list),
	.port = 68,
	.handler = dhcp_handle,
};

int dhcp_init(void)
{
	return udp_register_handler(&udp_handler_dhcp);
}
