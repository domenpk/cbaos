/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <drivers/i2c.h>

#ifdef BOARD_LPCXPRESSO
#include <mach/board_lpcxpresso.h>

#elif defined BOARD_ITLPC2138
#include <mach/board_itlpc2138.h>
#endif


void test_i2c_func(u32 arg)
{
	int i = 0;

	/* i2c scanning */
	for (i=0; i<0x7f; i++) {
		if (i2c_write(&i2c0, i, NULL, 0) == 0)
			printf("%s, i2c0: %x_w present\n", __func__, i);
		if (i2c_read(&i2c0, i, NULL, 0) == 0)
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
