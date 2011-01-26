#ifndef _ARCH_SCHED_H_
#define _ARCH_SCHED_H_

#include <types.h>

struct task_context {
	u32 psp;
	/* registers (r0-3, r12, lr, pc, xPSR) are saved on stack on exception entry
	 * afterwards we manually save r4-11 */
};


static inline u32 ms2ticks(u32 msecs)
{
	return msecs * (CONFIG_FCPU/1000);
}

static inline u32 ticks2ms(u32 ticks)
{
	return (ticks+CONFIG_FCPU/1000-1) / (CONFIG_FCPU/1000);
}

#endif
