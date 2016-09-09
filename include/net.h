#ifndef NET_H_
#define NET_H_

#include <types.h>


typedef u8 ip_t[4];
typedef u8 mac_t[6];

enum network_state { NET_UNCONFIGURED, NET_IP };
struct netconfig {
	mac_t mac;
	ip_t ip;
	ip_t server_ip;
	enum network_state state;
};

extern struct netconfig netconfig;

#endif
