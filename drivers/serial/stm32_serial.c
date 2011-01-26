/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdlib.h>
#include <errno.h>

#include <driver.h>
#include <device.h>

#include <types.h>
#include <drivers/stm32_serial.h>

/* supported chips:
 * stm32f103
 */


static int uart_direct_putchar(struct stm32_uart *uart, int c)
{
	while (!(uart->SR & 1<<7 /* TXE */))
		;
	uart->DR = c;

	return 0;
}

static int uart_direct_getchar(struct stm32_uart *uart)
{
	if (!(uart->SR & 1<<5 /* RXNE */))
		return -EAGAIN;

	return uart->DR;
}


static int uart_simple_probe(struct device *dev, void *data)
{
	struct stm32_uart_data *uart_data = data;
	const int BR = uart_data->baudrate;
	struct stm32_uart *uart = uart_data->base;

	if (!data)
		return -EINVAL;

	uart->BRR = (uart_data->parent_clk+BR/2)/BR;
	uart->CR2 = 0;
	uart->CR3 = 0;
	uart->CR1 = 1<<13 | 1<<3 | 1<<2;

	dev->priv = uart_data;

	return 0;
}


static void uart_simple_shutdown(struct device *dev)
{
	/* irq disable, gpio remap back */
}
static int uart_simple_open(struct device *dev, int flags)
{
	// implement O_NONBLOCK
	return 0;
}
static void uart_simple_close(struct device *dev)
{
}
static int uart_simple_read(struct device *dev, void *_buf, size_t count)
{
	char * const buf = _buf;
	struct stm32_uart_data *uart_data = dev->priv;
	int i;

	for (i=0; i<count; i++) {
		int tmp = uart_direct_getchar(uart_data->base);
		if (tmp >= 0) {
			buf[i] = tmp;
		} else {
			if (i > 0)
				return i;
			else
				return tmp;
		}
	}
	return count;
}
static int uart_simple_write(struct device *dev, const void *_buf, size_t count)
{
	const char * const buf = _buf;
	struct stm32_uart_data *uart_data = dev->priv;
	int i;

	for (i=0; i<count; i++)
		uart_direct_putchar(uart_data->base, buf[i]);
	return count;
}

struct driver serial_driver = {
	.probe = uart_simple_probe,
	.shutdown = uart_simple_shutdown,
	.open = uart_simple_open,
	.close = uart_simple_close,
	.read = uart_simple_read,
	.write = uart_simple_write,
};

