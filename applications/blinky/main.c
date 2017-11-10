/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>

#include <sched.h>
#include <lock.h>
#include <compiler.h>
#include <version.h>

#include <board.h>
#include <gpio.h>

#ifdef BOARD_LPCXPRESSO
#include <mach/lpc13xx_gpio.h>
#define GPIO_LED GPIO_0_7
#define GPIO_LED2 GPIO_0_1
#elif defined BOARD_LPCXPRESSO1114
#include <mach/lpc11xx_gpio.h>
#define GPIO_LED GPIO_0_7
#define GPIO_LED2 GPIO_0_1
#elif defined BOARD_CBA_LIGHTS_0_2
#include <mach/lpc13xx_gpio.h>
#define GPIO_LED GPIO_1_5
#define GPIO_LED2 GPIO_3_2
#elif defined BOARD_QEMU_NETDUINO2
#include <mach/stm32_gpio.h>
#define GPIO_LED GPIO_PA0
#define GPIO_LED2 GPIO_PA1
#elif defined BOARD_QEMU_NETDUINO2_CM0
#include <mach/lpc11xx_gpio.h>
#define GPIO_LED GPIO_0_7
#define GPIO_LED2 GPIO_0_1
#else
#warning "no board defined"
#endif


static void blinky_func(u32 arg)
{
	int i=0;
	gpio_init(GPIO_LED, GPIO_OUTPUT, 0);
	gpio_init(GPIO_LED2, GPIO_OUTPUT, 1);

	while (1) {
		int on = i%2;
		printf("%s, loop %i\n", __func__, i);
		gpio_set(GPIO_LED, on);
		gpio_set(GPIO_LED2, !on);
		msleep(500);
		i++;
	}
}

/* this is basically a hw test */
static void blinky_nosched(void)
{
	int i=0;
	gpio_init(GPIO_LED, GPIO_OUTPUT, 0);
	gpio_init(GPIO_LED2, GPIO_OUTPUT, 1);

	while (i < 10) {
		int on = i%2;
		if (i == 5) {
			const char *s = "putchar\r\n";
			while (*s)
				putchar(*s++);
		} else
		if (i == 6) {
			const char s[] = "fwrite\r\n";
			fwrite(stdout, s, sizeof(s));
		} else
		if (i == 7) {
			printf("%s, printf test; build: \"%s\"\n", __func__, VERSION);
		} else
		if (i > 7) {
			printf("%s, printf test %i\n", __func__, i);
		}

		gpio_set(GPIO_LED, on);
		gpio_set(GPIO_LED2, !on);
		udelay(500*1000);
		i++;
	}
}


#ifdef ARCH_UNIX
#define STACK 2048
#else
#define STACK 128
#endif
static u32 blinky_stack[STACK];
static struct task blinky_task;

void blinky()
{
	blinky_task = (struct task) {
		.name = "blinky",
		.stack = blinky_stack,
		.stack_len = ALEN(blinky_stack),
	};

	sched_init();

	task_new(&blinky_task, blinky_func, 0, 0);

	sched_start();
}

int main()
{
	blinky_nosched();

	blinky();
	return 0;
}
