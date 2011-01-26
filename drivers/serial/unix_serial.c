/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <unistd.h>
#include <driver.h>

static int uart_probe(struct device *dev, int data)
{
	return 0;
}
static int uart_open(struct device *dev, int flags)
{
	// implement O_NONBLOCK
	return 0;
}
static void uart_close(struct device *dev)
{
}
static int uart_read(struct device *dev, void *buf, size_t count)
{
	return read(0, buf, count);
}
static int uart_write(struct device *dev, const void *buf, size_t count)
{
	return write(1, buf, count);
}

struct driver uart_driver = {
	.probe = uart_probe,
	.open = uart_open,
	.close = uart_close,
	.read = uart_read,
	.write = uart_write,
};
