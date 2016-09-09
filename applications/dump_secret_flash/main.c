/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sched.h>
#include <lock.h>
#include <compiler.h>
#include <endianess.h>

#include <interrupt.h>
#include <mach/lpc13xx_interrupt.h>
#include <mach/lpc13xx_regs.h>
#include <mach/LPC13xx.h>


#define DEVICE_ID ((u32*)0x400483f4)

void print_hex(unsigned base, const u8 *buf, unsigned len)
{
	int p = 0;
	while (len--) {
		if (p%16 == 0) {
			if (p)
				printf("\n");
			printf("%08x: ", base);
			base += 16;
		}
		printf("%02x ", buf[p++]);
	}
	printf("\n");
}


static u32 buf[1024];

__attribute__ ((section(".data"))) void copy_secret_flash(int addr)
{
	u32 *reg = (u32*)0x4003c000;

	int i;
	u32 *src = (u32*)addr;
	asm volatile ("":::"memory");
	*reg |= 0x40;
	for (i=0; i<sizeof(buf)/sizeof(buf[0]); i++)
		buf[i] = src[i];

	*reg &= ~0x40;
}

__attribute__ ((section(".data"))) u32 copy_one_secret_flash(int addr)
{
	u32 *reg = (u32*)0x4003c000;
	u32 val;

	u32 *src = (u32*)addr;
	asm volatile ("":::"memory");
	*reg |= 0x40;
	asm volatile ("":::"memory");
	val = *src;
	asm volatile ("":::"memory");
	*reg &= ~0x40;
	asm volatile ("":::"memory");

	return val;
}

int main()
{
	int i;

	printf("%s: enter\n", __func__);
	print_hex(0x10000000, (u8*)0x10000000, 0x200);

	printf("%s: dumping bootloader for device %08x\n", __func__, *DEVICE_ID);
	print_hex(0x1fff0000, (u8*)0x1fff0000, 0x4000);

	for (i=0; i<sizeof(buf)/sizeof(buf[0]); i++)
		buf[0] = 0;
	printf("%s: copy_secret_flash\n", __func__);
	copy_secret_flash(0);
	print_hex(0x0, (u8*)buf, sizeof(buf));

	for (i=0; i<sizeof(buf)/sizeof(buf[0]); i++)
		buf[0] = 0;
	printf("%s: copy_ one by one\n", __func__);
	for (i=0; i<sizeof(buf)/sizeof(buf[0]); i++)
		buf[i] = copy_one_secret_flash(i*4);
	print_hex(0x0, (u8*)buf, sizeof(buf));

	while (1)
		;

	return 0;
}
