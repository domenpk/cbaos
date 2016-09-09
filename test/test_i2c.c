/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <drivers/i2c.h>

#ifdef BOARD_LPCXPRESSO
#include <mach/board_lpcxpresso.h>

#elif defined BOARD_ITLPC2138
#include <mach/board_itlpc2138.h>

#elif defined BOARD_STM32F103X4_MINE
#include <mach/board_stm32f103x4_mine.h>
#endif


void test_i2c_func(u32 arg)
{
	int i = 0;

	/* i2c scanning */
	for (i=0; i<0x7f; i++) {
		//u8 tmp;
		if (i2c_write(&i2c0, i, NULL, 0) == 0)
			printf("%s, i2c0: %x_w present\n", __func__, i);
		/* must read one byte with read scanning, only then can master NACK and STOP */
		//if (i2c_read(&i2c0, i, &tmp, 1) == 0)
		if (i2c_read(&i2c0, i, NULL, 0) == 0) // XXX for now, till lpc i2c driver is fixed
			printf("%s, i2c0: %x_r present\n", __func__, i);
	}

	while (1) {
		/* basically this is all you need to get tmp275 working */
		u8 buf[2];

		if (i2c_read(&i2c0, 0x4f, (u8*)buf, 2) != 0) {
			printf("%s, i2c temp fail\n", __func__);
		} else {
			int t1 = buf[0]<<8 | buf[1];
			t1 = t1 >> 4;
			printf("%s, temperature: %i.%02i\n", __func__,
					t1/16, t1%16 * 100 / 16);
		}

		msleep(1000);
	}
}
