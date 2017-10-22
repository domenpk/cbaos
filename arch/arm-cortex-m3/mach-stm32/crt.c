/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
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

void wwdg_irqhandler() __alias(dummy_handler);
void pvd_irqhandler() __alias(dummy_handler);
void tamper_irqhandler() __alias(dummy_handler);
void rtc_irqhandler() __alias(dummy_handler);
void flash_irqhandler() __alias(dummy_handler);
void rcc_irqhandler() __alias(dummy_handler);
void exti0_irqhandler() __alias(dummy_handler);
void exti1_irqhandler() __alias(dummy_handler);
void exti2_irqhandler() __alias(dummy_handler);
void exti3_irqhandler() __alias(dummy_handler);
void exti4_irqhandler() __alias(dummy_handler);
void dma1_ch1_irqhandler() __alias(dummy_handler);
void dma1_ch2_irqhandler() __alias(dummy_handler);
void dma1_ch3_irqhandler() __alias(dummy_handler);
void dma1_ch4_irqhandler() __alias(dummy_handler);
void dma1_ch5_irqhandler() __alias(dummy_handler);
void dma1_ch6_irqhandler() __alias(dummy_handler);
void dma1_ch7_irqhandler() __alias(dummy_handler);
void adc1_2_irqhandler() __alias(dummy_handler);
void usb_hp_can1_tx_irqhandler() __alias(dummy_handler);
void usb_lp_can1_rx0_irqhandler() __alias(dummy_handler);
void can1_rx1_irqhandler() __alias(dummy_handler);
void can1_sce_irqhandler() __alias(dummy_handler);
void exti9_5_irqhandler() __alias(dummy_handler);
void tim1_brk_irqhandler() __alias(dummy_handler);
void tim1_up_irqhandler() __alias(dummy_handler);
void tim1_trg_com_irqhandler() __alias(dummy_handler);
void tim1_cc_irqhandler() __alias(dummy_handler);
void tim2_irqhandler() __alias(dummy_handler);
void tim3_irqhandler() __alias(dummy_handler);
void tim4_irqhandler() __alias(dummy_handler);
void i2c1_ev_irqhandler() __alias(dummy_handler);
void i2c1_er_irqhandler() __alias(dummy_handler);
void i2c2_ev_irqhandler() __alias(dummy_handler);
void i2c2_er_irqhandler() __alias(dummy_handler);
void spi1_irqhandler() __alias(dummy_handler);
void spi2_irqhandler() __alias(dummy_handler);
void usart1_irqhandler() __alias(dummy_handler);
void usart2_irqhandler() __alias(dummy_handler);
void usart3_irqhandler() __alias(dummy_handler);
void exti15_10_irqhandler() __alias(dummy_handler);
void rtc_alarm_irqhandler() __alias(dummy_handler);
void usb_wkup_irqhandler() __alias(dummy_handler);
void tim8_brk_irqhandler() __alias(dummy_handler);
void tim8_up_irqhandler() __alias(dummy_handler);
void tim8_trg_com_irqhandler() __alias(dummy_handler);
void tim8_cc_irqhandler() __alias(dummy_handler);
void adc3_irqhandler() __alias(dummy_handler);
void fsmc_irqhandler() __alias(dummy_handler);
void sdio_irqhandler() __alias(dummy_handler);
void tim5_irqhandler() __alias(dummy_handler);
void spi3_irqhandler() __alias(dummy_handler);
void uart4_irqhandler() __alias(dummy_handler);
void uart5_irqhandler() __alias(dummy_handler);
void tim6_irqhandler() __alias(dummy_handler);
void tim7_irqhandler() __alias(dummy_handler);
void dma2_ch1_irqhandler() __alias(dummy_handler);
void dma2_ch2_irqhandler() __alias(dummy_handler);
void dma2_ch3_irqhandler() __alias(dummy_handler);
void dma2_ch4_5_irqhandler() __alias(dummy_handler);


void __naked dummy_handler()
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
	0,
	0,                      /* 8, 0x20 */
	0,
	0,
	svc_handler,
	debugmon_handler,       /* 12, 0x30 */
	0,
	pendsv_handler,
	systick_handler,

	/* interrupts, mach specific */
	wwdg_irqhandler,	/* 16, 0x40 */
	pvd_irqhandler,
	tamper_irqhandler,
	rtc_irqhandler,
	flash_irqhandler,
	rcc_irqhandler,
	exti0_irqhandler,
	exti1_irqhandler,
	exti2_irqhandler,	/* 24, 0x60 */
	exti3_irqhandler,
	exti4_irqhandler,
	dma1_ch1_irqhandler,
	dma1_ch2_irqhandler,
	dma1_ch3_irqhandler,
	dma1_ch4_irqhandler,
	dma1_ch5_irqhandler,
	dma1_ch6_irqhandler,	/* 32, 0x80 */
	dma1_ch7_irqhandler,
	adc1_2_irqhandler,
	usb_hp_can1_tx_irqhandler,
	usb_lp_can1_rx0_irqhandler,
	can1_rx1_irqhandler,
	can1_sce_irqhandler,
	exti9_5_irqhandler,
	tim1_brk_irqhandler,	/* 40, 0xa0 */
	tim1_up_irqhandler,
	tim1_trg_com_irqhandler,
	tim1_cc_irqhandler,
	tim2_irqhandler,
	tim3_irqhandler,
	tim4_irqhandler,
	i2c1_ev_irqhandler,
	i2c1_er_irqhandler,	/* 48, 0xc0 */
	i2c2_ev_irqhandler,
	i2c2_er_irqhandler,
	spi1_irqhandler,
	spi2_irqhandler,
	usart1_irqhandler,
	usart2_irqhandler,
	usart3_irqhandler,
	exti15_10_irqhandler,	/* 56, 0xe0 */
	rtc_alarm_irqhandler,
	usb_wkup_irqhandler,
	tim8_brk_irqhandler,
	tim8_up_irqhandler,
	tim8_trg_com_irqhandler,
	tim8_cc_irqhandler,
	adc3_irqhandler,
	fsmc_irqhandler,	/* 64, 0x100 */
	sdio_irqhandler,
	tim5_irqhandler,
	spi3_irqhandler,
	uart4_irqhandler,
	uart5_irqhandler,
	tim6_irqhandler,
	tim7_irqhandler,
	dma2_ch1_irqhandler,	/* 72, 0x120 */
	dma2_ch2_irqhandler,
	dma2_ch3_irqhandler,
	dma2_ch4_5_irqhandler,
};
