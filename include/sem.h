#ifndef _SEM_H_
#define _SEM_H_

#include <sched.h>
#include <lock.h>

struct sem {
	volatile int count;
	struct list tasks; /* tasks waiting on semaphore */
	struct lock lock;
};

void sem_init(struct sem *sem, int count);
__attribute__ ((warn_unused_result)) int down(struct sem *sem, u32 timeout);
void up(struct sem *sem);

#endif
