#ifndef _SCHED_H_
#define _SCHED_H_

#include <types.h>
#include <list.h>

#include <arch/sched.h>

#define TIME_SLICE 100
#define TASK_PRIORITIES 3

struct task {
	struct list list;
	const char *name;
	u32 *stack;
	int stack_len;       /* how many stack words */

	u32 timeout;         /* only valid when on waiting list */
	int prio;            /* 0 is highest, TASK_PRIORITIES-1 is lowest */
	int dont_reschedule; /* if this is set to 1, task will not be rescheduled */

	struct list waiting; /* tasks waiting on same semaphore */

	struct task_context context;
};
extern struct task *current;

static inline int time_before(u32 a, u32 b)
{
	return (s32)(a - b) < 0;
	//return a - b >= 0x80000000;
}

static inline int time_after(u32 a, u32 b)
{
	return (s32)(b - a) < 0;
	//return b - a >= 0x80000000;
}

u32 msleep(u32 msecs);
u32 ticks_now(void);

void sched_init(void);
void sched_start(void);
int task_new(struct task *task, void (*func)(u32 arg), u32 arg, int prio);

/* internal and debugging stuff */
void sched_yield(void); /* is there a valid use? */
void mdelay(int msecs);

void sched_interrupt(void);
void task_printall(void);
u32 sched_timeout(u32 ticks);
void task_wake(struct task *task);

/* internally used arch stuff */
void arch_task_new(struct task *task, void (*func)(u32 arg), u32 arg);
void arch_task_switch(struct task *newt);
void arch_task_first(struct task *task);
void arch_sched_start_timer(void);
void arch_wait_for_interrupt(void);
void arch_sched_next_interrupt(int offset);
u32 arch_ticks_now(void);

#endif
