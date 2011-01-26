#ifndef _LPC_SPI_H_
#define _LPC_SPI_H_

/* NOTE currently this is only for lpc213x, might work on others too */
/* this is only meant to be included from mach/board file */

#include <types.h>
#include <compiler.h>
#include <drivers/spi.h>

struct lpc_spi {
	volatile u32 SPCR;
	volatile u32 SPSR;
	volatile u32 SPDR;
	volatile u32 SPCCR;
	u32 _dummy0[3];
	volatile u32 SPINT;
};

struct lpc_spi_data {
	int pclk;
};

int lpc_spi_register(struct spi_master *master);
void __interrupt spi0_handler();

#endif
