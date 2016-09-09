#ifndef _SEM_H_
#define _SEM_H_

#include <sched.h>
#include <lock.h>

struct sem {
	volatile int count;
	struct list tasks; /* tasks waiting on semaphore */
	struct lock lock;
};

#define SEM_DECLARE_INIT(sname, scount)            \
	struct sem sname = {                       \
		.count = scount,                   \
		.tasks = LIST_INIT(sname.tasks),   \
		.lock = LOCK_INIT,                 \
	}

void sem_init(struct sem *sem, int count);
__attribute__ ((warn_unused_result)) int down(struct sem *sem, u32 timeout);
void up(struct sem *sem);


struct mutex {
	volatile int locked;
	struct list tasks; /* tasks waiting on mutexaphore */
	struct lock lock;
};

void mutex_init(struct mutex *mutex, int count);
__attribute__ ((warn_unused_result)) int mutex_down(struct mutex *mutex, u32 timeout);
__attribute__ ((warn_unused_result)) int mutex_downtry(struct mutex *mutex);
void mutex_up(struct mutex *mutex);

#endif
