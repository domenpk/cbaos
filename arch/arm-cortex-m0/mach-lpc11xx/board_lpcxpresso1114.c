/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <board.h>
#include <mach/lpc11xx_regs.h>
#include <drivers/lpc_serial.h>
#include <device.h>


// XXX spi and i2c code - just use the lpc13xx stuff

void tty0_init()
{
	static struct device tty0;
	static struct lpc_uart_data uart_data;	

	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_IOCON;
	LPC_IOCON->PIO1_6 = 1; /* uart RXD */
	LPC_IOCON->PIO1_7 = 1; /* uart TXD */

	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_UART;
	LPC_SYSCON->UARTCLKDIV = 1; /* main clock / 1 */

	uart_data.base = (void*)LPC_UART_BASE;
	uart_data.flags = LPC_UART_FDR;
	uart_data.pclk = CONFIG_FCPU;
	uart_data.baudrate = 115200;

	tty0.name = "tty0";
	tty0.drv = &serial_driver;
	if (tty0.drv->probe(&tty0, &uart_data) == 0)
		device_register(&tty0);
}

void udelay(int x)
{
	x = x*(CONFIG_FCPU/1000000)/4;
	asm volatile(
		"mov	%0, %1\n\t"
		".udelay_%=:\n\t"
		".syntax unified\n"
		"subs	%0, %0, #1\n\t"		/* 1 cycle */
		".syntax divided\n"
		"bne	.udelay_%=\n\t"	/* 3 cycles */
		: "=r" (x) : "r" (x) : "cc" );
}

void board_init()
{
	/* power up IRC */
	LPC_SYSCON->PDRUNCFG &= ~(PDRUNCFG_IRCOUT_PD | PDRUNCFG_IRC_PD);

	/* select IRC for main clock */
	LPC_SYSCON->MAINCLKSEL = MAINCLKSEL_IRC;
	LPC_SYSCON->MAINCLKUEN = 0;
	LPC_SYSCON->MAINCLKUEN = 1;

	/* IRC clock for PLL */
	LPC_SYSCON->SYSPLLCLKSEL = SYSPLLCLKSEL_IRC;
	LPC_SYSCON->SYSPLLCLKUEN = 0;
	LPC_SYSCON->SYSPLLCLKUEN = 1;

	/* for mode 1: Fclkout = M * Fclkin = Fcco/(2*P)
	 * Fclkin (IRC) = 12MHz, Fcco e [156, 320] */
#if CONFIG_FCPU == 72000000
	LPC_SYSCON->SYSPLLCTRL = 1<<5 /* P=2 */ | 5<<0 /* M=6 */; /* set clock to 72 MHz */
#elif CONFIG_FCPU == 48000000
	LPC_SYSCON->SYSPLLCTRL = 1<<5 /* P=2 */ | 3<<0 /* M=4 */; /* set clock to 48 MHz */
#else
#error "invalid CONFIG_FCPU"
#endif

#if CONFIG_FCPU > 40000000
	FLASHCFG = (FLASHCFG & ~3) | 2; /* 3 system clocks */
#elif CONFIG_FCPU > 20000000
	FLASHCFG = (FLASHCFG & ~3) | 1; /* 2 system clocks */
#else
	FLASHCFG = (FLASHCFG & ~3) | 0; /* 1 system clocks */
#endif

	/* power up SYSPLL */
	LPC_SYSCON->PDRUNCFG &= ~PDRUNCFG_SYSPLL_PD;
	/* wait for PLL lock */
	while ((LPC_SYSCON->SYSPLLSTAT & 1) == 0)
		;

	/* select PLL output for main clock */
	LPC_SYSCON->MAINCLKSEL = MAINCLKSEL_PLL;
	LPC_SYSCON->MAINCLKUEN = 0;
	LPC_SYSCON->MAINCLKUEN = 1;
}
