/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <gpio.h>
#include <types.h>
#include <stdio.h>
#include <mach/stm32_regs.h>
#include <mach/stm32_gpio.h>


void gpio_init(int pin, enum gpio_mode mode, int value)
{
	int port = (pin / 0x100) & 0x7;
	u32 data = 0;

	if ((pin & ~0x070f) != 0x4000)
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x0f;

	if (mode == GPIO_INPUT_PU || ((mode & GPIO_OUTPUT) && value == 1))
		STM32_GPIO(port)->BSRR = 1<<pin;
	else if (mode == GPIO_INPUT_PD || ((mode & GPIO_OUTPUT) && value == 0))
		STM32_GPIO(port)->BRR = 1<<pin;

	if (mode & GPIO_OUTPUT) {
		data = 0x3;
		if (mode == GPIO_OUTPUT_SLOW)
			data = 0x2;
	} else {
		if (mode == GPIO_INPUT)
			data = 0x4;
		else if (mode == GPIO_INPUT_PU)
			data = 0x8;
		else if (mode == GPIO_INPUT_PD)
			data = 0x8;
		else if (mode == GPIO_ANALOG)
			data = 0x0;
	}
	if (pin < 8) {
		STM32_GPIO(port)->CRL &= ~(0xf << (pin*4));
		STM32_GPIO(port)->CRL |= data << (pin*4);
	} else {
		pin -= 8;
		STM32_GPIO(port)->CRH &= ~(0xf << (pin*4));
		STM32_GPIO(port)->CRH |= data << (pin*4);
	}
}

void gpio_set(int pin, int value)
{
	int port = (pin / 0x100) & 0x7;
	if ((pin & ~0x070f) != 0x4000)
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x0f;
	if (value)
		STM32_GPIO(port)->BSRR = 1<<pin;
	else
		STM32_GPIO(port)->BRR = 1<<pin;
}

int gpio_get(int pin)
{
	int port = (pin / 0x100) & 0x7;
	if ((pin & ~0x070f) != 0x4000)
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x0f;

	return (STM32_GPIO(port)->IDR >> pin) & 1;
}
