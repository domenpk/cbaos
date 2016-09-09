/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <driver.h>

static int null_probe(struct device *dev, void *data)
{
	return 0;
}
static int null_open(struct device *dev, int flags)
{
	return 0;
}
static void null_close(struct device *dev)
{
}
static int null_read(struct device *dev, void *buf, size_t count)
{
	return 0; // hmm? it should just wait indefinitely
}
static int null_write(struct device *dev, const void *buf, size_t count)
{
	return count;
}

struct driver null_driver = {
	.probe = null_probe,
	.open = null_open,
	.close = null_close,
	.read = null_read,
	.write = null_write,
};
