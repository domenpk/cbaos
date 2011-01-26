/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <errno.h>
#include <types.h>
#include <sem.h>
#include <interrupt.h>
#include <compiler.h>
#include <drivers/i2c.h>
#include <drivers/lpc_i2c.h>



#define CON_AA   (1<<2)
#define CON_SI   (1<<3)
#define CON_STO  (1<<4)
#define CON_STA  (1<<5)
#define CON_I2EN (1<<6)


static void lpc_i2c_init(struct i2c_master *master)
{
	struct lpc_i2c *i2c_data = master->priv;
	struct lpc_i2c_regs *i2c = i2c_data->regs;
	int speed = master->speed;

	i2c->CONCLR = CON_AA | CON_SI | CON_STA | CON_I2EN;
	speed = i2c_data->pclk/speed/2;
	if (speed < 4)
		speed = 4;
	if (speed > 0xffff)
		speed = 0xffff;

	i2c->SCLH = speed;
	i2c->SCLL = speed;
	i2c->CONSET = CON_I2EN; /* slave disabled */
}


static void lpc_i2c_handler(struct i2c_master *master)
{
	struct lpc_i2c *i2c_data = master->priv;
	struct lpc_i2c_regs *i2c = i2c_data->regs;

	irq_ack(i2c_data->irq);
	i2c_state_machine(master, i2c->STAT);
	i2c->CONCLR = CON_SI;
}

/* TODO find some nicer way to do this */
#ifdef MACH_LPC21XX
static struct i2c_irq_data {
	struct i2c_master *master;
} i2c_irq_data[2];

void __interrupt i2c0_handler()
{
	lpc_i2c_handler(i2c_irq_data[0].master);
}

void __interrupt i2c1_handler()
{
	lpc_i2c_handler(i2c_irq_data[1].master);
}
#endif
#ifdef MACH_LPC13XX
static struct i2c_irq_data {
	struct i2c_master *master;
} i2c_irq_data[2];

void __interrupt i2c_irqhandler()
{
	lpc_i2c_handler(i2c_irq_data[0].master);
}
#endif

static void lpc_i2c_start(struct i2c_master *master)
{
	struct lpc_i2c *i2c_data = master->priv;
	struct lpc_i2c_regs *i2c = i2c_data->regs;
	i2c->CONSET = CON_STA;
}

static void lpc_i2c_restart(struct i2c_master *master)
{
	struct lpc_i2c *i2c_data = master->priv;
	struct lpc_i2c_regs *i2c = i2c_data->regs;
	i2c->CONSET = CON_STA; /* user must clear it */
}

static void lpc_i2c_stop(struct i2c_master *master)
{
	struct lpc_i2c *i2c_data = master->priv;
	struct lpc_i2c_regs *i2c = i2c_data->regs;
	i2c->CONCLR = CON_AA; /* clear ack bit / slave enabled bit */
	i2c->CONSET = CON_STO; /* auto cleared */
}

static void lpc_i2c_ack(struct i2c_master *master, int ack)
{
	struct lpc_i2c *i2c_data = master->priv;
	struct lpc_i2c_regs *i2c = i2c_data->regs;
	if (ack)
		i2c->CONSET = CON_AA; /* set ack bit for next received byte */
	else
		i2c->CONCLR = CON_AA; /* clear ack bit for next received byte */
}

static void lpc_i2c_write(struct i2c_master *master, u8 data)
{
	struct lpc_i2c *i2c_data = master->priv;
	struct lpc_i2c_regs *i2c = i2c_data->regs;
	i2c->CONCLR = CON_STA; /* clear start bit */
	i2c->DAT = data;
}

static u8 lpc_i2c_read(struct i2c_master *master)
{
	struct lpc_i2c *i2c_data = master->priv;
	struct lpc_i2c_regs *i2c = i2c_data->regs;
	i2c->CONCLR = CON_STA; /* clear start bit */
	return i2c->DAT;
}


int lpc_i2c_register(struct i2c_master *master)
{
	struct lpc_i2c *i2c_data = master->priv;
	struct lpc_i2c_regs *i2c = i2c_data->regs;

	i2c->CONCLR = CON_AA | CON_SI | CON_STA | CON_I2EN;

	i2c_irq_data[i2c_data->bus].master = master;
	irq_enable(i2c_data->irq);

	master->init = lpc_i2c_init;
	master->start = lpc_i2c_start;
	master->restart = lpc_i2c_restart;
	master->stop = lpc_i2c_stop;
	master->ack = lpc_i2c_ack;
	master->read = lpc_i2c_read;
	master->write = lpc_i2c_write;

	return 0;
}
