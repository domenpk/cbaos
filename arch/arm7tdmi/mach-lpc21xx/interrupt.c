/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <interrupt.h>
#include <stdio.h>

// myeah, should be in arch for NVIC
#include <mach/lpc21xx_regs.h>

#warning "TODO"


struct irq_entry {
	/* this will be changed to doubly linked list, and irq_request will also have to be fixed */
	irqreturn_t (*handler)(int irq, void *dev_id);
	void *dev_id;
};

int irq_request(unsigned int irq, irqreturn_t (*handler)(int irq, void *dev_id), unsigned long flags, void *dev_id);
void irq_free(unsigned int irq, void *dev_id);

void irq_enable(int irq)
{
	VICIntSelect &= ~(1<<irq);
	VICIntEnable |= 1<<irq;
}

void irq_disable(int irq)
{
	VICIntEnClr |= 1<<irq;
}

void irq_ack(int irq)
{
	VICVectAddr = 0; /* clear interrupt pending (of current executing interrupt) */
}


// XXX install me
static irqreturn_t irq_default_handler(int irq, void *dev_id)
{
	printf("%s: unhandled irq %i, disabling\n", __func__, irq);
	irq_disable(irq);
	return IRQ_HANDLED;
}
