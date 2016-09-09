/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <sched.h>
#include <timekeeping.h>
#include <lock.h>
#include <compiler.h>

#include <stdio.h>
#include <assert.h>

/*
 * In scheduling, priority inversion is the scenario where a low priority task holds a shared resource that is required by a high priority task. This causes the execution of the high priority task to be blocked until the low priority task has released the resource, effectively "inverting" the relative priorities of the two tasks. If some other medium priority task, one that does not depend on the shared resource, attempts to run in the interim, it will take precedence over both the low priority task and the high priority task.
 *
 * is this a real life problem, or just bad design problem?
 */

/* TODO sched stats per task */

/* XXX should rethink all of this, write nicer code, write faster up() task
 * wakeup and nice test cases for tasks and semaphores (also for the case:
 *   up() from irq that wakes the task that will replace &cba
 */


#define sched_ctx_t struct lock
#define sched_lock lock
#define sched_unlock unlock

struct task cba;
#ifdef ARCH_UNIX
static u32 cba_stack[2048];
#else
static u32 cba_stack[32]; /* on cortex-m3 16+2 actually seems enough */
#endif

u32 sched_time_started;


struct task *current;

struct list tasks_ready[TASK_PRIORITIES];
struct list tasks_waiting[TASK_PRIORITIES];

int task_new(struct task *task, void (*func)(u32 arg), u32 arg, int prio)
{
	int i;

	printf("%s: %s with prio:%i\n", __func__, task->name, prio);
	for (i=0; i<task->stack_len; i++)
		task->stack[i] = MAGIC_STACK_POISON;

	assert(prio < TASK_PRIORITIES);

	arch_task_new(task, func, arg);

	if (task == &cba)
		return 0;

	list_add_tail(&tasks_ready[prio], &task->list);

	return 0;
}

u32 ticks_now()
{
	return arch_ticks_now();
}

void task_printall()
{
	int i;

	printf("ERROR do not use this code in production\n");
	printf("ready tasks:\n");
	for (i=0; i<TASK_PRIORITIES; i++) {
		struct list *list;

		list_for_each(&tasks_ready[i], list) {
			struct task *task = list_entry(list, struct task, list);
			int s;
			int len = task->stack_len;

			for (s=0; s<len; s++)
				if (task->stack[s] != MAGIC_STACK_POISON)
					break;
			printf("\t%s, stack:%i/%i\n", task->name, len-s, len);
		}
	}

	printf("waiting tasks:\n");
	for (i=0; i<TASK_PRIORITIES; i++) {
		struct list *list;

		list_for_each(&tasks_waiting[i], list) {
			struct task *task = list_entry(list, struct task, list);
			int s;
			int len = task->stack_len;

			for (s=0; s<len; s++)
				if (task->stack[s] != MAGIC_STACK_POISON)
					break;
			printf("\t%s, stack:%i/%i, timeout in %i ticks\n", task->name, len-s, len, task->timeout-ticks_now());
		}
	}

	struct task *task = &cba;
	int s;
	int len = task->stack_len;

	for (s=0; s<len; s++)
		if (task->stack[s] != MAGIC_STACK_POISON)
			break;
	printf("%s, stack:%i/%i\n", task->name, len-s, len);
}

/* scheduler idea:
 * For each task priority there's a list of running and waiting tasks.
 */

/* list for running tasks, and list for tasks waiting for events? */
/* // see below; event system, that's bound to tasks (timer->task, sem->task)? */
/* every task has ->waiting list, then those task either have time_wake or ->sem set? */
// XXX was here

/* OK, so some insight that i got on the bus ride home:
 * cba task + 2 lists for every priority - ready and waiting on event
 * first task to run is always at the start of running list. when a task
 * (has been ran) is scheduled!, it's put at the end of that list (->prev->prev!)
 * (something to protect task list operations will be needed).
 * if a task calls sleep/down, then we put it on other list at same prio (how!?)
 * ..*/

/* must be called under sched_lock */
static void task_make_waiting(struct task *task)
{
	list_del(&task->list);
	list_add(&tasks_waiting[task->prio], &task->list);
}

void task_wake(struct task *task)
{
	sched_ctx_t ctx;

	sched_lock(&ctx);
	list_del(&task->list);
	list_add(&tasks_ready[task->prio], &task->list);
	sched_unlock(&ctx);
}



u32 next_event; /* 0 is a special value meaning unused */

/* must be called under sched_lock */
/* some timeout happened, so wake the tasks, and set the next timeout */
static int sched_process_timeout(u32 now)
{
	int i;
	int woken = 0;

	next_event = 0;
	for (i=0; i<TASK_PRIORITIES; i++) {
		struct list *list, *list2;
		list_for_each_safe(&tasks_waiting[i], list, list2) {
			struct task *task = list_entry(list, struct task, list);
			if (time_after(now, task->timeout)) {
				list_del(list);
				list_add(&tasks_ready[i], list); /* add to start */
				/* don't care about task->timeout anymore, since it's running */
				woken++;
				continue;
			}
			if (next_event == 0 || time_before(task->timeout, next_event))
				next_event = task->timeout;
		}
	}
	return woken;
}


/* must be called under sched_lock */
/*
 * schedule next scheduler activation
 * @runnable: 1 if any task is ready to run
 * @now: current timestamp
 */
static void sched_next_wake(int runnable, u32 now)
{
	u32 timeo;

	if (runnable) {
		if (next_event) {
			if (time_before(next_event, now + ms2ticks(TIME_SLICE)))
				timeo = next_event;
			else
				timeo = now + ms2ticks(TIME_SLICE);
		} else
			timeo = now + ms2ticks(TIME_SLICE);

	} else {
		if (!next_event)
			fprintf(stderr, "%s: BUG, no events pending and no runnable tasks\n", __func__);
		timeo = next_event;
	}

	arch_sched_next_interrupt(timeo-now);
}

/* must be called under sched_lock */
static void sched_run()
{
	struct task *task = NULL;
	int i;
	int runnable = 1;
	u32 now = ticks_now();

	timekeeping(now);

//	fprintf(stderr, "%s:%i\n", __func__, ticks_now() - sched_time_started);

	/* first wake tasks */
	/* FIXME, i don't like this. It isn't very clear */
	if (next_event) {
		/* event woke us, so wake the waiting tasks */
		if (time_before(next_event, now))
			sched_process_timeout(now);
	}

	/* find a task to run */
	for (i=0; i<TASK_PRIORITIES; i++) {
		struct list *list;
		/* it's ok to use non-safe variant, since we stop traversing on hit */
		list_for_each(&tasks_ready[i], list) {
			task = list_entry(list, struct task, list);
			list_del(list);
			list_add_tail(&tasks_ready[i], list);
			goto picked_next_task;
		}
	}

	task = &cba;

	/* no task is in ready state, wait for next event */
	runnable = 0;

 picked_next_task:
//	fprintf(stderr, "%s: switching to %s, runnable: %i\n", __func__, task->name, runnable);

	sched_next_wake(runnable, now);

	/* this would mean it's the only task at this priority */
	/* this also means we could just disable preemption */
	if (current == task)
		return;

	arch_task_switch(task);
}

/*
 * sched_timeout
 * @ticks: ticks to sleep for
 * @return: 0 if we slept till the end, or the remaining number of ticks to
 * sleep if we were woken by an event
 */
u32 sched_timeout(u32 ticks)
{
	u32 now;
	sched_ctx_t ctx;

	sched_lock(&ctx);
	now = ticks_now();

	if (current->dont_reschedule) {
//fprintf(stderr, "%s: not rescheduling %s\n", __func__, current->name);
		sched_unlock(&ctx);
		return ticks;
	}

	current->timeout = now + ticks;
	/* 0 is a special value, meaning there's no next event */
	if (current->timeout == 0)
		current->timeout = 1;

	task_make_waiting(current);
//fprintf(stderr, "%s: %s will wait\n", __func__, current->name);

	/* ugh, but only if we're already waiting on timeout */
	if (next_event == 0 || time_before(current->timeout, next_event)) {
		next_event = current->timeout;
	}

	sched_run();

	sched_unlock(&ctx);

	now = ticks_now();
	/* we didn't expire -> some event happened! */
	if (time_before(now, current->timeout))
		return current->timeout-now;
	return 0;
}

/* voluntary give up cpu time to other tasks (of same priority) */
void sched_yield()
{
	sched_ctx_t ctx;

	sched_lock(&ctx);
	sched_run();
	sched_unlock(&ctx);
}

u32 msleep(u32 msecs)
{
	u32 ticks;
	/* gah, this hack is ugly */
	current->dont_reschedule = 0;
	while (msecs > 1000) {
		msecs -= 1000;
		ticks = sched_timeout(ms2ticks(1000));
		if (ticks)
			return ticks2ms(ticks) + msecs;
	}
	ticks = sched_timeout(ms2ticks(msecs));
	return ticks2ms(ticks);
}

void sched_interrupt()
{
//	fprintf(stderr, "%s: now:%i\n", __func__, ticks_now());
	sched_ctx_t ctx;
	sched_lock(&ctx);

	sched_run();

	sched_unlock(&ctx);
}

/* never use this for anything but testing */
// XXX rename to wait_until, implement real mdelay */
void mdelay(int msecs)
{
	u32 now = ticks_now();
	u32 timeo = now + ms2ticks(msecs);

	while (time_before(ticks_now(), timeo))
		;
}

void sched_init()
{
	int i;

	for (i=0; i<TASK_PRIORITIES; i++) {
		list_init(&tasks_ready[i]);
		list_init(&tasks_waiting[i]);
	}
}

static void sched_thread_cba(u32 arg)
{
	/* running on private stack now, final initializations, enable systick */
	arch_sched_start_timer();

	sched_time_started = arch_ticks_now();

	while (1) {
		arch_wait_for_interrupt();
	}
}

void __noreturn sched_start()
{
	cba = (struct task) {
		.name = "cba",
		.stack = cba_stack,
		.stack_len = ALEN(cba_stack),
	};

	task_new(&cba, sched_thread_cba, 0, 0);

	arch_task_first(&cba);

	/* will never return; also, this stack space is now free */
	for (;;);
}
