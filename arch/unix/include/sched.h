#ifndef _ARCH_SCHED_H_
#define _ARCH_SCHED_H_

#include <types.h>

#include <ucontext.h>

struct task_context {
	ucontext_t uc;
};

typedef sigset_t sched_ctx_t;

static inline u32 ms2ticks(u32 msecs)
{
	return msecs * 1000;
}

static inline u32 ticks2ms(u32 ticks)
{
	return (ticks+999) / 1000;
}

#endif
