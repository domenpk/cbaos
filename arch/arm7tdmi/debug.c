/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <types.h>
#include <stdio.h>
#include <compiler.h>
#include <cbashell.h>

struct regs {
	u32 reg[16];
	u32 spsr;
	u32 cpsr;
	u32 ex_lr;
};

const char * const exceptions[] = {
	"Unknown",
	"Reset",
	"Undefined",
	"SWI",
	"PrefetchAbort",
	"DataAbort",
	"IRQ",
	"FIQ",
};

static const char * const regnames[16] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
		"r8", "r9", "r10/sl", "r11/fp", "r12/ip", "sp", "lr", "pc" };

static char str[9] = "nzcv ift";
void dump_regs(struct regs *regs)
{
	int i;
	u32 cpsr = regs->cpsr;

	printf("exception in mode %x was called, dumping regs:\n", cpsr & 0x1f);

	for (i=0; i<15; i++)
		printf("%s\t0x%08x\t%i\n", regnames[i], regs->reg[i], regs->reg[i]);
	//printf("pc\t0x%08x\t%i\n", regs->reg[15], regs->reg[15]); /* current pc isn't interesting */

	u32 tmp1, tmp2;
	asm volatile (
			"mrs	r3, CPSR\n"
			"msr	CPSR_c, #0xc0|0x12\n" /* irq */
			"mov	%0, sp\n"
			"mov	%1, lr\n"
			"msr	CPSR_c, r3\n"
			: "=r"(tmp1), "=r"(tmp2) : : "r3" );
	printf("sp_irq\t0x%08x\n", tmp1);
	printf("lr_irq\t0x%08x\n", tmp2);

	asm volatile (
			"mrs	r3, CPSR\n"
			"msr	CPSR_c, #0xc0|0x13\n" /* svc */
			"mov	%0, sp\n"
			"mov	%1, lr\n"
			"msr	CPSR_c, r3\n"
			: "=r"(tmp1), "=r"(tmp2) : : "r3" );
	printf("sp_svc\t0x%08x\n", tmp1);
	printf("lr_svc\t0x%08x\n", tmp2);

	printf("pc\t0x%08x\n", regs->ex_lr);

	for (i=0; i<sizeof(str); i++)
		str[i] |= 0x20;
	for (i=0; i<4; i++) {
		if (cpsr & (1<<(31-i)))
			str[i] -= 0x20;
		if (cpsr & (1<<(8-i)))
			str[i+4] -= 0x20;
	}
	str[4] = ' ';
	printf("cpsr\t0x%08x\t%s, mode:%x\n", cpsr, str, cpsr & 0x1f);

	cpsr &= 0x1f;
	if (cpsr != 0x10 && cpsr != 0x1f) {
		cpsr = regs->spsr;
		for (i=0; i<sizeof(str); i++)
			str[i] |= 0x20;
		for (i=0; i<4; i++) {
			if (cpsr & (1<<(31-i)))
				str[i] -= 0x20;
			if (cpsr & (1<<(8-i)))
				str[i+4] -= 0x20;
		}
		str[4] = ' ';
		printf("spsr\t0x%08x\t%s, mode:%x\n", cpsr, str, cpsr & 0x1f);
	}

#if 0
	printf("\nentering cbashell\n");
	cbashell_init();
	while (1) {
		int c;
		c = getchar();
		if (c >= 0)
			cbashell_charraw(c);
	}
#endif
}
