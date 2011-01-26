/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <board.h>
#include <mach/stm32_regs.h>
#include <drivers/stm32_serial.h>
#include <device.h>



void tty0_init()
{
	static struct device tty0;
	static struct stm32_uart_data uart_data;	

	/* usart1_tx/PA9 as AFpp, usart1_rx/PA10 as input */
	STM32_RCC->APB2ENR |= 1<<2; /* IOPAEN */
	STM32_GPIO(0)->CRH &= ~0xf0; /* PA9 */
	STM32_GPIO(0)->CRH |= 0xb0;
	STM32_GPIO(0)->CRH &= ~0xf00; /* PA10 */
	STM32_GPIO(0)->CRH |= 0x400;

	STM32_RCC->APB2ENR |= 1<<14; /* USART1EN */

	uart_data.base = (void*)STM32_USART1_BASE;
	uart_data.parent_clk = CONFIG_FCPU;
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
		"subs	%0, %0, #1\n\t"		/* 1 cycle */
		"bne	.udelay_%=\n\t"	/* 3 cycles */
		: "=r" (x) : "r" (x) : "cc" );
}

void board_init()
{
	STM32_RCC->APB2ENR |= 0xffffffff; /* just enable every peripheral, makes development easier */

	/* limits: AHB, APB2 - 72 MHz, APB1 - 36 MHz, ADCCLK - 14 MHz
	 * with HSI (8 MHz), you can achieve max 64 MHz
	 */

	/* set up PLL with HSI, 64 MHz */
	STM32_RCC->CFGR &= ~0x3; /* switch to HSI clock */
	STM32_RCC->CR &= ~(1<<24); /* pll off */

	STM32_RCC->CFGR &= ~0xfff0; /* all dividers to 0 */
	STM32_RCC->CFGR |= 0x8400; /* ADCCLK = PCLK2/6, APB1 = HCLK/2 */

	STM32_RCC->CFGR &= ~(0xf<<18 | 1<<16);
	STM32_RCC->CFGR |= 0xf<<18; /* PLL = 16*HSI/2 */
	STM32_RCC->CR |= 1<<24; /* PLL on */
	while ((STM32_RCC->CR & 1<<25) == 0)
		;

#if CONFIG_FCPU > 48000000
	FLASH_ACR = 1<<4 | 2; /* prefetch enabled, 2 wait states */
#elif CONFIG_FCPU > 24000000
	FLASH_ACR = 1<<4 | 1; /* prefetch enabled, 1 wait state */
#else
	FLASH_ACR = 1<<4 | 0; /* prefetch enabled, 0 wait states */
#endif

	STM32_RCC->CFGR |= 0x2; /* switch to PLL clock */
}
