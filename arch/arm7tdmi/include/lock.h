#ifndef _ARCH_LOCK_H_
#define _ARCH_LOCK_H_

#include <types.h>

struct lock {
	u32 cpsr;
};

#define LOCK_INIT (struct lock) {}

static inline void lock_init(struct lock *lock)
{
}

/* just set/reset IRQ bit in CPSR, hrm... SPSR, i think not. ponder XXX */

static inline void lock(struct lock *lock)
{
	u32 tmp;
	asm volatile (
			"mrs	%0, CPSR\n"
			"orr	r1, %0, #0x80\n" /* IRQ bit */
			"msr	CPSR_c, r1\n"
			: "=r" (tmp) : : "r1", "memory" );
	lock->cpsr = tmp;
}

static inline void unlock(struct lock *lock)
{
	asm volatile (
			"and	%0, #0xff\n"
			"msr	CPSR_c, %0\n"
		: : "r" (lock->cpsr) : "memory" );
}

#endif
