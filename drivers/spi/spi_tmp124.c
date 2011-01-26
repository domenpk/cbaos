/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <drivers/spi_tmp124.h>
#include <drivers/spi.h>
#include <types.h>

/*
 * This is a simple temperature sensor. We do not implement anything
 * else but reading the temperature.
 * The temperature is returned in degC*16.
 */
int tmp124_read(struct spi_device *device)
{
	u8 tmp[2];
	s16 data;

	struct spi_transfer transfer;
	transfer.tx_buf = tmp;
	transfer.rx_buf = tmp;
	transfer.len = 2;

	spi_transfer(device, &transfer);

	data = tmp[0]<<8 | tmp[1];

	/* rely on sign extended shift to work */
	return data >> 3;
}
