/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <compiler.h>

// XXX needs to be compiled with -marm

void undefined_handler() __alias(dummy_handler);
void swi_handler() __alias(dummy_handler);
void dabort_handler() __alias(dummy_handler);
void pabort_handler() __alias(dummy_handler);
void fiq_handler() __alias(dummy_handler);


void __naked dummy_handler()
{
	asm volatile (	"b	generic_exception_handler\n\t");
}

__attribute__ ((section(".vectors"))) void __naked _vectors()
{
	asm volatile (
			"ldr	pc, =reset_handler\n"
			"ldr	pc, =undefined_handler\n"
			"ldr	pc, =swi_handler\n"
			"ldr	pc, =pabort_handler\n"
			"ldr	pc, =dabort_handler\n"
			".word	0\n" /* checksum */
			"ldr	pc, [pc, #-0xff0]\n" /* VIC, 0xfffff030 */
			"ldr	pc, =fiq_handler\n"
	);
}
