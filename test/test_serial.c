/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <sched.h>
#include <types.h>

void test_serial_func(u32 arg)
{
	int i = 0;
	FILE f;
	int r;

	r = fopen(&f, "/dev/serial", 0);
	//r = fopen(&f, "/dev/null", 0);
	if (r) {
		printf("%s, failed\n", __func__);
		while (1)
			msleep(1000);
	}

	while (1) {
		fprintf(&f, "%s, loop %i\n", __func__, i);
		msleep(1000);
		i++;
	}
}
