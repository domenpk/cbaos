#ifndef _LPC_SSP_H_
#define _LPC_SSP_H_

/* currently this was tested to work on lpc2138 and lpc1343 */

/* this is only meant to be included from mach/board file */

#include <types.h>
#include <compiler.h>
#include <drivers/spi.h>

struct lpc_ssp {
	volatile u32 SSPCR0;
	volatile u32 SSPCR1;
	volatile u32 SSPDR;
	volatile u32 SSPSR;
	volatile u32 SSPCPSR;
	volatile u32 SSPIMSC;
	volatile u32 SSPRIS;
	volatile u32 SSPMIS;
	volatile u32 SSPICR;
};

struct lpc_ssp_data {
	int pclk;
	int irq;
};

int lpc_ssp_register(struct spi_master *master);

#ifdef MACH_LPC21XX
void __interrupt ssp0_handler();
#endif

#endif
