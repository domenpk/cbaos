/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <errno.h>
#include <types.h>
#include <sem.h>
#include <interrupt.h>
#include <compiler.h>
#include <drivers/i2c.h>
#include <drivers/stm32_i2c.h>
#include <mach/stm32_regs.h>


#define CR1_SWRST (1<<15)
#define CR1_ACK   (1<<10)
#define CR1_STOP  (1<<9)
#define CR1_START (1<<8)
#define CR1_PE    (1<<0)

#define CR2_ITEVTEN (1<<9)
#define CR2_ITERREN (1<<8)

#define CCR_FAST (1<<15)
#define CCR_DUTY (1<<14)


static void stm32_i2c_init(struct i2c_master *master)
{
	/* parent clock needs to be between 2 and 36 MHz */
	struct stm32_i2c_data *i2c_data = master->priv;
	struct stm32_i2c_regs *i2c = i2c_data->regs;
	int speed = master->speed;

	i2c->CR1 = CR1_PE | CR1_SWRST;
	i2c->CR1 = CR1_PE;
	i2c->CR2 = CR2_ITEVTEN | CR2_ITERREN | i2c_data->parent_clk/1000000;

	speed = i2c_data->parent_clk/speed/2;
	if (speed < 0)
		speed = 1;
	if (speed > 0xfff)
		speed = 0xfff;
	i2c->CCR = speed;

	i2c->CR1 = CR1_PE;
}


static void stm32_i2c_irqhandler(struct i2c_master *master)
{
	struct stm32_i2c_data *i2c_data = master->priv;
	struct stm32_i2c_regs *i2c = i2c_data->regs;
	u8 state = 0;
	u32 status;

	status = i2c->SR1;
//	printf("%s, status:%x\n", __func__, status);
	/* i2c event generated on: SB, ADDR, STOPF, BTF */
	if (status & 1<<0) { /* SB, start or repeated start sent */
		/* cleared by reading SR1, then writing DR */
		state = 0x08; /* or 0x10 */
	}
	else if (status & 1<<1) { /* ADDR, addr sent, ack received */
		/* cleared by reading, SR1, SR2 */
		if (i2c->SR2 & 1<<2)
			state = 0x18; /* sla+w sent, ack received */
		else
			state = 0x40; /* sla+r sent, ack received */
	}
	else if (status & 1<<2) { /* BTF, byte sent, ack received */
		/* cleared by reading SR1 + r/w DR or when start/stop */
		if (status & 1<<6) /* RxNE, got data */
			state = 0x50; /* or 0x58 */
		else if (status & 1<<7) /* TxE, waiting for data tx */
			state = 0x28;
	}
	else if (status & 1<<9) { /* ARLO, arbitration lost */
		/* cleared by writing 0 or periph disable */
		i2c->SR1 = status & ~(1<<9);
		state = 0x38;
	}
	else if (status & 1<<10) { /* AF, no ack received */
		/* cleared by writing 0 or periph disable */
		i2c->SR1 = status & ~(1<<10);
		state = 0x20; /* or 0x30, or 0x48 */
	}

	i2c_state_machine(master, state);

	/* TODO check SR1/2 values, and error out if not cleared */
}

/* TODO find some nicer way to do this */
#ifdef MACH_STM32
static struct i2c_irq_data {
	struct i2c_master *master;
} i2c_irq_data[2];

void __interrupt i2c1_ev_irqhandler()
{
	irq_ack(IRQ_I2C1_EV);
	stm32_i2c_irqhandler(i2c_irq_data[0].master);
}
void __interrupt i2c1_er_irqhandler()
{
	irq_ack(IRQ_I2C1_ER);
	stm32_i2c_irqhandler(i2c_irq_data[0].master);
}

void __interrupt i2c2_ev_irqhandler()
{
	irq_ack(IRQ_I2C2_EV);
	stm32_i2c_irqhandler(i2c_irq_data[1].master);
}
void __interrupt i2c2_er_irqhandler()
{
	irq_ack(IRQ_I2C2_ER);
	stm32_i2c_irqhandler(i2c_irq_data[1].master);
}
#endif

static void stm32_i2c_start(struct i2c_master *master)
{
	struct stm32_i2c_data *i2c_data = master->priv;
	struct stm32_i2c_regs *i2c = i2c_data->regs;
	i2c->CR1 |= CR1_START; /* auto cleared */
}

static void stm32_i2c_restart(struct i2c_master *master)
{
	struct stm32_i2c_data *i2c_data = master->priv;
	struct stm32_i2c_regs *i2c = i2c_data->regs;
	i2c->CR1 |= CR1_START; /* auto cleared */
}

static void stm32_i2c_stop(struct i2c_master *master)
{
	struct stm32_i2c_data *i2c_data = master->priv;
	struct stm32_i2c_regs *i2c = i2c_data->regs;
	u32 tmp;

	tmp = i2c->CR1;
	tmp &= ~CR1_ACK;
	tmp |= CR1_STOP; /* auto cleared */
	i2c->CR1 = tmp;

	/* it's possible that slave sent a byte before getting stop (ie. S ADDR|R [A] [DATA]/P), so eat it */
	tmp = i2c->SR1;
	tmp = i2c->DR;
}

static void stm32_i2c_ack(struct i2c_master *master, int ack)
{
	struct stm32_i2c_data *i2c_data = master->priv;
	struct stm32_i2c_regs *i2c = i2c_data->regs;
	if (ack)
		i2c->CR1 |= CR1_ACK; /* set ack bit for next received byte */
	else
		i2c->CR1 &= ~CR1_ACK; /* clear ack bit for next received byte */
}

static void stm32_i2c_write(struct i2c_master *master, u8 data)
{
	struct stm32_i2c_data *i2c_data = master->priv;
	struct stm32_i2c_regs *i2c = i2c_data->regs;
	i2c->DR = data;
}

static u8 stm32_i2c_read(struct i2c_master *master)
{
	struct stm32_i2c_data *i2c_data = master->priv;
	struct stm32_i2c_regs *i2c = i2c_data->regs;
	return i2c->DR;
}


int stm32_i2c_register(struct i2c_master *master)
{
	struct stm32_i2c_data *i2c_data = master->priv;
	struct stm32_i2c_regs *i2c = i2c_data->regs;

	i2c->CR1 = 0;

	i2c_irq_data[i2c_data->bus].master = master;
	irq_enable(i2c_data->irq_ev);
	irq_enable(i2c_data->irq_er);

	master->init = stm32_i2c_init;
	master->start = stm32_i2c_start;
	master->restart = stm32_i2c_restart;
	master->stop = stm32_i2c_stop;
	master->ack = stm32_i2c_ack;
	master->read = stm32_i2c_read;
	master->write = stm32_i2c_write;

	return 0;
}
