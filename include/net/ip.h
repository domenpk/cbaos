#ifndef IP_H_
#define IP_H_

#include <list.h>
#include <types.h>
#include <compiler.h>
#include <net.h>


/* internet layer */
#define IP_PROT_UDP	0x11

struct ip_packet {
	union {
		u8 raw[20];
		struct {
			u8 vihl;
			u8 dscpecn;
			u8 len[2];

			u8 id[2];
			u8 flags_foff[2];

			u8 ttl;
			u8 protocol;
			u8 hcsum[2];

			ip_t src;
			ip_t dest;
			u8 payload[0];
		};
	};
} __packed;


struct ip_handler {
	struct list list;
	u8 protocol;
	int (*handler)(u8 *buf, size_t len);
};

int ip_init(void);
int ip_register_handler(struct ip_handler *han);

#endif
