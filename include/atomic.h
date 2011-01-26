#ifndef _ATOMIC_H_
#define _ATOMIC_H_

struct atomic {
	int value;
};

/* generic implementations. feel free to replace with more efficient arch ones */
static inline void atomic_inc(struct atomic *a)
{
	struct lock l = LOCK_INIT;
	lock(&l);
	a->value++;
	unlock(&l);
}

static inline int atomic_dec_and_test(struct atomic *a)
{
	int r;
	struct lock l = LOCK_INIT;
	lock(&l);
	r = --a->value;
	unlock(&l);
	return r == 0;
}

#endif
