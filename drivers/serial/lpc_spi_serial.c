/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdlib.h>
#include <errno.h>

#include <driver.h>
#include <device.h>

#include <types.h>
#include <drivers/lpc_ssp.h>


static struct lpc_ssp *spi = (void*)0x40040000;
static int spi_simple_probe(struct device *dev, void *data)
{
	return 0;
}


static void spi_simple_shutdown(struct device *dev)
{
	/* irq disable, gpio remap back */
}
static int spi_simple_open(struct device *dev, int flags)
{
	// i'm cheating, i know
	spi->SSPCR1 = 0; /* disable */

	// ok, this is ugly and everything, drop speed to 50khz, so 115k uart bridge can handle it w/o any buffers
	spi->SSPCR0 = 7 | 19<<8; /* 8 bit transfer, clock /20 */
	spi->SSPCPSR = CONFIG_FCPU / 1000000; /* 1 MHz */
	spi->SSPIMSC = 0; /* no interrupts */
	spi->SSPCR1 = 1<<1; /* enable */

	// implement O_NONBLOCK
	return 0;
}
static void spi_simple_close(struct device *dev)
{
}
static int spi_simple_read(struct device *dev, void *_buf, size_t count)
{
	return 0;
}
static int spi_simple_write(struct device *dev, const void *_buf, size_t count)
{
	const char * const buf = _buf;
	int i;

	for (i=0; i<count; i++) {
		while ((spi->SSPSR & 1<<1) /* TNF */ == 0)
			;
		spi->SSPDR = buf[i];

#if 0
		while ((spi->SSPSR & 1<<2) /* RNE */ == 0)
			;
		u8 dummy = spi->SSPDR;
#endif
	}
	return count;
}

struct driver serial_spi_driver = {
	.probe = spi_simple_probe,
	.shutdown = spi_simple_shutdown,
	.open = spi_simple_open,
	.close = spi_simple_close,
	.read = spi_simple_read,
	.write = spi_simple_write,
};

