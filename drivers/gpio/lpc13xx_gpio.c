/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#ifdef MACH_LPC13XX
#include <mach/lpc13xx_gpio.h>
#elif defined MACH_LPC11XX
#include <mach/lpc11xx_gpio.h>
#else
#error "this driver does not support your machine"
#endif
#include <stdio.h>
#include <gpio.h>
#include <types.h>
#include <compiler.h>
#include <sched.h>
#include <interrupt.h>
#include <mach/lpc13xx_interrupt.h>


#define dprintf(...)

struct LPC_GPIO {
	volatile u32 DATA[0x4000/4];
	u32 _reserved0[0x4000/4];
	volatile u32 DIR;
	volatile u32 IS;
	volatile u32 IBE;
	volatile u32 IEV;
	volatile u32 IE;
	volatile const u32 RIS;
	volatile const u32 MIS;
	volatile u32 IC;
	u32 _reserved1[(0x10000-0x8020)/4];
};

//static LPC_GPIO_TypeDef (* const LPC_GPIO[4]) = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3 };
struct LPC_GPIO *LPC_GPIO = (struct LPC_GPIO*)0x50000000;
static volatile u32 *IOCON = (volatile u32*)0x40044000;

void gpio_init(int pin, enum gpio_mode mode, int value)
{
	u32 iocon;
	int orig_pin = pin;
	int port = GPIO_PORT(pin);

	if (!(pin & GPIO_MARK))
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x1f;

	iocon = orig_pin>>20 & 0x3ff;
	if (mode & GPIO_OUTPUT) {
		LPC_GPIO[port].DIR |= 1<<pin;
		gpio_set(orig_pin, value);
	} else if (mode & GPIO_INPUT) {
		LPC_GPIO[port].DIR &= ~(1<<pin);

		if (mode == GPIO_INPUT_PD)
			iocon |= 1<<3;
		else if (mode == GPIO_INPUT_PU)
			iocon |= 2<<3;
	}
	IOCON[orig_pin>>12 & 0xff] = iocon;
}

void gpio_set(int pin, int value)
{
	int port = GPIO_PORT(pin);
	if (!(pin & GPIO_MARK))
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x1f;

	LPC_GPIO[port].DATA[1<<pin] = value<<pin;
}

int gpio_get(int pin)
{
	int port = GPIO_PORT(pin);
	if (!(pin & GPIO_MARK))
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x1f;

	return LPC_GPIO[port].DATA[1<<pin]>>pin & 1;
}

/* should not be needed, just use gpio_irq_register */
int gpio_irq_ispending(int pin)
{
	int port = GPIO_PORT(pin);
	pin &= 0x1f;

	int val = (LPC_GPIO[port].MIS >> pin) & 1;
	if (val) {
		LPC_GPIO[port].IC = 1<<pin;
		asm volatile ("nop\n nop"); /* lpc1343 um says so */
	}
	return val;
}

void gpio_irq_ack(int pin)
{
	int port = GPIO_PORT(pin);
	pin &= 0x1f;

	LPC_GPIO[port].IC = 1<<pin;
	asm volatile ("nop\n nop"); /* lpc1343 um says so */
}

void gpio_irq_setup(int pin, enum gpio_irq_edge edge)
{
	int port = GPIO_PORT(pin);
	if (!(pin & GPIO_MARK))
		printf("%s, invalid pin: %i\n", __func__, pin);

	pin &= 0x1f;

	LPC_GPIO[port].IE &= ~(1<<pin); /* disable */
	if (edge == GPIO_IRQ_DISABLED)
		return;

	LPC_GPIO[port].IS &= ~(1<<pin); /* edge (not level) */
	if (edge == GPIO_IRQ_EDGE_BOTH)
		LPC_GPIO[port].IBE |= (1<<pin); /* both edges */
	else {
		LPC_GPIO[port].IBE &= ~(1<<pin); /* IEV controls which edge */

		if (edge == GPIO_IRQ_EDGE_RISING)
			LPC_GPIO[port].IEV |= 1<<pin; /* rising edge */
		else
			LPC_GPIO[port].IEV &= ~(1<<pin); /* falling edge */
	}

	LPC_GPIO[port].IE |= 1<<pin; /* enable */
	irq_enable(IRQ_GPIO_0 - port); /* also enable NVIC */
}


static int (*gpio_irq_vec[3*12+6])(int);
static u32 gpio_irq_count[3*12+6];

/* it might make sense to just have debounce here */
#ifdef GPIO_DEBOUNCE
#define DEBOUNCE_TIME ms2ticks(2)
static u32 gpio_irq_time[3*12+6];
#endif

int gpio_irq_register(int opin, enum gpio_irq_edge edge, int (*handler)(int pin))
{
	int port = GPIO_PORT(opin);
	int pin = GPIO_PIN(opin);
	int idx = port*12+pin;

	if (gpio_irq_vec[idx] == NULL) {
		gpio_irq_vec[idx] = handler;
#ifdef GPIO_DEBOUNCE
		gpio_irq_time[idx] = ticks_now();
#endif
		gpio_irq_setup(opin, edge);
		dprintf("%s: GPIO_%d_%d registered by %p\n", __func__, port, pin,
				gpio_irq_vec[idx]);
		return 0;
	}

	printf("%s: GPIO_%d_%d already handled by %p\n", __func__, port, pin,
			gpio_irq_vec[idx]);
	return -1;
}

static void gpio_irqhandler(int port)
{
	u32 pending = LPC_GPIO[port].MIS;

	u32 mask = 1;
	int i;
	int idx = port*12;
	for (i=0; i<12; i++) {
		if (pending & mask) {
			dprintf("GPIO_%d_%d = %d, handler:%p:\n", port, i,
					gpio_irq_count[idx], gpio_irq_vec[idx]);
			/* ack before handling, so we don't miss anything */
			LPC_GPIO[port].IC = mask;
			//asm volatile ("nop\n nop"); /* lpc1343 um says so */

#ifdef GPIO_DEBOUNCE
			/* skip if handled in last DEBOUNCE_TIME */
			u32 now = ticks_now();
			if (time_after(gpio_irq_time[idx], now))
				continue;
#endif

			if (gpio_irq_vec[idx]) {
				gpio_irq_vec[idx](GPIO_PPIN(port, i));
#ifdef GPIO_DEBOUNCE
				gpio_irq_time[idx] = now + DEBOUNCE_TIME;
#endif
			} else
				printf("%s: GPIO_%d_%d has no handler\n",
						__func__, port, i);
			gpio_irq_count[idx]++;
		}
		mask <<= 1;

		if (++idx >= ALEN(gpio_irq_vec))
			break;
	}
}

void gpio_irq_debug(void)
{
	printf("%s\n", __func__);
	int i;
	u32 reghandl = 0;
	u32 totcount = 0;
	for (i=0; i<ALEN(gpio_irq_count); i++) {
		if (gpio_irq_count[i] || gpio_irq_vec[i]) {
			printf("GPIO_%d_%d = %d, handler:%p:\n", i/12, i%12,
					gpio_irq_count[i], gpio_irq_vec[i]);
			totcount += gpio_irq_count[i];
		}
		if (gpio_irq_vec[i]) {
			reghandl++;
		}
	}
	printf("%s: total: %d; registered handlers: %d\n", __func__, totcount, reghandl);
}

void __interrupt pioint0_irqhandler(void) { gpio_irqhandler(0); }
void __interrupt pioint1_irqhandler(void) { gpio_irqhandler(1); }
void __interrupt pioint2_irqhandler(void) { gpio_irqhandler(2); }
void __interrupt pioint3_irqhandler(void) { gpio_irqhandler(3); }
