/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <interrupt.h>
#include <types.h>
#include <gpio.h>
#include <compiler.h>
#include <mach/lpc21xx_gpio.h>
#include <mach/lpc21xx_regs.h>
#include <drivers/spi.h>
#include <drivers/lpc_spi.h>



static int lpc_spi_change_device(struct spi_device *device)
{
	struct lpc_spi *spi = device->master->spi;
	struct lpc_spi_data *data = device->master->spi_data;
	int tmp;
	int spi_cr = 0;

	if (device->mode & SPI_CPHA)
		spi_cr |= 1<<3;
	if (device->mode & SPI_CPOL)
		spi_cr |= 1<<4;
	spi_cr |= 1<<5; /* MSTR master mode */
	if (device->mode & SPI_LSB)
		spi_cr |= 1<<6; /* LSBF LSB first */

	spi_cr |= 1<<7; /* SPIE interrupt enable */
	spi->SPINT = 1; /* clear old interrupt */
	irq_enable(IRQ_SPI0);

	spi->SPCR = spi_cr;
	tmp = (data->pclk + device->clock-1) / device->clock;
	tmp &= ~1;
	if (tmp < 8)
		tmp = 8;
	if (tmp > 255)
		tmp = 254;
	/* this limits spi clock to pclk/8 */
	spi->SPCCR = tmp;

	gpio_init(device->cs_pin, GPIO_OUTPUT, 1);

	return 0;
}

static void lpc_spi_tx(struct lpc_spi *spi, u8 data)
{
	spi->SPDR = data;
}

static struct spi0_irq_data {
	struct spi_device *device;
	struct spi_transfer *transfer;
	int pos;
} spi0_irq_data;

void __interrupt spi0_handler()
{
	struct lpc_spi *spi = spi0_irq_data.device->master->spi;
	struct spi_transfer *transfer = spi0_irq_data.transfer;
	int pos = spi0_irq_data.pos;

	spi->SPINT = 1; /* clear spi interrupt */
	VICVectAddr = 0; /* clear pending interrupt */

	if ((spi->SPSR & 1<<7) == 0) {
		printf("%s: should error out with spi_transfer_finished\n", __func__);
		transfer->error = -EINVAL;
		spi_transfer_finished(spi0_irq_data.device, transfer);
		return;
	}
	transfer->rx_buf[pos++] = spi->SPDR;

	if (pos >= transfer->len) {
		gpio_set(spi0_irq_data.device->cs_pin, 1);
		spi_transfer_finished(spi0_irq_data.device, transfer);
	} else {
		lpc_spi_tx(spi, transfer->tx_buf[pos]);
	}

	spi0_irq_data.pos = pos;
}

static int lpc_spi_transfer(struct spi_device *device, struct spi_transfer *transfer)
{
	struct lpc_spi *spi = device->master->spi;

	spi0_irq_data.pos = 0;
	spi0_irq_data.device = device;
	spi0_irq_data.transfer = transfer;

	gpio_set(device->cs_pin, 0);
	lpc_spi_tx(spi, transfer->tx_buf[0]);

	/* after this, the irq routine picks up */

	return 0;
}


int lpc_spi_register(struct spi_master *master)
{
	/* some init goes here... which isn't needed in this case */

	master->change_device = lpc_spi_change_device;
	master->transfer = lpc_spi_transfer;
	return 0;
}
