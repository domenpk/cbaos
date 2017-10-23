/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <compiler.h>
#include <arch/crt.h>


#if 0 /* very basic debugging code. when everything goes wrong, this will set some leds */
			"ldr	r0, =0xe002c000\n"
			"mov	r1, #0x0\n"
			"str	r1, [r0, #0]\n"

			"ldr	r0, =0xe0028000\n"
			"mov	r1, #0xf0\n"
			"str	r1, [r0, #8]\n"
			"mov	r1, #0xa0\n"
			"str	r1, [r0, #4]\n"
			"mov	r1, #0x50\n"
			"str	r1, [r0, #0xc]\n"
			"foo:	b foo\n"
#endif

void __naked reset_handler()
{
	asm volatile (
			"msr	CPSR_c, #0xc0|0x11\n" /* fiq */
			"ldr	sp, =_ram_end\n"
			"msr	CPSR_c, #0xc0|0x13\n" /* super */
			"ldr	sp, =_ram_end-0x80\n"
			"msr	CPSR_c, #0xc0|0x17\n" /* abort */
			"ldr	sp, =_ram_end-0x100\n"
			"msr	CPSR_c, #0xc0|0x1b\n" /* undef */
			"ldr	sp, =_ram_end-0x180\n"
			"msr	CPSR_c, #0xc0|0x12\n" /* irq */
			"ldr	sp, =_ram_end-0x200\n"

			"msr	CPSR_c, #0x1f\n" /* system, interrupts not masked */
			"ldr	sp, =_ram_end-0x800\n"

			"bl	init\n"
			"bl	main\n"
			"b	generic_exception_handler\n"
	);
}

void __naked generic_exception_handler()
{
	asm volatile (
			"push	{lr}\n" /* = stmfd/stmdb sp! */
			"mrs	lr, CPSR\n"
			"push	{lr}\n"
			"mrs	lr, SPSR\n"
			"push	{lr}\n"

			"stmfd	sp, {r0-r15}^\n" // push user mode registers
			"sub	sp, sp, #64\n"
			"mov	r0, sp\n"

			"bl	dump_regs\n" // XXX generic_exception_handler_c
	);
}

