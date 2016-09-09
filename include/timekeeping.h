#ifndef TIMEKEEPING_H_
#define TIMEKEEPING_H_

#include <types.h>

void timekeeping(u32 now);
u32 time_ms(void);

/* limit: 49.7 days in seconds or ms */
static inline u32 time(void)
{
	return time_ms()/1000;
}

#endif
