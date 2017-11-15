/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <arch/sched.h>
#include <sched.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

u32 arch_ticks_now()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000u + tv.tv_usec;
}

void arch_task_new(struct task *task, void (*func)(u32 arg), u32 arg)
{
	getcontext(&task->context.uc);
	task->context.uc.uc_stack.ss_sp = task->stack;
	task->context.uc.uc_stack.ss_size = task->stack_len * 4;
	task->context.uc.uc_link = NULL;
	makecontext(&task->context.uc, (void(*)())func, 1, arg);
}

void arch_task_first(struct task *task)
{
	current = task;
	setcontext(&current->context.uc);
}

void arch_task_switch(struct task *newt)
{
	struct task *old = current;
	current = newt;
	swapcontext(&old->context.uc, &newt->context.uc);
}

static void arch_sched_sig_alarm(int sig)
{
	//fprintf(stderr, "%s:%i\n", __func__, sig);
	sched_interrupt();
}

void arch_sched_start_timer()
{
	struct sigaction sigact = {
		.sa_handler = arch_sched_sig_alarm,
	};
	sigaction(SIGALRM, &sigact, NULL);

	/* schedule timer right away */
	//sigqueue(getpid(), SIGALRM, 0...?);
	ualarm(1, 0);
}

void arch_sched_now()
{
	//TODO same as above, pend SIGALRM immediately
	ualarm(1, 0);
}

void arch_wait_for_interrupt()
{
	sigset_t sigs;
	sigemptyset(&sigs);
	//sigfillset(&sigs);
	//sigdelset(&sigs, SIGALRM);
	/* wait for SIGALRM */
	sigsuspend(&sigs);
}

void arch_sched_next_interrupt(int offset)
{
	int ret;
	/* on some systems 999999 is the maximum */
	if (offset < 1000000)
		ret = ualarm(offset, 0);
	else
		ret = ualarm(999999, 0);
	// XXX it seems that sometimes ualarm takes about 2.9 seconds instead of one. just FYI
	// that might be some weird bug
	if (ret < 0) printf("ualarm(%i) returned %i, %s\n", offset, ret, strerror(errno));
}
