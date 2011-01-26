/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <types.h>
#include <stdio.h>
#include <compiler.h>
#include <arch/cm0_regs.h>
#include <cbashell.h>

extern void _ram_start;
extern void _ram_end;

static const char * const exceptions[] = {
	"Unknown",
	"Reset",
	"NMI",
	"Hard Fault",
};

static const char * const regnames[16] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
		"r8", "r9", "r10/sl", "r11/fp", "r12/ip", "sp", "lr", "pc" };

/* my precious debugging function
 * HardFault is triggered on ie. invalid memory access.
 * note: on IMPRECISERR, this doesn't help much. it just means that some
 * invalid access occured but possibly before (after/prediction?) PC
 */
void generic_exception_handler_c(u32 *oldstack, u32 *newstack, unsigned exception)
{
	int oldstack_valid = (oldstack >= (u32*)&_ram_start && oldstack < (u32*)&_ram_end);
	int i;

	if (exception < ALEN(exceptions))
		printf("%s was called, dumping regs:\n", exceptions[exception]);
	else
		printf("Exception #%i was caled, dumping regs:\n", exception);

	printf("new_lr\t0x%08x\n", newstack[8]);
	if (oldstack_valid) {
		for (i=0; i<4; i++)
			printf("%s\t0x%08x\t%i\n", regnames[i], oldstack[i], oldstack[i]);
	} else {
		printf("sp\t%p (newstack:%p)\n", oldstack, newstack);
		printf("sp is invalid, so some registers are missing\n");
	}
	for (i=0; i<8; i++)
		printf("%s\t0x%08x\t%i\n", regnames[i+4], newstack[i], newstack[i]);

	if (oldstack_valid) {
		printf("r12/ip\t0x%08x\t%i\n", oldstack[4], oldstack[4]);
		printf("sp\t%p\n", oldstack);
		printf("lr\t0x%08x\n", oldstack[5]);
		printf("pc\t0x%08x\n", oldstack[6]);
		printf("xPSR\t0x%08x\n", oldstack[7]);
	}

	printf("HFSR\t0x%08x\n", HFSR);
	if (MMAR != 0xe000ed34)
		printf("MMAR\t0x%08x\n", MMAR);
	if (BFAR != 0xe000ed38)
		printf("BFAR\t0x%08x\n", BFAR);

	printf("\nentering cbashell\n");
	cbashell_init();
	while (1) {
		int c;
		c = getchar();
		if (c >= 0)
			cbashell_charraw(c);
	}
}
