/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <driver.h>
#include <device.h>
#include <unistd.h>
#include <board.h>


void tty0_init()
{
	extern struct driver uart_driver;
	static struct device tty0;

	tty0.name = "tty0";
	tty0.drv = &uart_driver;
	if (tty0.drv->probe(&tty0, 0) == 0)
		device_register(&tty0);
}

void udelay(int x)
{
	usleep(x);
}

void board_init()
{
}
