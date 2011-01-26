#ifndef _ARCH_LOCK_H_
#define _ARCH_LOCK_H_

#include <signal.h>
#include <stdlib.h>

typedef sigset_t lock_state_t;

struct lock {
	lock_state_t lock_state;
};

#define LOCK_INIT (struct lock) {}

static inline void lock_init(struct lock *lock)
{
}

static inline void lock(struct lock *lock)
{
	sigset_t sigs;
	sigemptyset(&sigs);
	sigaddset(&sigs, SIGALRM);
	sigprocmask(SIG_BLOCK, &sigs, &lock->lock_state);
}

static inline void unlock(struct lock *lock)
{
	sigprocmask(SIG_SETMASK, &lock->lock_state, NULL);
}

#endif
