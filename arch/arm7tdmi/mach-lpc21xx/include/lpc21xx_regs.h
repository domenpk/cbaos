#ifndef _LPC21XX_REGS_
#define _LPC21XX_REGS_

/* include what comes from KEIL */
#include <mach/LPC21xx.H>
/* This is a bit problematic: there are a bunch of include files that are
 * quite similar. Maybe I should merge them. TODO
 */

/* PCONP bits for lpc213x */
#define PCONP_TIM0      (1<<1)
#define PCONP_TIM1      (1<<2)
#define PCONP_UART0     (1<<3)
#define PCONP_UART1     (1<<4)
#define PCONP_PWM0      (1<<5)

#define PCONP_I2C0      (1<<7)
#define PCONP_SPI0      (1<<8)
#define PCONP_RTC       (1<<9)
#define PCONP_SPI1      (1<<10)

#define PCONP_AD0       (1<<12)

#define PCONP_I2C1      (1<<19)
#define PCONP_AD1       (1<<20)

enum LPC21XX_IRQ {
	IRQ_WDT = 0,
	IRQ_ARMCore0 = 2,
	IRQ_ARMCore1 = 3,
	IRQ_TIMER0 = 4,
	IRQ_TIMER1 = 5,
	IRQ_UART0 = 6,
	IRQ_UART1 = 7,
	IRQ_PWM0 = 8,
	IRQ_I2C0 = 9,
	IRQ_SPI0 = 10,
	IRQ_SPI1 = 11,
	IRQ_PLL = 12,
	IRQ_RTC = 13,
	IRQ_EINT0 = 14,
	IRQ_EINT1 = 15,
	IRQ_EINT2 = 16,
	IRQ_EINT3 = 17,
	IRQ_AD0 = 18,
	IRQ_I2C1 = 19,
	IRQ_BOD = 20,
	IRQ_AD1 = 21,
};

#endif
