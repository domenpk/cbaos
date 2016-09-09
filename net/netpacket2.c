#include <errno.h>
#include <stdio.h>

#include <net/netpacket.h>


/* send one packet that's queued for sending */
int netpacket_work(void)
{
	struct netpacket *packet = netpacket_getdata();
	if (!packet)
		return -EAGAIN;
	int len = packet->len;
	int ret = enc28j60_raw_send(packet->hw_dependent, len-offsetof(struct netpacket, hw_dependent));
	if (ret == 0)
		netpacket_free(packet);
	return ret;
}

int netpacket_handle(struct netpacket *packet)
{
	printf("RX ");
	netpacket_print(packet);
	return ethernet_handle(packet->ethernet.raw, packet->len-offsetof(struct netpacket, ethernet));
}


int net_init(void)
{
	/* lower layer needs to be initialised before upper */
	// XXX add handlers
	eth_init();
	arp_init();
	ip_init();
	udp_init();
	dhcp_init();
}
