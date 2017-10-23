/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <board.h>
#include <drivers/semihosting_serial.h>
#include <arch/semihosting.h>
#include <device.h>


void tty0_init()
{
	static struct device tty0;
	tty0.name = "tty0";
	tty0.drv = &semihosting_serial;
	device_register(&tty0);
}

void udelay(int x)
{
	int end = semihosting(SEMIHOSTING_CLOCK, 0) + x/10000;

	while (semihosting(SEMIHOSTING_CLOCK, 0) < end)
		;
}

void board_init()
{
	/* Notes:
	 * .ld script needs to have flash at 0x0, so qemu works. Why?!
	 * This is qemu emulator, RCC periph doesn't even exist here.
	 *
	 * info mtree:
	   00000000 (prio 0, R-): alias STM32F205.flash.alias @STM32F205.flash 0000000000000000-00000000000fffff
	   08000000 (prio 0, R-): STM32F205.flash
	   20000000 (prio 0, RW): STM32F205.sram
	   22000000 (prio 0, RW): bitband
	   40000000 (prio 0, RW): stm32f2xx_timer (4 timers 0x400 apart)
	   40003800 (prio 0, RW): stm32f2xx-spi (2 spi 0x400 apart)
	   40004400 (prio 0, RW): stm32f2xx-usart (4 usart 0x400 apart)
	   40011000 (prio 0, RW): stm32f2xx-usart (2 usart 0x400 apart)
	   40012000 (prio 0, RW): stm32f2xx-adc (3 adc 0x100 apart)
	   40013000 (prio 0, RW): stm32f2xx-spi
	   40013800 (prio 0, RW): stm32f2xx-syscfg
	   42000000 (prio 0, RW): bitband
	   e000e000 (prio 0, RW): nvic
	 */
}
