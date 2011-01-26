/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <arch/crt.h>

/* lets have weak aliases to dummy handler */
void nmi_handler() __alias(dummy_handler);
void hardfault_handler() __alias(dummy_handler);
void svc_handler() __alias(dummy_handler);
void pendsv_handler() __alias(dummy_handler);
void systick_handler() __alias(dummy_handler);

void wakeup_irqhandler() __alias(dummy_handler);
void c_can_irqhandler() __alias(dummy_handler);
void ssp1_irqhandler() __alias(dummy_handler);
void i2c_irqhandler() __alias(dummy_handler);
void timer16_0_irqhandler() __alias(dummy_handler);
void timer16_1_irqhandler() __alias(dummy_handler);
void timer32_0_irqhandler() __alias(dummy_handler);
void timer32_1_irqhandler() __alias(dummy_handler);
void ssp0_irqhandler() __alias(dummy_handler);
void uart_irqhandler() __alias(dummy_handler);

void adc_irqhandler() __alias(dummy_handler);
void wdt_irqhandler() __alias(dummy_handler);
void bod_irqhandler() __alias(dummy_handler);

void pioint3_irqhandler() __alias(dummy_handler);
void pioint2_irqhandler() __alias(dummy_handler);
void pioint1_irqhandler() __alias(dummy_handler);
void pioint0_irqhandler() __alias(dummy_handler);


void __naked dummy_handler()
{
	asm volatile (	"b	generic_exception_handler\n\t");
}

__attribute__ ((section(".vectors"))) void (* const _vectors[])() =
{
	/* exceptions, arch specific */
	&_ram_end,              /* stack */
	reset_handler,
	nmi_handler,
	hardfault_handler,
	0,                      /* 4, 0x10 */
	0,
	0,
	0,                      /* 0x1c reserved, checksum for boot loader on lpc11/13 */
	0,                      /* 8, 0x20 */
	0,
	0,
	svc_handler,
	0,                      /* 12, 0x30 */
	0,
	pendsv_handler,
	systick_handler,

	/* interrupts, mach specific */
	wakeup_irqhandler,	/* P0_0 */
	wakeup_irqhandler,	/* P0_1 */
	wakeup_irqhandler,	/* P0_2 */
	wakeup_irqhandler,	/* P0_3 */
	wakeup_irqhandler,	/* P0_4 */
	wakeup_irqhandler,	/* P0_5 */
	wakeup_irqhandler,	/* P0_6 */
	wakeup_irqhandler,	/* P0_7 */
	wakeup_irqhandler,	/* P0_8 */
	wakeup_irqhandler,	/* P0_9 */
	wakeup_irqhandler,	/* P0_10 */
	wakeup_irqhandler,	/* P0_11 */
	wakeup_irqhandler,	/* P1_0 */

	c_can_irqhandler,       /* 13 */
	ssp1_irqhandler,
	i2c_irqhandler,
	timer16_0_irqhandler,
	timer16_1_irqhandler,
	timer32_0_irqhandler,
	timer32_1_irqhandler,
	ssp0_irqhandler,        /* 20 */
	uart_irqhandler,
	0,
	0,

	adc_irqhandler,
	wdt_irqhandler,
	bod_irqhandler,
	0,

	pioint3_irqhandler,
	pioint2_irqhandler,
	pioint1_irqhandler,     /* 30 */
	pioint0_irqhandler,     /* 31 */
};
