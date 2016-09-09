/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <board.h>
#include <gpio.h>
#include <mach/stm32_regs.h>
#include <mach/stm32_gpio.h>
#include <drivers/stm32_serial.h>
#include <drivers/i2c.h>
#include <drivers/stm32_i2c.h>
#include <device.h>
#include <interrupt.h>


void stm32_pin_config(int pin, int mode)
{
	int port = (pin / 0x100) & 0x7;

	pin &= 0x0f;
	if (pin < 8) {
		STM32_GPIO(port)->CRL &= ~(0xf << (pin*4));
		STM32_GPIO(port)->CRL |= mode << (pin*4);
	} else {
		pin -= 8;
		STM32_GPIO(port)->CRH &= ~(0xf << (pin*4));
		STM32_GPIO(port)->CRH |= mode << (pin*4);
	}
}

void tty0_init()
{
	static struct device tty0;
	static struct stm32_uart_data uart_data;	

	/* usart1_tx/PA9 as AFpp, usart1_rx/PA10 as input */
	STM32_RCC->APB2ENR |= APB2_IOPA;
	stm32_pin_config(GPIO_PA9, 0xb); /* USART1_TX - AF push-pull */
	stm32_pin_config(GPIO_PA10, 0x4); /* USART1_RX - input */

	STM32_RCC->APB2ENR |= APB2_USART1;

	uart_data.base = (void*)STM32_USART1_BASE;
	uart_data.parent_clk = CONFIG_FCPU;
	uart_data.baudrate = 115200;

	tty0.name = "tty0";
	tty0.drv = &serial_driver;
	if (tty0.drv->probe(&tty0, &uart_data) == 0)
		device_register(&tty0);
}

#define STM32_I2C1_ENABLED
#ifdef STM32_I2C1_ENABLED
static struct stm32_i2c_data stm32_i2c_data0;
static struct stm32_i2c_data stm32_i2c_data1;

struct i2c_master i2c0 = {
	.priv = &stm32_i2c_data0,
	.speed = 100000,
};

struct i2c_master i2c1 = {
	.priv = &stm32_i2c_data1,
	.speed = 100000,
};

static int board_i2c0_init(int remap)
{
	STM32_RCC->APB2ENR |= APB2_IOPB;
	/* configure i2c pins as AF open drain 2 MHz */
	if (!remap) {
		stm32_pin_config(GPIO_PB6, 0xe); /* I2C1_SCL */
		stm32_pin_config(GPIO_PB7, 0xe); /* I2C1_SDA */
	} else {
		STM32_RCC->APB2ENR |= APB2_AFIO;
		STM32_AFIO->MAPR |= REMAP_I2C1;
		stm32_pin_config(GPIO_PB8, 0xe); /* I2C1_SCL */
		stm32_pin_config(GPIO_PB9, 0xe); /* I2C1_SDA */
	}

	STM32_RCC->APB1ENR |= APB1_I2C1;

	stm32_i2c_data0.regs = (void*)STM32_I2C1_BASE;
	stm32_i2c_data0.parent_clk = CONFIG_FCPU/2; /* APB1, which is max 36 MHz */
	stm32_i2c_data0.irq_ev = IRQ_I2C1_EV;
	stm32_i2c_data0.irq_er = IRQ_I2C1_ER;
	stm32_i2c_data0.bus = 0;
	stm32_i2c_register(&i2c0);
	i2c_register_master(&i2c0);

	return 0;
}

static int board_i2c1_init(void)
{
	STM32_RCC->APB2ENR |= APB2_IOPB;
	/* configure i2c pins as AF open drain 2 MHz */
	stm32_pin_config(GPIO_PB10, 0xe); /* I2C2_SCL */
	stm32_pin_config(GPIO_PB11, 0xe); /* I2C2_SDA */

	STM32_RCC->APB1ENR |= APB1_I2C2;

	stm32_i2c_data1.regs = (void*)STM32_I2C2_BASE;
	stm32_i2c_data1.parent_clk = CONFIG_FCPU/2; /* APB1, which is max 36 MHz */
	stm32_i2c_data1.irq_ev = IRQ_I2C2_EV;
	stm32_i2c_data1.irq_er = IRQ_I2C2_ER;
	stm32_i2c_data1.bus = 1;
	stm32_i2c_register(&i2c1);
	i2c_register_master(&i2c1);

	return 0;
}
#else
static int board_i2c0_init() { return 0; }
static int board_i2c1_init() { return 0; }
#endif

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

	board_i2c0_init(0);

	/* set-up interrupt on PA0 transitioning to low and high - cbashell entry */
	gpio_init(GPIO_PA0, GPIO_INPUT_PU, 0);
	STM32_AFIO->EXTICR1 = 0; /* PA0 */
	STM32_EXTI->IMR |= 1;
	STM32_EXTI->EMR |= 1;
	STM32_EXTI->RTSR |= 1;
	STM32_EXTI->FTSR |= 1;
	irq_enable(IRQ_EXTI0);

}
