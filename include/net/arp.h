#ifndef ARP_H_
#define ARP_H_

#include <compiler.h>
#include <types.h>
#include <net.h>


int arp_init(void);

int arp_ip2mac(mac_t mac, const ip_t ip);
void arp_table_put(const mac_t mac, const ip_t ip);
int arp_request(const ip_t ip);


struct arp_packet {
	union {
		u8 raw[28];
		struct {
			u8 htype[2];
			u8 ptype[2];
			u8 hlen;
			u8 plen;
			u8 oper[2];
			mac_t smac;
			ip_t sip;
			mac_t tmac;
			ip_t tip;
		};
	};
} __packed;


#endif
