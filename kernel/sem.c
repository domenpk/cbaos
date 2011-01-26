/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <sem.h>
#include <lock.h>
#include <sched.h>

#include <errno.h>
#include <stdio.h>


void sem_init(struct sem *sem, int count)
{
	sem->count = count;
	list_init(&sem->tasks);
	lock_init(&sem->lock);
}

__attribute__ ((warn_unused_result)) int down(struct sem *sem, u32 timeout)
{
	/* count and list operations are protected by lock */
	lock(&sem->lock);
	list_add_tail(&sem->tasks, &current->waiting);

//printf("%s: %p, count:%i\n", __func__, sem, sem->count);
	do {
		// SMP: while (TAS(&foo)) ;
		if (sem->count > 0) {
			sem->count--;
			/* success! */
			current->dont_reschedule = 0;
			list_del(&current->waiting);
			// SMP: foo = 0
			unlock(&sem->lock);
//printf("%s: %p, count:%i, ok\n", __func__, sem, sem->count);
			return 0;
		}
		// SMP: foo = 0
		unlock(&sem->lock);

		/* if up() is called here, don't reschedule will be set, and
		 * sched_timeout will return */

		timeout = sched_timeout(timeout);
		lock(&sem->lock);

	} while (timeout > 0);

	list_del(&current->waiting);
//printf("%s: %p, count:%i, fail\n", __func__, sem, sem->count);
	unlock(&sem->lock);

	return -ETIMEDOUT;
}

void up(struct sem *sem)
{
	struct task *task;
	int resched = 0;
	lock(&sem->lock);
	// SMP: while (TAS(&foo)) ;
	if (!list_empty(&sem->tasks)) {
		task = list_entry(sem->tasks.next, struct task, waiting);
		task->dont_reschedule = 1;
		task_wake(task);
//		fprintf(stderr, "waking %s\n", task->name);
		resched = 1;
	}
	sem->count++;
//printf("%s: %p, count:%i\n", __func__, sem, sem->count);
	// SMP: foo = 0
	unlock(&sem->lock);
	/* also call scheduler? well... maybe, but only !in_interrupt() */

	/* must call scheduler, to get the woken task running */
	if (resched)
		arch_sched_next_interrupt(1); /* now, this is ok, since we only schedule it. but it's still some delay that i don't like */
//		sched_yield();
//		arch_task_switch(task);
	// ^ this is problematic... if up() is called from interrupt, then it's not quite normal to call SVC here, were not in task context! fuck
}