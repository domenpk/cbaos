#ifndef ATOMIC_H_
#define ATOMIC_H_

#include <lock.h>
#include <arch/atomic.h>

struct atomic {
	struct lock l;
	int value;
};

/* generic implementations. feel free to replace with more efficient arch ones */
static inline void atomic_inc(struct atomic *a)
{
	lock(&a->l);
	a->value++;
	unlock(&a->l);
}

static inline int atomic_dec_and_test(struct atomic *a)
{
	int r;
	lock(&a->l);
	r = --a->value;
	unlock(&a->l);
	return r == 0;
}

static inline void atomic_init(struct atomic *a)
{
	lock_init(&a->l);
	a->value = 0;
}

static inline void atomic_bit_set(struct atomic *a, u32 bit, u8 value)
{
	lock(&a->l);
	if (value)
		a->value |= 1<<bit;
	else
		a->value &= ~(1<<bit);
	unlock(&a->l);
}

static inline int atomic_bit_get(struct atomic *a, u32 bit)
{
	return (a->value>>bit)&1;
}

#endif
