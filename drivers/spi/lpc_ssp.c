/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <interrupt.h>
#include <types.h>
#include <gpio.h>
#include <compiler.h>
#include <drivers/spi.h>
#include <drivers/lpc_ssp.h>


static int lpc_ssp_change_device(struct spi_device *device)
{
	struct lpc_ssp *spi = device->master->spi;
	struct lpc_ssp_data *data = device->master->spi_data;
	int tmp;

	spi->SSPCR1 = 0; /* disable */

	tmp = 7; /* 8 bit transfer */
	if (device->mode & SPI_CPHA)
		tmp |= 1<<7;
	if (device->mode & SPI_CPOL)
		tmp |= 1<<6;
	// scr remains 0, ok?
	spi->SSPCR0 = tmp;

	tmp = (data->pclk + device->clock-1) / device->clock;
	tmp &= ~1;
	if (tmp < 2)
		tmp = 2;
	if (tmp > 255)
		tmp = 254;
	/* this limits spi clock to pclk/2 */
	spi->SSPCPSR = tmp;

	//spi->SPINT = 1; /* clear old interrupt */
	irq_enable(data->irq);

	gpio_init(device->cs_pin, GPIO_OUTPUT, 1);

	return 0;
}

static struct ssp0_irq_data {
	struct spi_device *device;
	struct spi_transfer *transfer;
	int txpos, rxpos;
} ssp0_irq_data;

#ifdef MACH_LPC21XX
void __interrupt ssp0_handler()
#endif
#ifdef MACH_LPC13XX
void __interrupt ssp_irqhandler()
#endif
{
	struct lpc_ssp *spi = ssp0_irq_data.device->master->spi;
	struct lpc_ssp_data *data = ssp0_irq_data.device->master->spi_data;
	struct spi_transfer *transfer = ssp0_irq_data.transfer;
	int txpos = ssp0_irq_data.txpos;
	int rxpos = ssp0_irq_data.rxpos;
	int len = transfer->len;

	irq_ack(data->irq);

	//printf("%s, ris:%x, sr:%x\n", __func__, spi->SSPRIS, spi->SSPSR);
	/* there's space in tx fifo, so send */
	while (txpos < len && (spi->SSPSR & 1<<1 /* TNF */)) {
		//printf("spi, tx[%i]\n", txpos);
		spi->SSPDR = transfer->tx_buf[txpos++];
	}
	ssp0_irq_data.txpos = txpos;

	if (txpos == len)
		spi->SSPIMSC &= ~(1<<3); /* disable TX half-empty interrupt */

	/* there's data in rx fifo, so receive */
	while (rxpos < len && (spi->SSPSR & 1<<2 /* RNE */)) {
		//printf("spi, rx[%i]\n", rxpos);
		transfer->rx_buf[rxpos++] = spi->SSPDR;
	}
	ssp0_irq_data.rxpos = rxpos;

	if (rxpos == len) {
	//if (spi->SSPRIS & (1<<1) /* rx timeout */) {
		gpio_set(ssp0_irq_data.device->cs_pin, 1);
	//	if (rxpos < len)
	//		transfer->error = -ETIMEDOUT;
		spi->SSPICR = 1<<1; /* clear RTI */
		spi->SSPCR1 = 0; /* disable */
		spi_transfer_finished(ssp0_irq_data.device, transfer);
	}
	if (spi->SSPRIS & (1<<1) /* rx timeout */)
		spi->SSPICR = 1<<1; /* clear RTI */
}

static int lpc_ssp_transfer(struct spi_device *device, struct spi_transfer *transfer)
{
	struct lpc_ssp *spi = device->master->spi;

	ssp0_irq_data.txpos = 0;
	ssp0_irq_data.rxpos = 0;
	ssp0_irq_data.device = device;
	ssp0_irq_data.transfer = transfer;

	gpio_set(device->cs_pin, 0);

	/* should get us into TX irq immediately */
	spi->SSPIMSC = 1<<1 | 1<<2 | 1<<3; /* RTIM, RXIM and TXIM */
	spi->SSPCR1 = 1<<1; /* enable */

	/* after this, the irq routine picks up */

	return 0;
}


int lpc_ssp_register(struct spi_master *master)
{
	/* some init goes here... which isn't needed in this case */

	master->change_device = lpc_ssp_change_device;
	master->transfer = lpc_ssp_transfer;
	return 0;
}
