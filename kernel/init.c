/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <init.h>
#include <types.h>
#include <board.h>
#include <stdio.h>
#include <magic.h>
#include <driver.h>
#include <device.h>
#include <fcntl.h>


/* LIBC support */
struct device *global_fds[MAX_FDS];

ssize_t _write(int fd, const void *buf, size_t count)
{
	struct device *dev;

	if (fd < 0 || fd >= MAX_FDS)
		return -1;
	dev = global_fds[fd];
	if (dev == NULL)
		return -1;
	return dev->drv->write(dev, buf, count);
}

ssize_t _read(int fd, void *buf, size_t count)
{
	struct device *dev;

	if (fd < 0 || fd >= MAX_FDS)
		return -1;
	dev = global_fds[fd];
	if (dev == NULL)
		return -1;
	return dev->drv->read(dev, buf, count);
}

static void console_init()
{
	extern struct driver null_driver;
	static struct device null;

	/* register null device */
	null.name = "null";
	null.drv = &null_driver;
	if (null.drv->probe(&null, 0) == 0)
		device_register(&null);

	struct device *dev = device_find("tty0");
	if (!dev) {
		device_find("null");
	}
	dev->drv->open(dev, O_NONBLOCK);

	global_fds[0] = dev;
	global_fds[1] = dev;
	global_fds[2] = dev;
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
