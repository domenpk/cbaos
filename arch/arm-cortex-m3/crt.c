/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <compiler.h>
#include <arch/crt.h>

void __naked_asm reset_handler()
{
	/* cortex-m3 trm 2.2.1 main stack and process stack */
	asm volatile (
			//"ldr	sp, =_ram_end\n\t" /* isn't actually needed, cpu reads addr 0 */
			/* select PSP as current stack pointer */
			"mrs	r0, CONTROL\n\t"
			"orr	r0, r0, #2\n\t"
			"msr	CONTROL, r0\n\t"
			"isb\n\t"

			/* lr at this point is 0x1fff12c7, in case you want to do whatever :) */

			/* hrm... this stack is ignored laters anyways, as the scheduler starts,
			 * so it could just be the same as for exceptions, yes? */
			"ldr	sp, =_ram_end-256\n\t" /* 256 for main stack */
			//"ldr	sp, =_ram_end-1024\n\t" /* 1k for main stack */

			"bl	init\n\t"
			"bl	main\n\t"
			"b	generic_exception_handler\n\t"
	);
}

/* cortex-m3 trm, 5.5.1, mcu stacks xPSR, PC, LR, R12, R3, R2, R1, R0 */
void __naked_asm generic_exception_handler()
{
	asm volatile (
			"tst	lr, #0x4\n\t" /* process task, see EXC_RETURN */
			"ite	eq\n\t"
			"mrseq	r0, MSP\n\t"
			"mrsne	r0, PSP\n\t"
			"push	{r4-r11, lr}\n\t" /* = stmfd/stmdb sp!, ... */
			"mov	r1, sp\n\t"
			"mrs	r2, IPSR\n\t"
			"b	generic_exception_handler_c\n\t"
	);
}

