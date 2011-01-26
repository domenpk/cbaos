/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <sched.h>

#include <drivers/spi.h>
#include <drivers/spi_tmp124.h>

#ifdef BOARD_LPCXPRESSO
#include <mach/lpc13xx_gpio.h>
#include <mach/board_lpcxpresso.h>
#define GPIO_CS0 GPIO_2_10
#define GPIO_CS1 GPIO_2_2

#elif defined BOARD_ITLPC2138
#include <mach/lpc21xx_gpio.h>
#include <mach/board_itlpc2138.h>
#define GPIO_CS0 GPIO_0_7
#define GPIO_CS1 GPIO_0_10
#endif


void test_spi_func(u32 arg)
{
	int i = 0;

	struct spi_device sens1 = {
		.mode = 0,
		.cs_pin = GPIO_CS0,
		.clock = 10*1000*1000, /* 10 MHz max */
	};
	spi_register_device(&spi0, &sens1);
	struct spi_device sens2 = {
		.mode = 0,
		.cs_pin = GPIO_CS1,
		.clock = 10*1000*1000, /* 10 MHz max */
	};
	spi_register_device(&spi0, &sens2);

	while (1) {
		const char progress[] = { '+', 'x' };
		int t1 = tmp124_read(&sens1);
		int t2 = tmp124_read(&sens2);

		printf("%s: temperatures: %i.%02i, %i.%02i %c\n", __func__,
				t1/16, t1%16 * 100 / 16,
				t2/16, t2%16 * 100 / 16,
				progress[i++%2]);

		msleep(1000);
	}
}
