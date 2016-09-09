#ifndef UDP_H_
#define UDP_H_

#include <types.h>
#include <compiler.h>
#include <list.h>

struct udp_packet {
	u8 sport[2];
	u8 dport[2];
	u8 len[2];
	u8 checksum[2];
	u8 payload[0];
} __packed;


struct udp_handler {
	struct list list;
	u16 port;
	int (*handler)(u8 *buf, size_t len);
};

int udp_init(void);
int udp_register_handler(struct udp_handler *han);

#endif
