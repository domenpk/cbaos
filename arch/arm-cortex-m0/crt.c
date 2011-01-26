/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include "arch/crt.h"

extern void _ram_end;

void __naked reset_handler()
{
	/* cortex-m3 trm 2.2.1 main stack and process stack */
	asm volatile (
			/* select PSP as current stack pointer */
			"mrs	r0, CONTROL\n"
			"mov	r1, #2\n"
			"orr	r0, r1\n"
			"msr	CONTROL, r0\n"
			"isb\n"

			/* hrm... this stack is ignored laters anyways, as the scheduler starts,
			 * so it could just be the same as for exceptions, yes? */
			"ldr	r0, =_ram_end-1024\n" /* 1k for main stack */
			"mov	sp, r0\n"

			"bl	init\n"
			"bl	main\n"
			"b	generic_exception_handler\n"
	);
}

/* cortex-m3 trm, 5.5.1, mcu stacks xPSR, PC, LR, R12, R3, R2, R1, R0 */
void __naked generic_exception_handler()
{
	asm volatile (
			"mov	r2, lr\n"
			"mov	r1, #0x4\n"
			"tst	r2, r1\n" /* process task, see EXC_RETURN */

			"mrs	r0, MSP\n"
			"beq	1f\n"
			"mrs	r0, PSP\n"
			"1:\n"

			/* yeah, on cortex-m3 this is push {r4-r11, lr} :P */
			"push	{lr}\n"
			"mov	lr, r0\n" /* tmp for sp */
			"mov	r0, r8\n"
			"mov	r1, r9\n"
			"mov	r2, r10\n"
			"mov	r3, r11\n"
			"push	{r0-r3}\n" /* r8-r11 */
			"push	{r4-r7}\n"
			"mov	r0, lr\n" /* restore sp to r0 */

			"mov	r1, sp\n\t"
			"mrs	r2, IPSR\n\t"
			"b	generic_exception_handler_c\n\t"
	);
}

