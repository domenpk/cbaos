/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>

#include <sem.h>
#include <sched.h>
#include <compiler.h>

static struct sem test;

extern u32 sched_time_started;

#ifdef ARCH_UNIX
#define STACK 2048
#else
#define STACK 128
#endif

static u32 s1[STACK];
static u32 s2[STACK];
static u32 s3[STACK];

static struct task t1, t2, t3;


/* XXX lock is abused here for irq disable */
static void thread(u32 arg)
{
	struct lock l;
	lock(&l);
	fprintf(stderr, "%s %s started with %i\n", __func__, current->name, arg);
	unlock(&l);
	while (1) {
		u32 foo = ticks2ms(ticks_now() - sched_time_started);
		lock(&l);
		fprintf(stderr, "%s:%i.%03i\n", current->name, foo/1000, foo%1000);
		unlock(&l);

		int r = down(&test, ms2ticks(10000));
		lock(&l);
		if (r == 0)
			fprintf(stderr, "%s:got sem\n", current->name);
		else
			fprintf(stderr, "%s:timeouted\n", current->name);
		if (arg == 2)
			task_printall();
		unlock(&l);
	}
}

static void sem_up(u32 arg)
{
	struct lock l;
	lock(&l);
	fprintf(stderr, "%s %s started with %i\n", __func__, current->name, arg);
	unlock(&l);
	while (1) {
		u32 foo = ticks2ms(ticks_now() - sched_time_started);
		lock(&l);
		fprintf(stderr, "%s:%i.%03i\n", current->name, foo/1000, foo%1000);
		unlock(&l);
		up(&test);
		msleep(1000*arg);
	}
}


void semtest(void)
{
	t1 = (struct task) {
		.name = "t1",
		.stack = s1,
		.stack_len = ALEN(s1),
	};

	t2 = (struct task) {
		.name = "t2",
		.stack = s2,
		.stack_len = ALEN(s2),
	};

	t3 = (struct task) {
		.name = "t3",
		.stack = s3,
		.stack_len = ALEN(s3),
	};

	sched_init();
	sem_init(&test, 0);

	task_new(&t1, thread, 1, 1);
	task_new(&t2, thread, 2, 1);
	task_new(&t3, sem_up, 3, 0);

	sched_start();
}
