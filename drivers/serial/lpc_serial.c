/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdlib.h>
#include <errno.h>

#include <driver.h>
#include <device.h>

#include <types.h>
#include <drivers/lpc_serial.h>

/* supported chips:
 * lpc13xx
 * lpc11xx
 * lpc213x - no FDR
 */


static int uart_direct_putchar(struct lpc_uart *uart, int c)
{
	while (!(uart->LSR & 0x20))
		;
	uart->THR = c;

	return 0;
}

static int uart_direct_getchar(struct lpc_uart *uart)
{
	if (!(uart->LSR & 0x01))
		return -EAGAIN;

	return uart->RBR;
}


/* algorithm to find the best dividers for uart */
static void uart_best_dividers(int pclk, int br, u16 *DL, u8 *FDR)
{
	int dl;
	int dv, mv;

	dl = (pclk+8*br)/(16*br);

	int savedl = dl, savefdr = 0x10;
	int br_diff = br;

	if (dl*br*16 != pclk) {
		br_diff = abs(pclk/16/dl-br);

		for (dv=1; dv<=14; dv++)
			for (mv=dv+1; mv<=15; mv++) {
				int tmp = 16*br*(mv+dv);
				int newbr = pclk*mv/16/dl/(mv+dv);
				dl = (pclk*mv+tmp/2)/tmp;
				if (dl >= 3 && abs(newbr-br) < br_diff) {
					br_diff = abs(newbr-br);
					savedl = dl;
					savefdr = mv<<4 | dv;
				}
			}
	}

	*DL = savedl;
	*FDR = savefdr;
#if 0
	dl = savedl;
	mv = savefdr>>4 & 0xf;
	dv = savefdr & 0xf;
	br_diff = abs(br-pclk*mv/16/dl/(mv+dv));
	/* error: 100*br_diff/br % */
#endif
}

static int uart_simple_probe(struct device *dev, void *data)
{
	struct lpc_uart_data *uart_data = data;
	const int BR = uart_data->baudrate;
	struct lpc_uart *uart = uart_data->base;

	if (!data)
		return -EINVAL;

	uart->LCR = 0x83;
	if (uart_data->flags & LPC_UART_FDR) {
		u16 div;
		u8 fdr;
		uart_best_dividers(uart_data->pclk, BR, &div, &fdr);
		uart->DLM = div>>8;
		uart->DLL = div&0xff;
		uart->FDR = fdr;
	} else {
		int divider = (uart_data->pclk/16+BR/2)/BR;
		uart->DLM = divider>>8;
		uart->DLL = divider&0xff;
	}
	uart->LCR = 0x03;
	uart->FCR = 0x07; /* enable and reset fifos, trigger level for RX int - 1 char */

	dev->priv = uart_data;

	return 0;
}


static void uart_simple_shutdown(struct device *dev)
{
//	LPC_SYSCON->SYSAHBCLKCTRL &= ~SYSAHBCLKCTRL_UART;
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
	struct lpc_uart_data *uart_data = dev->priv;
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
	struct lpc_uart_data *uart_data = dev->priv;
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

