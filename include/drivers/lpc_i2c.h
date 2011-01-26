#ifndef _LPC_I2C_H_
#define _LPC_I2C_H_

/* currently this was tested to work on lpc2138 and lpc1343 */

/* this is only meant to be included from mach/board file */

#include <types.h>
#include <compiler.h>
#include <drivers/i2c.h>


struct lpc_i2c_regs {
	volatile u32 CONSET;
	volatile u32 STAT;
	volatile u32 DAT;
	volatile u32 ADR;
	volatile u32 SCLH;
	volatile u32 SCLL;
	volatile u32 CONCLR;
	/* on lpc13xx there are other registers, mostly slave related */
};

struct lpc_i2c {
	struct lpc_i2c_regs *regs;
	int pclk;
	int irq;
	int bus;
};


int lpc_i2c_register(struct i2c_master *master);

#ifdef MACH_LPC21XX
void __interrupt i2c0_handler();
void __interrupt i2c1_handler();
#endif

#endif
