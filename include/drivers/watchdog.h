#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

enum watchdog_type {
	WATCHDOG_INTERRUPT,
	WATCHDOG_RESET
};

/* timeout is in seconds. and is +-40%
 * Note that the maximum here is about 4e6 (1000 hours)
 */
void watchdog_setup(enum watchdog_type type, u32 timeout);
void watchdog_ping(void);

#endif
