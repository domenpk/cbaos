/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
//#include <mach/lpc13xx_regs.h>
#include <gpio.h>
#include <types.h>
#include <stdio.h>


struct lpc21xx_gpio {
	volatile u32 iopin;
	volatile u32 ioset;
	volatile u32 iodir;
	volatile u32 ioclr;
};

static struct lpc21xx_gpio * const LPC_GPIO = (struct lpc21xx_gpio *)0xe0028000;


void gpio_init(int pin, enum gpio_mode mode, int value)
{
	int orig_pin = pin;
	int port = (pin / 0x100) & 0x1;

	if ((pin & ~0x011f) != 0x4000)
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x1f;

	if (mode & GPIO_OUTPUT) {
		LPC_GPIO[port].iodir |= 1<<pin;
		gpio_set(orig_pin, value);
	} else {
		LPC_GPIO[port].iodir &= ~(1<<pin);
	}
}

void gpio_set(int pin, int value)
{
	int port = (pin / 0x100) & 0x1;
	if ((pin & ~0x011f) != 0x4000)
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x1f;
	if (value)
		LPC_GPIO[port].ioset = 1<<pin;
	else
		LPC_GPIO[port].ioclr = 1<<pin;
}

int gpio_get(int pin)
{
	int port = (pin / 0x100) & 0x1;
	if ((pin & ~0x011f) != 0x4000)
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x1f;

	return (LPC_GPIO[port].iopin >> pin) & 1;
}
