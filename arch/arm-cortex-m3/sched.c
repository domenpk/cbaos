/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>

#include <sched.h>
#include <compiler.h>

#include <arch/crt.h>
#include <arch/cm3_regs.h>


/* cortex-m3 pushes following on stack when entering exception: xPSR, PC, LR, r12, r3, r2, r1, r0
 * what's remaining/needs to be saved is: PSP, r4-r11
 */
void arch_task_new(struct task *task, void (*func)(u32 arg), u32 arg)
{
	/* saved by exception */
	task->stack[task->stack_len-1] = 1<<24 /* xPSR, thumb bit */;
	task->stack[task->stack_len-2] = (u32)func;
	task->stack[task->stack_len-3] = 0x01010101*14 /* LR - this could be something nice, so tasks can actually exit */;
	task->stack[task->stack_len-4] = 0x01010101*12 /* r12 */;
	task->stack[task->stack_len-5] = 0x01010101*3  /* r3 */;
	task->stack[task->stack_len-6] = 0x01010101*2  /* r2 */;
	task->stack[task->stack_len-7] = 0x01010101*1  /* r1 */;
	task->stack[task->stack_len-8] = arg /* r0 */;

	/* saved manually */
	task->stack[task->stack_len-9] = 0x01010101*11 /* r11 */;
	task->stack[task->stack_len-10] = 0x01010101*10 /* r10 */;
	task->stack[task->stack_len-11] = 0x01010101*9 /* r9 */;
	task->stack[task->stack_len-12] = 0x01010101*8 /* r8 */;
	task->stack[task->stack_len-13] = 0x01010101*7 /* r7 */;
	task->stack[task->stack_len-14] = 0x01010101*6 /* r6 */;
	task->stack[task->stack_len-15] = 0x01010101*5 /* r5 */;
	task->stack[task->stack_len-16] = 0x01010101*4 /* r4 */;

	task->context.psp = (u32)&task->stack[task->stack_len-16];
}

void __naked_asm arch_task_first(struct task *task)
{
	/* one must not mix asm and C in naked functions */
	asm volatile (
			"ldr	r1, =current\n\t"
			"str	%[task], [r1]\n\t"

			/* restore created context */
			"mov	sp, %[psp]\n\t"
			"pop	{r4-r11}\n\t"
			"pop	{r0-r3,r12,lr}\n\t"

			"add	sp, sp, #8\n\t"    /* pc and xPSR space, just ignore it */
			
			"ldr	pc, [sp, #-8]\n\t" /* and jump to the task entry, sp here is top of stack, so -8 is entry -2 = func */
			: : [task] "r" (task), [psp] "r" (task->context.psp)
	);
}

/* task to switch to (not static because of inline assembly below */
struct task *new_task;

/* cortex-m3 trm 5.11 Setting up multiple stacks
 * does the task switching from current to new_task */
void __naked_asm pendsv_handler()
{
	/* must not use auto allocated registers */
	asm volatile (
			"ldr	r0, =current\n\t"
			"ldr	r1, [r0]\n\t"

			"mrs	r3, PSP\n\t"
			"stmdb	r3!, {r4-r11}\n\t" /* save regs to process stack */
			"str	r3, [r1, %[context_off]]\n\t"  /* current->context.psp = PSP */

			"ldr	r2, =new_task\n\t"
			"ldr	r2, [r2]\n\t"

			"ldr	r3, [r2, %[context_off]]\n\t"  /* PSP = new_task->context.psp */
			"ldmia	r3!, {r4-r11}\n\t" /* restore regs from process stack */
			"msr	PSP, r3\n\t"


			"str	r2, [r0]\n\t"      /* current = new_task */

			"bx	lr"
			: : [context_off] "i" (offsetof(struct task, context))
	);
}

void arch_task_switch(struct task *newt)
{
	new_task = newt;
	/* request PendSV exception */
	ICSR = ICSR_PENDSVSET;
}

void arch_wait_for_interrupt()
{
	asm volatile ("wfi");
}
