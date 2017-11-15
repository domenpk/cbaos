/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */

#include <string.h>
#include <errno.h>

#include <net.h>
#include <net/ethernet.h>
#include <net/arp.h>
#include <net/netpacket.h>
#include <helpers.h>
#include <timekeeping.h>


//#define arp_print_array print_array
#define arp_print_array(tag, buf, size)

const mac_t mac_bcast = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
const ip_t ip_bcast = { 0xff, 0xff, 0xff, 0xff };
const ip_t ip_unused = { };
#define MAC_BCAST { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }
#define IP_BCAST { 0xff, 0xff, 0xff, 0xff }



/* arp resolution */

/*
 * Idea here is that least recently used arp table entry will get replaced
 * by a new one. To prevent arp flooding attacks, we set new arp table
 * entries to be X seconds in the past. When a packet is sent to a host,
 * the time will be updated. Ah, it can easily be spoofed then also.
 * But at least it will still work if done accidentally :)
 */

#define ARP_TABLE_LEN 3
//#define ARP_TABLE_LEN 5
#define ARP_PACKET_FAKE_AGE 1800 // X from above comment

struct arp_entry {
	mac_t mac;
	ip_t ip;
	u32 last_used_time;
};

static struct arp_entry arp_table[ARP_TABLE_LEN] = {
	{ .mac = MAC_BCAST, .ip = IP_BCAST }
	/* the rest are implicitly ip_unused */
};

static int arp_table_find_by_ip(const ip_t ip)
{
	int i;
	for (i=0; i<ALEN(arp_table); i++) {
		if (memcmp(ip, arp_table[i].ip, sizeof(ip_t)) == 0)
			return i;
	}
	return -ENOENT;
}

static int arp_table_find_free_or_lru(void)
{
	int i;
	u32 lru_time = arp_table[1].last_used_time;
	u32 lru = 1;

	/* 0 is special, skip */
	for (i=1; i<ALEN(arp_table); i++) {
		if (memcmp(ip_unused, arp_table[i].ip, sizeof(ip_t)) == 0)
			return i;
		if ((s32)lru_time - (s32)arp_table[i].last_used_time < 0) {
			lru_time = arp_table[i].last_used_time;
			lru = i;
		}
	}

	/* always returns a valid index */
	return lru;
}

int arp_ip2mac(mac_t mac, const ip_t ip)
{
	int i;
	i = arp_table_find_by_ip(ip);
	if (i < 0)
		return i;

	arp_table[i].last_used_time = time();

	memcpy(mac, arp_table[i].mac, sizeof(mac_t));
	return 0;
}

void arp_table_put(const mac_t mac, const ip_t ip)
{
	int i;
	i = arp_table_find_by_ip(ip);
	if (i < 0)
		i = arp_table_find_free_or_lru();

	memcpy(arp_table[i].mac, mac, sizeof(mac_t));
	memcpy(arp_table[i].ip, ip, sizeof(ip_t));
	arp_table[i].last_used_time = time() - ARP_PACKET_FAKE_AGE;
}

#define ARP_OPER_LO_REQUEST	0x01
#define ARP_OPER_LO_REPLY	0x02
static const u8 arp_req_template[7] = { 0x00, 0x01, 0x08, 0x00, 6, 4, 0x00 };


static int arp_handle(u8 *buf, size_t len)
{
	struct arp_packet *arprx = (struct arp_packet *)buf;

	arp_print_array("RX arp", buf, sizeof(struct arp_packet));

	/* because of padding frame will be >28B! */
	if (len < sizeof(struct arp_packet))
		return -1;
	len = sizeof(*arprx);
	if (memcmp(buf, arp_req_template, sizeof(arp_req_template)) != 0)
		return -1;

	if (memcmp(arprx->tip, netconfig.ip, sizeof(ip_t)) != 0)
		return -1;

	arp_table_put(arprx->smac, arprx->sip);

	/* send a reply with my MAC */
	struct netpacket *packet = netpacket_alloc(offsetof(struct netpacket, arp) + sizeof(struct arp_packet));
	if (!packet)
		return -ENOMEM;
	struct arp_packet *arptx = &packet->arp;
	struct ethernet_frame *ethernet = &packet->ethernet;

	memcpy(arptx->raw, arp_req_template, sizeof(arp_req_template));
	arptx->oper[1] = ARP_OPER_LO_REPLY;
	memcpy(arptx->tmac, arprx->smac, sizeof(mac_t) + sizeof(ip_t)); /* switch src/dest */
	memcpy(arptx->smac, netconfig.mac, sizeof(netconfig.mac)); /* my mac */
	memcpy(arptx->sip, netconfig.ip, sizeof(netconfig.ip)); /* my ip */

	arp_print_array("TX arp", arptx->raw, sizeof(struct arp_packet));

	memcpy(ethernet->src, netconfig.mac, sizeof(mac_t));
	memcpy(ethernet->dest, arptx->tmac, sizeof(mac_t));
	put_be16(ethernet->type, ETHERTYPE_ARP);

	return netpacket_send(packet);
}

int arp_request(const ip_t ip)
{
	struct netpacket *packet = netpacket_alloc(offsetof(struct netpacket, arp) + sizeof(struct arp_packet));
	if (!packet)
		return -ENOMEM;
	struct arp_packet *arptx = &packet->arp;
	struct ethernet_frame *ethernet = &packet->ethernet;

	memcpy(arptx->raw, arp_req_template, sizeof(arp_req_template));
	arptx->oper[1] = ARP_OPER_LO_REQUEST;
	memcpy(arptx->smac, netconfig.mac, sizeof(netconfig.mac)); /* my mac */
	memcpy(arptx->sip, netconfig.ip, sizeof(netconfig.ip)); /* my ip */
	memcpy(arptx->tip, ip, sizeof(ip_t)); /* asking for MAC of this IP */
	/* tmac is don't care */

	arp_print_array("TX arprq", arptx->raw, sizeof(struct arp_packet));

	memcpy(ethernet->src, netconfig.mac, sizeof(mac_t));
	memcpy(ethernet->dest, mac_bcast, sizeof(mac_t));
	put_be16(ethernet->type, ETHERTYPE_ARP);

	return netpacket_send(packet);
}


static struct eth_handler eth_handler_arp = {
	.list = LIST_INIT(eth_handler_arp.list),
	.ethertype = ETHERTYPE_ARP,
	.handler = arp_handle,
};

int arp_init(void)
{
	return eth_register_handler(&eth_handler_arp);
}
