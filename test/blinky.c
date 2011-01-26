/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>

#include <sched.h>
#include <lock.h>
#include <compiler.h>

#include <gpio.h>

#ifdef BOARD_LPCXPRESSO
#include <mach/lpc13xx_gpio.h>
#define GPIO_LED GPIO_0_7
#else
#define GPIO_LED 0
#endif

static void blinky_func(u32 arg)
{
	int i= 0;
	gpio_init(GPIO_LED, GPIO_OUTPUT, 0);

	while (1) {
		printf("%s, loop %i\n", __func__, i);
		gpio_set(GPIO_LED, i%2);
		msleep(500);
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
