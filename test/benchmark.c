/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>

#include <sched.h>
#include <lock.h>
#include <compiler.h>


static int bench0_count;
static int bench1_count;

static void bench0(u32 arg)
{
	while (1) {
		bench0_count++;
		sched_yield();
	}
}

static void bench1(u32 arg)
{
	while (1) {
		bench1_count++;
		sched_yield();
	}
}

/* hmm... context switch here takes >5us, on chibios it's 1.2us, improve when bored */
static void bench_print(u32 arg)
{
	while (1) {
		struct lock l;
		lock(&l);
		printf("context switches 0:%i, 1:%i\n", bench0_count, bench1_count);
		unlock(&l);
		msleep(1000);
	}
}


#ifdef ARCH_UNIX
#define STACK 2048
#else
#define STACK 128
#endif
static u32 s1[STACK];
static u32 s2[STACK];
static u32 s3[STACK];
static struct task t1, t2, t3;

void benchmark()
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

	task_new(&t1, bench0, 0, 0);
	task_new(&t2, bench1, 0, 0);
	task_new(&t3, bench_print, 0, 0);

	sched_start();
}
