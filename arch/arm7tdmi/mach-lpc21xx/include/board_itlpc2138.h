#ifndef _BOARD_ITLPC2138_H_
#define _BOARD_ITLPC2138_H_

#include <drivers/spi.h>
#include <drivers/i2c.h>

extern struct spi_master spi0;
extern struct spi_master spi1;
extern struct i2c_master i2c0;
extern struct i2c_master i2c1;

#endif
