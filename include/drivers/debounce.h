#ifndef DEBOUNCE_H_
#define DEBOUNCE_H_

#include <types.h>
#include <lock.h>
#include <list.h>


enum debounce_settings {
	DEBOUNCE_FALLING = 0x1,
	DEBOUNCE_RISING  = 0x2,
};

struct debounce_info {
	struct list list;
	int (*callback)(int pin, int value); /* context of debounce_periodic */
	int pin;
	enum debounce_settings settings; /* mask of settings */
	u32 history;
};


int debounce_register(struct debounce_info *deb);
int debounce_periodic(void);

#endif
