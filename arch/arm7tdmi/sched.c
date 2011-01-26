/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>

#include <sched.h>
#include <interrupt.h>
#include <compiler.h>

#include <arch/sched.h>


// THIS CODE GOES TO MACH XXX this = timer code!
#include <mach/lpc21xx_regs.h>

u32 arch_ticks_now()
{
	return T0TC;
}

// gas trick to foce arm:			".arm\n"


/* arm7tdmi-s doesn't save anything on stack, instead used banked regs
 * We save regs on stack (low to high addr): {pc, xPSR} {r0-r12,lr} */
void arch_task_new(struct task *task, void (*func)(u32 arg), u32 arg)
{
	task->stack[task->stack_len-1] = 0x01010101*14 /* r14 */;
	task->stack[task->stack_len-2] = 0x01010101*12 /* r12 */;
	task->stack[task->stack_len-3] = 0x01010101*11 /* r11 */;
	task->stack[task->stack_len-4] = 0x01010101*10 /* r10 */;
	task->stack[task->stack_len-5] = 0x01010101*9 /* r9 */;
	task->stack[task->stack_len-6] = 0x01010101*8 /* r8 */;
	task->stack[task->stack_len-7] = 0x01010101*7 /* r7 */;
	task->stack[task->stack_len-8] = 0x01010101*6 /* r6 */;
	task->stack[task->stack_len-9] = 0x01010101*5 /* r5 */;
	task->stack[task->stack_len-10] = 0x01010101*4 /* r4 */;
	task->stack[task->stack_len-11] = 0x01010101*3 /* r3 */;
	task->stack[task->stack_len-12] = 0x01010101*2 /* r2 */;
	task->stack[task->stack_len-13] = 0x01010101*1 /* r1 */;
	task->stack[task->stack_len-14] = arg /* r0 */;
	task->stack[task->stack_len-15] = 0 /* xPSR, thumb bit not set */;
	task->stack[task->stack_len-16] = (u32)func;

	task->context.psp = (u32)&task->stack[task->stack_len-16];
}

void __naked arch_task_first(struct task *task)
{
	current = task;
	asm volatile (
			/* restore created context */
			"mov	sp, %0\n\t"

			"add	sp, sp, #8\n\t"    /* pc and xPSR space, just ignore it */
			"pop	{r0-r12,lr}\n\t"

			"ldr	pc, [sp, #-64]\n\t" /* and jump to the task entry; sp here is top of stack so -64 is entry -16 = func */
			: : "r" (task->context.psp)
	);
}

/* task to switch to */
static struct task *new_task;

/* arm arm A4.1.98 STM(2)
 * does the task switching from current to new_task */
void __naked swi_handler()
{
	asm volatile (
			/* banked registers: lr (thread pc), sp, xPSR */
			"push	{lr}\n"		/* lr == thread pc */

			/* get user sp */
/* A4.1.98 says W (writeback) bit must be 0, for this instruction (user mode regs) */
			"stmdb	sp, {sp}^\n"
			"ldr	lr, [sp, #-4]\n"

			/* save r0-r12,r14 */
			"stmdb	lr, {r0-r12,lr}^\n"
			"sub	lr, lr, #0x38\n"
			/* save pc and SPSR */
			"pop	{r0}\n"		/* get lr (thread pc) */
			"mrs	r1, SPSR\n"
			"stmdb	lr!, {r0,r1}\n" /* save lr (thread pc), and SPSR */


			"ldr	r0, =current\n\t"
			"ldr	r1, [r0]\n\t"

			"str	lr, [r1, %0]\n\t"  /* current->context.psp = PSP */

			"ldr	r2, =new_task\n\t"
			"ldr	r2, [r2]\n\t"

			"ldr	lr, [r2, %0]\n\t"  /* PSP = new_task->context.psp */

			"str	r2, [r0]\n\t"      /* current = new_task */

			/* restore pc/lr, SPSR */
			"ldmia	lr!, {r0,r1}\n"
			"msr	SPSR, r1\n"
			"push	{r0}\n"
			/* restore other regs */
			"ldmia	lr, {r0-r12,lr}^\n"
			"add	lr, lr, #0x38\n"

			/* restore user sp */
			"push	{lr}\n"
			"ldmia	sp, {sp}^\n"
			"add	sp, sp, #4\n"

			/* return to new_task */
			"pop	{lr}\n"
			"movs	pc, lr\n"
			: : "i" (offsetof(struct task, context))
	);
}

void arch_task_switch(struct task *newt)
{
	new_task = newt;
	/* request PendSV exception */
	asm volatile ("svc #0");
}


void __interrupt timer0_handler();
void arch_sched_start_timer()
{
	PCONP |= PCONP_TIM0;

	T0TCR = 1<<1; /* RESET */
	T0TC = 0;

	/* should have some wrappers to allocate VIC vectors */
	VICVectAddr0 = (u32)timer0_handler;
	VICVectCntl0 = 0x20 | IRQ_TIMER0;
// 213x specific. leave to default	T0CTCR = 0; /* timer mode */

	irq_enable(IRQ_TIMER0);

	/* 1us, 4clocks = 1pclk */
	T0PR = CONFIG_FCPU/1000000/4-1;
	T0MR0 = T0TC+1; /* match asap */

	T0MCR = 1<<0; /* int on MR0 */

	T0TCR = 1<<0; /* out of reset, enable timer */

	// note, could also use irq controller set pend interrupt
}

void arch_wait_for_interrupt()
{
	PCON = 1; /* idle mode, host clock stopped, interrupt will wake us up */
}

void arch_sched_next_interrupt(int offset)
{
	if (offset == 0)
		offset++;

	T0TCR = 0; /* disable timer so there's no race (T0TC update vs T0MR0 update) */
	T0MR0 = T0TC + offset;
	T0TCR = 1<<0; /* enable timer again */
}

void __interrupt timer0_handler()
{
	T0IR = 1<<0; /* ack MR0 interrupt */
	sched_interrupt();

	VICVectAddr = 0; /* clear pending interrupt, NOTE UM says this should be at the end, but I don't get why it doesn't work if it's above sched_interrupt */
}
