/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <string.h>

#include <sched.h>

#include <drivers/1w.h>
//#include <drivers/ds2482_1w.h>
#include <drivers/bitbang_1w.h>


void test_1w_func(u32 arg)
{
	int pin = arg;
	int i;
#if 0
	struct ds2482_data ds2482_data = {
		.i2c = &i2c0,
		.addr = 0x1b,
	};
	struct w1_master _w1, *w1 = &_w1;
	w1->priv = &ds2482_data;
	ds2482_1w_register(w1);
#else
	struct bitbang_1w_data bitbang_1w_data = {
		.pin = pin,
	};
	struct w1_master _w1, *w1 = &_w1;
	w1->priv = &bitbang_1w_data;
	bitbang_1w_register(w1);
#endif

	w1_addr_t addrs[5];
	int n = w1_scan(w1, addrs, 5);
	printf("found %i 1-wire devices:\n", n);
	for (i=0; i<n; i++) {
		u8 *b = addrs[i].bytes;
		printf(" %02x%02x%02x%02x%02x%02x%02x%02x\n",
				b[0], b[1], b[2], b[3],
				b[4], b[5], b[6], b[7]);
	}

	n = w1_read_rom(w1, &addrs[0]);
	printf("read_rom: %i\n", n);
	u8 *b = addrs[0].bytes;
	printf(" %02x%02x%02x%02x%02x%02x%02x%02x\n",
			b[0], b[1], b[2], b[3],
			b[4], b[5], b[6], b[7]);


	/* some example code for ds2890 1-wire digital potentiometer */
	w1_addr_t pot;
//	2c7147070000007b
	memcpy(pot.bytes, "\x2c\x71\x47\x07\x00\x00\x00\x7b", 8);

	/* read ctrl */
	w1_match_rom(w1, pot);
	w1_write(w1, '\xaa');
	u8 tmp1 = w1_read(w1);
	u8 tmp2 = w1_read(w1);
	printf("%s, 1w: feat:%02x, ctrl:%02x\n", __func__, tmp1, tmp2);

	/* read position */
	w1_match_rom(w1, pot);
	w1_write(w1, '\xf0');
	tmp1 = w1_read(w1);
	tmp2 = w1_read(w1);
	printf("%s, 1w: ctrl:%02x, wiper:%02x\n", __func__, tmp1, tmp2);

	/* write position */
	w1_match_rom(w1, pot);
	w1_write(w1, '\x0f');
	w1_write(w1, '\xf0');
	tmp1 = w1_read(w1);
	if (tmp1 == '\xf0')
		w1_write(w1, '\x96'); /* release code */
	printf("%s, 1w: new pos:%02x\n", __func__, tmp1);

	/* prepare for inc/dec */
	w1_match_rom(w1, pot);
	while (1) {
		if (i % 32 < 20)
			w1_write(w1, '\x99');
		else
			w1_write(w1, '\xc3');
		printf("%s, %i, wiper pos:%02x\n", __func__, i++, w1_read(w1));

		msleep(1000);
	}
}
