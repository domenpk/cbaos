#ifndef _GPIO_H_
#define _GPIO_H_

enum gpio_mode {
	GPIO_INPUT = 0x10,
	GPIO_INPUT_PU = 0x11,
	GPIO_INPUT_PD = 0x12,
	GPIO_OUTPUT = 0x20,
	GPIO_OUTPUT_SLOW = 0x21,
	GPIO_ANALOG = 0x30,
};

void gpio_init(int pin, enum gpio_mode mode, int value);
void gpio_set(int pin, int value);
int gpio_get(int pin);

#endif
