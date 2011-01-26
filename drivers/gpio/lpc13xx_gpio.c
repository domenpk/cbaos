/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#ifdef MACH_LPC13XX
#include <mach/lpc13xx_regs.h>
#elif defined MACH_LPC11XX
#include <mach/lpc11xx_regs.h>
#else
#error "this driver does not support your machine"
#endif
#include <stdio.h>
#include <gpio.h>

static LPC_GPIO_TypeDef (* const LPC_GPIO[4]) = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3 };

void gpio_init(int pin, enum gpio_mode mode, int value)
{
	int orig_pin = pin;
	int port = (pin / 0x100) & 0x3;

	if ((pin & ~0x031f) != 0x4000)
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x1f;

	if (mode & GPIO_OUTPUT) {
		LPC_GPIO[port]->DIR |= 1<<pin;
		gpio_set(orig_pin, value);
	} else {
		LPC_GPIO[port]->DIR &= ~(1<<pin);
	}
}

void gpio_set(int pin, int value)
{
	int port = (pin / 0x100) & 0x3;
	if ((pin & ~0x031f) != 0x4000)
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x1f;

	LPC_GPIO[port]->MASKED_ACCESS[1<<pin] = value<<pin;
}

int gpio_get(int pin)
{
	int port = (pin / 0x100) & 0x3;
	if ((pin & ~0x031f) != 0x4000)
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x1f;

	return LPC_GPIO[port]->MASKED_ACCESS[1<<pin]>>pin & 1;
}
