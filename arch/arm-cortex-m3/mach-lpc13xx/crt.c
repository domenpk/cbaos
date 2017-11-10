/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <compiler.h>
#include <arch/crt.h>


/* lets have weak aliases to dummy handler */
void nmi_handler() __alias(dummy_handler);
void hardfault_handler() __alias(dummy_handler);
void memmanage_handler() __alias(dummy_handler);
void busfault_handler() __alias(dummy_handler);
void usagefault_handler() __alias(dummy_handler);
void svc_handler() __alias(dummy_handler);
void debugmon_handler() __alias(dummy_handler);
void pendsv_handler() __alias(dummy_handler);
void systick_handler() __alias(dummy_handler);

void wakeup_irqhandler() __alias(dummy_handler);
void i2c_irqhandler() __alias(dummy_handler);
void timer16_0_irqhandler() __alias(dummy_handler);
void timer16_1_irqhandler() __alias(dummy_handler);
void timer32_0_irqhandler() __alias(dummy_handler);
void timer32_1_irqhandler() __alias(dummy_handler);
void ssp_irqhandler() __alias(dummy_handler);
void uart_irqhandler() __alias(dummy_handler);

void usb_irqhandler() __alias(dummy_handler);
void usb_fiqhandler() __alias(dummy_handler);

void adc_irqhandler() __alias(dummy_handler);
void wdt_irqhandler() __alias(dummy_handler);
void bod_irqhandler() __alias(dummy_handler);
void fmc_irqhandler() __alias(dummy_handler);

void pioint3_irqhandler() __alias(dummy_handler);
void pioint2_irqhandler() __alias(dummy_handler);
void pioint1_irqhandler() __alias(dummy_handler);
void pioint0_irqhandler() __alias(dummy_handler);


void __naked_asm dummy_handler()
{
	asm volatile (	"b	generic_exception_handler\n\t");
}

__attribute__ ((section(".vectors"))) void (* const _vectors[])() =
{
	/* exceptions, arch specific */
	(void*)&_ram_end,              /* stack */
	reset_handler,
	nmi_handler,
	hardfault_handler,
	memmanage_handler,      /* 4, 0x10 */
	busfault_handler,
	usagefault_handler,
	0,                      /* 0x1c reserved, checksum for boot loader on lpc11/13 */
	0,                      /* 8, 0x20 */
	0,
	0,
	svc_handler,
	debugmon_handler,       /* 12, 0x30 */
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
	wakeup_irqhandler,	/* P1_1 */
	wakeup_irqhandler,	/* P1_2 */
	wakeup_irqhandler,	/* P1_3 */
	wakeup_irqhandler,	/* P1_4 */
	wakeup_irqhandler,	/* P1_5 */
	wakeup_irqhandler,	/* P1_6 */
	wakeup_irqhandler,	/* P1_7 */
	wakeup_irqhandler,	/* P1_8 */
	wakeup_irqhandler,	/* P1_9 */
	wakeup_irqhandler,	/* P1_10 */
	wakeup_irqhandler,	/* P1_11 */

	wakeup_irqhandler,	/* P2_0 */
	wakeup_irqhandler,	/* P2_1 */
	wakeup_irqhandler,	/* P2_2 */
	wakeup_irqhandler,	/* P2_3 */
	wakeup_irqhandler,	/* P2_4 */
	wakeup_irqhandler,	/* P2_5 */
	wakeup_irqhandler,	/* P2_6 */
	wakeup_irqhandler,	/* P2_7 */
	wakeup_irqhandler,	/* P2_8 */
	wakeup_irqhandler,	/* P2_9 */
	wakeup_irqhandler,	/* P2_10 */
	wakeup_irqhandler,	/* P2_11 */

	wakeup_irqhandler,	/* P3_0 */
	wakeup_irqhandler,	/* P3_1 */
	wakeup_irqhandler,	/* P3_2 */
	wakeup_irqhandler,	/* P3_3 */

	i2c_irqhandler,         /* 40, 0xa0 (0xe0 absolute)*/
	timer16_0_irqhandler,
	timer16_1_irqhandler,
	timer32_0_irqhandler,
	timer32_1_irqhandler,
	ssp_irqhandler,
	uart_irqhandler,

	usb_irqhandler,
	usb_fiqhandler,

	adc_irqhandler,
	wdt_irqhandler,         /* 50, 0xc8 (0x108 absolute) */
	bod_irqhandler,
	fmc_irqhandler,

	pioint3_irqhandler,
	pioint2_irqhandler,
	pioint1_irqhandler,
	pioint0_irqhandler,     /* 56, 0xe0 (0x120 absolute) */
};
