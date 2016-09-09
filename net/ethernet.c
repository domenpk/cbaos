/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */

#include <net/ethernet.h>
#include <helpers.h>


//#define eth_print_array print_array
#define eth_print_array(tag, buf, size)


static LIST_DECLARE_INIT(eth_handler_list);


int ethernet_handle(u8 *buf, unsigned len)
{
	struct ethernet_frame *frame = (struct ethernet_frame*)buf;

	eth_print_array("ETH RX", buf, sizeof(*frame));
	if (len <= sizeof(*frame))
		return 0;

	len -= sizeof(*frame);

	u16 type = get_be16(frame->type);
//	printf("eth:type=%x\t", type);

	struct list *it;
	list_for_each(&eth_handler_list, it) {
		struct eth_handler *han = list_entry(it, struct eth_handler, list);

		if (type == han->ethertype)
			return han->handler(frame->payload, len);
	}

	return 0;
}

int eth_init(void)
{
//	eth_register_handler(&eth_handler_ip); XXX
	return 0;
}

int eth_register_handler(struct eth_handler *han)
{
	list_add_tail(&eth_handler_list, &han->list);
	return 0;
}
