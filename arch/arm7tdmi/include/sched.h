#ifndef _ARCH_SCHED_H_
#define _ARCH_SCHED_H_

#include <types.h>

struct task_context {
	u32 psp;
	/* no registers are saved on exception entry.
	 * sp, lr and xPSR are banked though */
};


static inline u32 ms2ticks(u32 msecs)
{
	return msecs * 1000;
}

static inline u32 ticks2ms(u32 ticks)
{
	return (ticks+1000-1) / 1000;
}

#endif
