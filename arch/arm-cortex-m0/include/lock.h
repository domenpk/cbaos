#ifndef _ARCH_LOCK_H_
#define _ARCH_LOCK_H_

#include <types.h>

struct lock {
	u32 primask;
};

#define LOCK_INIT (struct lock) {}

static inline void lock_init(struct lock *lock)
{
}

// source:
// http://embeddedfreak.wordpress.com/2009/08/14/cortex-m3-global-interruptexception-control/
//
// confirm this is correct by printing primask before after

// note arm architecture v7m rm... setting PRIMASK raises priority to 0

static inline void lock(struct lock *lock)
{
	u32 tmp;
	asm volatile (
		"mrs	%0, PRIMASK\n\t"  /* 4 cycles */
		"cpsid	i\n\t"            /* 1 cycle */
		: "=r" (tmp) : : "memory" );
	lock->primask = tmp;
}

static inline void unlock(struct lock *lock)
{
	asm volatile (
		"msr	PRIMASK, %0\n\t"  /* 4 cycles */
		: : "r" (lock->primask) : "memory" );
}

#endif
