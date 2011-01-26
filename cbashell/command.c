/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "command.h"
#include "types.h"
#ifdef ARCH_ARM_CORTEX_M3
#include <arch/cm3_regs.h>
#endif


static int command_x(const char *_cmd)
{
	static int len = 1;
	static int size = 4;
	static u32 addr;
	char *cmd = (char*)_cmd; /* we don't change it, but strtol is idiotic */

	if (cmd[0] != 'x' || isalpha(cmd[1]))
		return 0;

	cmd++;
	if (*cmd == '/') {
		cmd++;
		if (isdigit(*cmd)) {
			len = strtoul(cmd, &cmd, 0);
			/* just some sanity */
			if (len > 1024)
				len = 1024;
		}
		// while, check for xs...
		if (isalpha(*cmd)) {
			if (*cmd == 'b')
				size = 1;
			else if (*cmd == 'h')
				size = 2;
			else if (*cmd == 'w')
				size = 4;
			else {
				printf("E: invalid argument for size\n");
				return -1;
			}
			cmd++;
		}
	}
	if (*cmd == ' ') {
		cmd++;
		if (isdigit(*cmd))
			addr = strtoul(cmd, NULL, 0);
	}

	/* currently it only works for hex, with size 4 */
	int i;
	printf("%#010x: ", addr);
	for (i=0; i<len; i++) {
		if (i % 8 == 0 && i) {
			printf("\n%#010x: ", addr);
		}
		printf("%#010x ", *(u32*)addr);
		addr += size;
	}
	printf("\n");

	return 1;
}

static int command_boot(const char *cmd)
{
	if (strcmp(cmd, "boot") != 0)
		return 0;

#ifdef ARCH_ARM_CORTEX_M3
	AIRCR = (AIRCR & 0x8700) | 0x05FA<<16 /* magic */ | 1<<2 /* SYSRESETREQ */;
#endif

	while (1)
		;
}

static int command_fail(const char *cmd)
{
	volatile u32 *tmp = (volatile u32*)0x12345678;
	void (*f)() = (void (*)())0x23456789;

	if (strncmp(cmd, "fail ", 5) != 0)
		return 0;

	switch (cmd[5]) {
	case '0':
		/* invalid pointer deference */
		*tmp = 0xcba;
		break;
	case '1':
		/* invalid pointer deference */
		f();
		break;
	case 's':
		/* invalid stack pointer */
#ifdef ARCH_ARM_CORTEX_M3
		asm volatile ("ldr	sp, =#0x87654321\n\t");
#endif
		break;
	case 'u':
		/* undefined instruction */
#ifdef ARCH_ARM_CORTEX_M3
		*(u32*)0xe000ed24 |= 1<<18; /* enable usage fault */
		/* http://www.keil.com/support/man/docs/armasm/armasm_cihjijca.htm */
		asm volatile (".short	0xdead\n\t"); /* arm arm says, 0xdexx for thumb */
		/* some other document said 1101 011... 1 xxxx for arm mode */
#endif
		break;
	case '2':
		/* svc */
#ifdef ARCH_ARM_CORTEX_M3
		asm volatile ("svc #0\n");
#endif
		break;
	default:
		if (cmd[5])
			printf("%s: unknown argument %c\n", __func__, cmd[5]);
		printf("fail arguments: \n"
				"\t0 - write to invalid pointer\n"
				"\t1 - call invalid function\n"
				"\ts - corrupt stack pointer\n"
				"\tu - undefined instruction\n"
				);
	}
	return 1;
}

static int command_test(const char *cmd)
{
	if (strncmp(cmd, "test ", 5) != 0)
		return 0;
	char foo[16];
	strncpy(foo, cmd+5, 16);
	foo[15] = 0;
	//eprom_test(foo);

	printf("%s: %02x %02x %02x %02x\n", __func__, foo[0], foo[1], foo[2], foo[3]);
	return 1;
}

static int command_help(const char *cmd)
{
	if (strcmp(cmd, "help") != 0)
		return 0;
	printf("something helpful should be written here\n");
	return 1;
}
static int command_invalid(const char *cmd)
{
	if (*cmd)
		printf("invalid command\n");
	return 1;
}

/* 1 for handled, 0 for not handled, negative for error (should abort handling) */
int (* const command_list[])(const char *cmd) = {
	command_x,
	command_boot,
	command_fail,
	command_test,
	command_help,
	command_invalid,
	NULL
};

