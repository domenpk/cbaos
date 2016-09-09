/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>

#include <sched.h>
#include <lock.h>
#include <compiler.h>
#include <cbashell.h>

#include <gpio.h>

#ifdef BOARD_LPCXPRESSO
#include <mach/lpc13xx_gpio.h>
#define GPIO_LED GPIO_0_7
#define GPIO_LED2 GPIO_0_1
#elif defined BOARD_LPCXPRESSO1114
#include <mach/lpc11xx_gpio.h>
#define GPIO_LED GPIO_0_7
#define GPIO_LED2 GPIO_0_1
#else
#define GPIO_LED 0
#endif

static void blinky_func(u32 arg)
{
	int i= 0;
	gpio_init(GPIO_LED, GPIO_OUTPUT, 0);
	gpio_init(GPIO_LED2, GPIO_OUTPUT, 0);

	while (1) {
		gpio_set(GPIO_LED, i%2);
		gpio_set(GPIO_LED2, i%2);
		msleep(500);
		i++;
	}
}

static void cbashell_func(u32 arg)
{
	printf("\nentering cbashell\n");
	cbashell_init();
	while (1) {
		int c;
		c = getchar();
		if (c >= 0)
			cbashell_charraw(c);
	}
}

#ifdef ARCH_UNIX
#define STACK 2048
#else
#define STACK 128
#endif
static u32 blinky_stack[STACK];
static struct task blinky_task;

static u32 cbashell_stack[STACK];
static struct task cbashell_task;

int main()
{
	blinky_task = (struct task) {
		.name = "blinky",
		.stack = blinky_stack,
		.stack_len = ALEN(blinky_stack),
	};
	cbashell_task = (struct task) {
		.name = "cbashell",
		.stack = cbashell_stack,
		.stack_len = ALEN(cbashell_stack),
	};

	sched_init();

	task_new(&blinky_task, blinky_func, 0, 0);
	task_new(&cbashell_task, cbashell_func, 0, 0);

	sched_start();

	return 0;
}
