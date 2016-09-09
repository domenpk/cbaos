/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <types.h>
#include <board.h>
#include <stdio.h>
#include <magic.h>
#include <driver.h>
#include <device.h>

static FILE _stdout;

static void console_init()
{
	int r;
	extern struct driver null_driver;
	static struct device null;

	/* register null device */
	null.name = "null";
	null.drv = &null_driver;
	if (null.drv->probe(&null, 0) == 0)
		device_register(&null);

	r = fopen(&_stdout, "/dev/tty0", O_NONBLOCK);
	if (r != 0) {
		/* no console */
		fopen(&_stdout, "/dev/null", O_NONBLOCK);
	}
	stdin = &_stdout;
	stdout = &_stdout;
	stderr = &_stdout;
}

#ifndef ARCH_UNIX
extern unsigned long _fldata, _data, _edata, _bss, _ebss, _ram_end;
void init()
{
	unsigned long *src = &_fldata;
	unsigned long *dest = &_data;

	/* copy .data from flash */
	while (dest < &_edata)
		*dest++ = *src++;

	/* zero .bss */
	dest = &_bss;
	while (dest < &_ebss)
		*dest++ = 0;

	/* poison the rest of ram, yes &src is correct */
	while (dest < (unsigned long*)&src-16)
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
