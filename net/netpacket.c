#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <endianess.h>
#include <bip_buf.h>
#include <net/netpacket.h>

//#define NETPACKET_DEBUG

/* these are just wrappers and handlers for memory etc.
 * should contain no code that talks to hw
 */


#ifdef NETPACKET_DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif

// will not work for odd lengths
u16 ip_csum(u8 *src, int len)
{
	unsigned csum = 0;
	int i;

	for (i=0; i<len; i+=2)
		csum += (src[i]<<8) + src[i+1];

	csum = (csum&0xffff)+(csum>>16);
	csum = (csum&0xffff)+(csum>>16); /* in some cases it overflows again */
	return ~csum;
}

const static u8 ip_template[] = { 0x45, 0, 0,0, 0,0, 0,0, 0x20 };

/*
 * netpacket_prepare_ip
 * Can fail if IP is not yet known. In this case it sends an arp request.
 * Then fails. User should resubmit the packet in a second or so.
 */
int netpacket_prepare_ip(struct netpacket *packet, const ip_t dest, u16 payload_len)
{
	struct ip_packet *ip = &packet->ip;
	struct ethernet_frame *ethernet = &packet->ethernet;

	memcpy(packet->ip.raw, ip_template, sizeof(ip_template));
	ip->protocol = IP_PROT_UDP;
	put_be16(ip->len, payload_len + sizeof(struct ip_packet));
	ip->hcsum[0] = ip->hcsum[1] = 0;
	memcpy(ip->src, netconfig.ip, sizeof(ip_t));
	memcpy(ip->dest, dest, sizeof(ip_t));

	u16 csum = ip_csum(packet->ip.raw, 20);
	ip->hcsum[0] = csum>>8;
	ip->hcsum[1] = csum;

	memcpy(ethernet->src, netconfig.mac, sizeof(mac_t));
	if (arp_ip2mac(ethernet->dest, dest) < 0) {
		netpacket_alloc_discard();
		int ret = arp_request(dest);
		/* now we must fail the current packet, since we can't send before getting arp reply */
		printf("%s: mac not cached, arp request: %d\n", __func__, ret);
		return -EAGAIN;
	}

	put_be16(ethernet->type, ETHERTYPE_IP);
	return 0;
}

int netpacket_prepare_udp(struct netpacket *packet, const ip_t dest, u16 sport, u16 dport, u16 payload_len)
{
	int ret;

	ret = netpacket_prepare_ip(packet, dest, payload_len+sizeof(struct udp_packet));
	if (ret)
		return ret;

	put_be16(packet->udp.sport, sport);
	put_be16(packet->udp.dport, dport);
	put_be16(packet->udp.len, payload_len+sizeof(struct udp_packet));
	put_be16(packet->udp.checksum, 0); /* it is optional */

	return 0;
}

static struct netpacket *netpacket_alloc_udp(int payloadlen)
{
	return netpacket_alloc(offsetof(struct netpacket, udp) + sizeof(struct udp_packet) + payloadlen);
}

int netpacket_alloc_and_prepare_udp(struct netpacket **_packet, const ip_t dest, u16 sport, u16 dport, u16 len)
{
	struct netpacket *packet = netpacket_alloc_udp(len);
	if (!packet)
		return -ENOMEM;
	if (netpacket_prepare_udp(packet, dest, sport, dport, len) < 0) {
		/* no need to deallocate, since we haven't called commit yet */
		return -EAGAIN;
	}
	*_packet = packet;
	return 0;
}


static u8 netpacket_buf_data[1024];
static struct bip_buf netpacket_buf;
static int netpacket_in_alloc; // XXX consider just having a spinlock - code shouldn't block anyway
struct netpacket *netpacket_alloc(unsigned len)
{
	if (netpacket_in_alloc) {
		dprintf("%s(%d): in alloc!\n", __func__, len);
		return NULL;
	}

	if (!netpacket_buf.data) // XXX i don't like this
		bip_buf_init(&netpacket_buf, netpacket_buf_data, sizeof(netpacket_buf_data));

	struct netpacket *packet = bip_buf_alloc(&netpacket_buf, len);
	dprintf("%s(%d): %p\n", __func__, len, packet);
	if (packet) {
		packet->len = len;
		netpacket_in_alloc = 1;
	}
	return packet;
}

void netpacket_alloc_discard(void)
{
	dprintf("%s\n", __func__);
	netpacket_in_alloc = 0;
}

int netpacket_send(struct netpacket *packet)
{
	printf("TX ");
	netpacket_print(packet);
	bip_buf_alloc_commit(&netpacket_buf, packet->len);
	dprintf("%s(%d)\n", __func__, packet->len);
	netpacket_in_alloc = 0;
	return 0;
}

struct netpacket* netpacket_getdata(void)
{
	return bip_buf_getdata(&netpacket_buf);
}

void netpacket_free(struct netpacket *packet)
{
	bip_buf_free(&netpacket_buf, packet->len);
	dprintf("%s(%d)\n", __func__, packet->len);
}

void netpacket_print(struct netpacket *packet)
{
	u16 type = get_be16(packet->ethernet.type);

	printf("ethernet: type:%x ", type);
	if (type == ETHERTYPE_ARP) {
		printf("ARP %d", packet->arp.oper[1]);
	} else if (type == ETHERTYPE_IP) {
		u8 *ip = &packet->ip.src[0];
		printf("TCP ");
		printf("%d.%d.%d.%d ", ip[0], ip[1], ip[2], ip[3]);
		ip = &packet->ip.dest[0];
		printf("%d.%d.%d.%d ", ip[0], ip[1], ip[2], ip[3]);

		int prot = packet->ip.protocol;
		printf("protocol: %d ", prot);
		if (prot == IP_PROT_UDP) {
			printf("UDP %d %d", get_be16(packet->udp.sport), get_be16(packet->udp.dport));
		}
	}
	printf("\n");
#ifdef DEBUG_NETPACKET
	int i;
	for (i=0; i<len; i++) {
		if (i == 14 || i == 14+20)
			printf(" |");
		printf(" %02x", packet->ethernet.raw[i]);
	}
	printf("\n");
#endif
}
