/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <string.h>

#include <sched.h>
#include <lock.h>
#include <compiler.h>

#include <gpio.h>

#include <board.h>

#ifdef BOARD_LPCXPRESSO
#include <mach/lpc13xx_gpio.h>
#define GPIO_LED GPIO_3_2
#define GPIO_1W  GPIO_1_10

#elif defined BOARD_LPCXPRESSO1114
#include <mach/lpc11xx_gpio.h>
#define GPIO_LED GPIO_0_7

#elif defined BOARD_ITLPC2138
#include <mach/lpc21xx_gpio.h>
#define GPIO_LED GPIO_0_7

#elif defined BOARD_STM32F103X4_MINE
#include <mach/stm32_gpio.h>
#define GPIO_LED GPIO_PB10
#define GPIO_1W  GPIO_PB1

#elif defined ARCH_UNIX
#define GPIO_LED 0
#endif

static void my_pwm(int led, int state)
{
	int i;
	for (i=1; i<20; i++) {
		gpio_set(led, state);
		msleep(20-i);
		gpio_set(led, state^1);
		msleep(i);
	}
}


static void blinky_func(u32 arg)
{
	int i = 0;
	int led = arg;
	gpio_init(led, GPIO_OUTPUT, 0);

	while (1) {
		printf("%s, %i\n", __func__, i++);
		my_pwm(led, 0);
		msleep(100);
		my_pwm(led, 1);
		msleep(100);

		/* force debug shell entry */
		if (0 && i == 4) {
			volatile u32 *tmp = (volatile u32*)0x12345678;
			*tmp = 0xcba;
		}
	}
}


#define STACK 128
static u32 blinky_stack[STACK];
static struct task blinky_task;


void is_it_working()
{
	blinky_task = (struct task) {
		.name = "blinky",
		.stack = blinky_stack,
		.stack_len = ALEN(blinky_stack),
	};

	gpio_init(GPIO_LED, GPIO_OUTPUT, 1);
	udelay(500*1000);
	gpio_set(GPIO_LED, 0);
	udelay(500*1000);

	/* uart test */
	printf("%s, uart is working\n", __func__);
	gpio_set(GPIO_LED, 1);

	printf("%s, starting scheduler\n", __func__);

	sched_init();

	task_new(&blinky_task, blinky_func, GPIO_LED, 0);

#ifdef GPIO_1W
	extern void test_1w_func(u32 arg);
	static u32 w1_stack[STACK];
	static struct task w1_task = (struct task) {
		.name = "1w_test",
		.stack = w1_stack,
		.stack_len = ALEN(w1_stack),
	};
	task_new(&w1_task, test_1w_func, GPIO_1W, 0);
#endif

#if defined BOARD_LPCXPRESSO || defined BOARD_ITLPC2138
	extern void test_spi_func(u32 arg);
	static u32 spi_stack[STACK];
	static struct task spi_task = (struct task) {
		.name = "spi_test",
		.stack = spi_stack,
		.stack_len = ALEN(spi_stack),
	};
	task_new(&spi_task, test_spi_func, 0, 0);
#endif

#if defined BOARD_LPCXPRESSO || defined BOARD_ITLPC2138
	extern void test_i2c_func(u32 arg);
	static u32 i2c_stack[STACK];
	static struct task i2c_task = (struct task) {
		.name = "i2c_test",
		.stack = i2c_stack,
		.stack_len = ALEN(i2c_stack),
	};
	task_new(&i2c_task, test_i2c_func, 0, 0);
#endif

#if defined BOARD_ITLPC2138
	extern void test_hd44780_func(u32 arg);
	static u32 hd44780_stack[STACK];
	static struct task hd44780_task = (struct task) {
		.name = "hd44780_test",
		.stack = hd44780_stack,
		.stack_len = ALEN(hd44780_stack),
	};
	task_new(&hd44780_task, test_hd44780_func, 0, 0);
#endif

	sched_start();
}
