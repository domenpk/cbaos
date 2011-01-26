/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <types.h>
#include <board.h>
#include <stdio.h>
#include <magic.h>

static FILE _stdout;

static void console_init()
{
	int r;
	r = fopen(&_stdout, "/dev/tty0", O_NONBLOCK);
	if (r == 0) {
		stdin = &_stdout;
		stdout = &_stdout;
		stderr = &_stdout;
	} else {
		// TODO error, no console
	}
}

#ifndef ARCH_UNIX
extern void _fldata, _data, _edata, _bss, _ebss, _ram_end;
void init()
{
	u32 *src = &_fldata;
	u32 *dest = &_data;

	/* copy .data from flash */
	while (dest < (u32*)&_edata)
		*dest++ = *src++;

	/* zero .bss */
	dest = &_bss;
	while (dest < (u32*)&_ebss)
		*dest++ = 0;

	/* poison the rest of ram */
	while (dest < (u32*)&src-16)
		*dest++ = MAGIC_RAM_POISON;
#else

/* This is very much gcc dependent. Calls the function before main() */

void __attribute__((constructor)) init()
{
#endif

	board_init();
	tty0_init(); /* serial interface for printf (stdio) */

	console_init();
}
