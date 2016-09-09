#ifndef _STM32_I2C_H_
#define _STM32_I2C_H_

#include <types.h>
#include <drivers/i2c.h>


struct stm32_i2c_data {
	void *regs;
	int parent_clk;
	int irq_ev;
	int irq_er;
	int bus;
};

int stm32_i2c_register(struct i2c_master *master);

#endif
