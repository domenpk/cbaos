#ifndef _DS2482_1W_H_
#define _DS2482_1W_H_

#include <drivers/1w.h>
#include <drivers/i2c.h>

struct ds2482_data {
	struct i2c_master *i2c;
	u8 addr;
};

int ds2482_1w_register(struct w1_master *master);

#endif
