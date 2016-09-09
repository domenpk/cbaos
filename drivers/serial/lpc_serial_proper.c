/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */

#include <types.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <circ_buf.h>
#include <sem.h>
#include <interrupt.h>
#include <driver.h>
#include <device.h>
#include <ioctl.h>

#include <drivers/lpc_serial_proper.h>


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

/* a more generic way would be: struct stdstream, bufs, init func, putchar, getchar funcs */
static void uart_set_baudrate(struct lpc_uart_proper_data *data, int BR)
{
	struct lpc_uart_proper_data *uart_data = data;
	struct lpc_uart *uart = uart_data->base;
	
	uart_data->baudrate = BR;

	uart->LCR |= 0x80;
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
	uart->LCR &= ~0x80;
}

static void uart_set_parity(struct lpc_uart_proper_data *data, int parity)
{
	struct lpc_uart_proper_data *uart_data = data;
	struct lpc_uart *uart = uart_data->base;
	
	uart_data->parity = parity;

	uart->LCR &= ~(1<<3 | 3<<4);
	if (parity) {
		if (parity == 2)
			uart->LCR |= 1<<4;
		uart->LCR |= 1<<3;
	}
}


static struct uart_irq_data {
	struct lpc_uart_proper_data *data;
} uart_irq_data[1];

#if 0
#include <compiler.h>
__naked void uart_irqhandler()
{
	asm volatile (
		"tst	lr, #0x4\n\t" /* process task, see EXC_RETURN */
		"ite	eq\n\t"
		"mrseq	r0, MSP\n\t"
		"mrsne	r0, PSP\n\t"
		"ldr	r0, [r0, #0x14]\n\t"
		"b	uart_irqhandler_\n"
	);
}

void uart_irqhandler_(u32 lr)
#endif
void uart_irqhandler()
{
	struct lpc_uart_proper_data *data = uart_irq_data[0].data;
	struct lpc_uart *uart = data->base;

	u8 source = uart->IIR & 0xf;
	if (source == 0x2 /* THRE */) {
		int tmp = circ_buf_get_one(&data->tx_buf);
		if (tmp >= 0)
			uart->THR = tmp;
		mutex_up(&data->tx_mutex);
	} else
	if (source == 0x4 /* RDA */) {
		int lsr;
		//log_add(lr);
		/* while there's data in fifo */
		while ((lsr = uart->LSR) & 1<<0) {
			data->debug_rx_bytes++;
			if (circ_buf_put_one(&data->rx_buf, uart->RBR) < 0)
				data->debug_rx_overrun++;
			if (lsr & 1<<1)
				data->debug_rx_overrun |= 0x8000;
		}
		mutex_up(&data->rx_mutex);
		//log_add(0xa0000000 | (ticks_now() & 0xfffffff));
	}
}

static void uart_enable(struct lpc_uart_proper_data *uart_data)
{
	struct lpc_uart *uart = uart_data->base;

	uart->FCR = 0x07; /* enable and reset fifos, trigger level for RX int - 1 char */
	//uart->FCR = 0x00; //XXX there's no difference?!

	irq_enable(uart_data->irq);
	uart->IER |= 1<<1 /* THRE */ | 1<<0 /* RBR */;
}
static void uart_disable(struct lpc_uart_proper_data *uart_data)
{
	struct lpc_uart *uart = uart_data->base;

	while ((uart->LSR & 1<<5 /* THRE */) == 0)
		;
	uart->IER &= ~(1<<1 /* THRE */ | 1<<0 /* RBR */);
	irq_disable(uart_data->irq);
	mutex_up(&uart_data->rx_mutex); /* make rx finish */
}

static int uart_probe(struct device *dev, void *data)
{
	struct lpc_uart_proper_data *uart_data = data;
	struct lpc_uart *uart = uart_data->base;

	dev->priv = data;
	uart_irq_data[0].data = data;

	mutex_init(&uart_data->rx_mutex, 1);
	mutex_init(&uart_data->tx_mutex, 1);

	uart->LCR = 3; /* 8 data bits, no parity, 1 stop bit */
	uart_set_baudrate(uart_data, uart_data->baudrate);
	uart_enable(uart_data);

	return 0;
}
static void uart_shutdown(struct device *dev)
{
	struct lpc_uart_proper_data *uart_data = dev->priv;

	uart_disable(uart_data);
	//LPC_SYSCON->SYSAHBCLKCTRL &= ~SYSAHBCLKCTRL_UART;
	/* irq disable, gpio remap back */
}
static int uart_open(struct device *dev, int flags)
{
	// implement O_NONBLOCK
	return 0;
}
static void uart_close(struct device *dev)
{
}

/* hmm... 1s timeouts, now, for tx, this shouldn't be a problem, but for RX... maybe user wants to wait more time? fcntl setting?
 */
static int uart_read(struct device *dev, void *buf, size_t count)
{
	struct lpc_uart_proper_data *uart_data = dev->priv;
	int r;

	r = circ_buf_get(&uart_data->rx_buf, buf, count);
	if (r > 0 || (dev->flags & O_NONBLOCK))
		return r;

	/* otherwise wait for some data */
	r = mutex_down(&uart_data->rx_mutex, ms2ticks(1000));
	if (r < 0)
		return r;
	//log_add(0xb0000000 | (ticks_now() & 0xfffffff));

	r = circ_buf_get(&uart_data->rx_buf, buf, count);
	return r;
}

static int uart_write(struct device *dev, const void *buf, size_t count)
{
	struct lpc_uart_proper_data *uart_data = dev->priv;
	struct lpc_uart *uart = uart_data->base;
	int r;
	int lsr;

	/* man i hate this hack. maybe up/down with count argument would be nicer */
	r = mutex_downtry(&uart_data->tx_mutex);
	r = circ_buf_put(&uart_data->tx_buf, buf, count);
	if (r > 0)
		goto out;
	if (dev->flags & O_NONBLOCK)
		return r;

	r = mutex_down(&uart_data->tx_mutex, ms2ticks(1000));
	if (r < 0)
		return r;

	r = circ_buf_put(&uart_data->tx_buf, buf, count);
	if (r > 0)
		goto out;

	return r;

 out:
	/* start transmitter */
	lsr = uart->LSR;
	if (lsr & 1<<6 /* TEMT */)
		uart->THR = circ_buf_get_one(&uart_data->tx_buf);
	if (lsr & 1<<1)
		uart_data->debug_rx_overrun |= 0x4000;

	return r;
}

static int uart_ioctl(struct device *dev, enum ioctl cmd, int arg)
{
	struct lpc_uart_proper_data *uart_data = dev->priv;

	switch (cmd) {
	case I_GETBAUD:
		return uart_data->baudrate;
	case I_SETBAUD:
		uart_disable(uart_data);
		uart_set_baudrate(uart_data, arg);
		uart_enable(uart_data);
		return 0;
	case I_GETPARITY:
		return uart_data->parity;
	case I_SETPARITY:
		uart_disable(uart_data);
		uart_set_parity(uart_data, arg);
		uart_enable(uart_data);
		return 0;
	default:
		return -EINVAL;
	}
	return 0;
}

struct driver serial_driver_proper = {
	.probe = uart_probe,
	.shutdown = uart_shutdown,
	.open = uart_open,
	.close = uart_close,
	.read = uart_read,
	.write = uart_write,
	.ioctl = uart_ioctl,
};

