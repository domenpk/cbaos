#include <interrupt.h>
#include <mach/lpc13xx_regs.h>
#include <mach/lpc13xx_interrupt.h>
#include <mach/lpc13xx_wdt.h>
#include <drivers/watchdog.h>


/* timeout is in seconds. and is +-40%
 * Note that the maximum here is about 4e6 (1000 hours)
 */
void watchdog_setup(enum watchdog_type type, u32 timeout)
{
	LPC_SYSCON->PDRUNCFG &= ~(1<<6); /* power up wdtosc */
	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_WDT;
	LPC_SYSCON->WDTOSCCTRL = 0xe0; /* 1MHz wdtosc clock */

	LPC_SYSCON->WDTCLKSEL = 2; /* watchdog oscillator */
	LPC_SYSCON->WDTCLKDIV = 250; /* divide clock by 250 */
	LPC_SYSCON->WDTCLKUEN = 0;
	LPC_SYSCON->WDTCLKUEN = 1; /* update cloce source */
	//printf("%s: SYSRESSTAT: %lx, WDT: %x\n", __func__, LPC_SYSCON->SYSRESSTAT, lpc13xx_wdt->WDMOD);
	/* SYSRESSTAT bits: POR EXTRST WDT BOD  SYSRST */
	//if (lpc13xx_wdt->WDMOD == 4)

	lpc13xx_wdt->WDTC = 1000000/4/250 * timeout; /* in seconds +-40% */
	if (type == WATCHDOG_INTERRUPT) {
		irq_ack(IRQ_WDT);
		irq_enable(IRQ_WDT);
		lpc13xx_wdt->WDMOD = 0x1; /* enable wdt interrupt */
		lpc13xx_wdt->WDFEED = 0xaa;
		lpc13xx_wdt->WDFEED = 0x55;
	}
	else if (type == WATCHDOG_RESET) {
		lpc13xx_wdt->WDMOD = 0x2; /* reset (just 0x3 sometimes resets the mcu without wd flag set) */
		lpc13xx_wdt->WDFEED = 0xaa;
		lpc13xx_wdt->WDFEED = 0x55;
		lpc13xx_wdt->WDMOD = 0x3; /* enable+reset */
		lpc13xx_wdt->WDFEED = 0xaa;
		lpc13xx_wdt->WDFEED = 0x55;
	}
}

void watchdog_ping(void)
{
	lpc13xx_wdt->WDFEED = 0xaa;
	lpc13xx_wdt->WDFEED = 0x55;
}
