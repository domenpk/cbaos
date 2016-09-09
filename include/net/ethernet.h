#ifndef ETHERNET_H_
#define ETHERNET_H_

#include <list.h>
#include <types.h>
#include <compiler.h>
#include <net.h>


/* link layer */
#define ETHERTYPE_IP    0x0800
#define ETHERTYPE_ARP   0x0806

struct ethernet_frame {
	union {
		u8 raw[14];
		struct {
			mac_t dest;
			mac_t src;
			u8 type[2];
			u8 payload[0];
		};
	};
} __packed;


struct eth_handler {
	struct list list;
	u16 ethertype;
	int (*handler)(u8 *buf, size_t len);
};

int eth_init(void);
int eth_register_handler(struct eth_handler *han);

extern const mac_t mac_bcast; // XXX move elsewhere
extern const ip_t ip_bcast;

#endif
