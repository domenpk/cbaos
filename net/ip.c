/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */

#include <net/ethernet.h>
#include <net/ip.h>
#include <helpers.h>


//#define ip_print_array print_array
#define ip_print_array(tag, buf, size)


static LIST_DECLARE_INIT(ip_handler_list);


static int ip_handle(u8 *buf, unsigned len)
{
	struct ip_packet *ip = (struct ip_packet*)buf;

	ip_print_array("IP RX", buf, sizeof(*ip));
	if (len <= sizeof(*ip))
		return 0;

	len -= sizeof(*ip);

	if (ip->vihl != 0x45) /* IPv4, 20B header */
		goto ack;

	struct list *it;
	list_for_each(&ip_handler_list, it) {
		struct ip_handler *han = list_entry(it, struct ip_handler, list);

		if (ip->protocol == han->protocol)
			return han->handler(ip->payload, len);
	}
	return -1;

 ack:
	return 0;
}

static struct eth_handler eth_handler_ip = {
	.list = LIST_INIT(eth_handler_ip.list),
	.ethertype = ETHERTYPE_IP,
	.handler = ip_handle,
};

int ip_init(void)
{
	return eth_register_handler(&eth_handler_ip);
}

int ip_register_handler(struct ip_handler *han)
{
	list_add_tail(&ip_handler_list, &han->list);
	return 0;
}
