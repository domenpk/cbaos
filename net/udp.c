/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */

#include <list.h>
#include <net/ip.h>
#include <net/udp.h>
#include <helpers.h>


//#define udp_print_array print_array
#define udp_print_array(tag, buf, size)


static LIST_DECLARE_INIT(udp_handler_list);


static int udp_handle(u8 *buf, size_t len)
{
	struct udp_packet *udp = (struct udp_packet *)buf;
	int l = get_be16(udp->len);
	int port = get_be16(udp->dport);

	if (len > l)
		len = l;

	udp_print_array("UDX RX", buf, sizeof(*udp));
	/* no data? */
	if (len <= sizeof(*udp))
		return 0;

	/* skip ports, len, checksum for now */
	len -= sizeof(*udp);

	udp_print_array("UDX RX data", udp->payload, len);

	struct list *it;
	list_for_each(&udp_handler_list, it) {
		struct udp_handler *han = list_entry(it, struct udp_handler, list);

		if (port == han->port)
			return han->handler(udp->payload, len);
	}

	return -1;
}

static struct ip_handler ip_handler_udp = {
	.list = LIST_INIT(ip_handler_udp.list),
	.protocol = IP_PROT_UDP,
	.handler = udp_handle,
};

int udp_init(void)
{
	return ip_register_handler(&ip_handler_udp);
}

int udp_register_handler(struct udp_handler *han)
{
	list_add_tail(&udp_handler_list, &han->list);
	return 0;
}
