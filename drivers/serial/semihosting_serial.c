/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */

#include <arch/semihosting.h>
#include <driver.h>
#include <device.h>
#include <types.h>

static int semihosting_probe(struct device *dev, void *data)
{
	return 0;
}
static int semihosting_open(struct device *dev, int flags)
{
	u32 cmdblock[3] = { (u32)":tt", 0 /* "r" */, 3 /* strlen(":tt") */ };
	dev->priv = (void*)semihosting(SEMIHOSTING_OPEN, cmdblock);
	return 0;
}
static void semihosting_close(struct device *dev)
{
	u32 cmdblock[1] = { (u32)dev->priv };
	semihosting(SEMIHOSTING_CLOSE, cmdblock);
}
static int semihosting_read(struct device *dev, void *buf, size_t count)
{
	return 0; /* NOTE needs separate open fd with qemu */
}
static int semihosting_write(struct device *dev, const void *buf, size_t count)
{
	u32 cmdblock[3] = { (u32)dev->priv, (u32)buf, (u32)count };
	semihosting(SEMIHOSTING_WRITE, cmdblock);
#if 0
	size_t i;
	for (i=0; i<count; i++) {
		semihosting(SEMIHOSTING_WRITEC, buf+i);
	}
#endif
	return count;
}

struct driver semihosting_serial = {
	.probe = semihosting_probe,
	.open = semihosting_open,
	.close = semihosting_close,
	.read = semihosting_read,
	.write = semihosting_write,
};
