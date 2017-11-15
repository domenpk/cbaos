/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <interrupt.h>
#include <arch/cm3_regs.h>


struct irq_entry {
	/* this will be changed to doubly linked list, and irq_request will also have to be fixed */
	irqreturn_t (*handler)(int irq, void *dev_id);
	void *dev_id;
};

int irq_request(unsigned int irq, irqreturn_t (*handler)(int irq, void *dev_id), unsigned long flags, void *dev_id);
void irq_free(unsigned int irq, void *dev_id);

void irq_enable(int irq)
{
	NVIC->ISER[irq>>5] = 1 << (irq&0x1f);
}

void irq_disable(int irq)
{
	NVIC->ICER[irq>>5] = 1 << (irq&0x1f);
}

void irq_ack(int irq)
{
}

// clear pending - this only clears the pending bit that you can set. no ack is needed
//	NVIC->ICPR[irq>>5] = 1 << (irq&0x1f);

#if 0
static irqreturn_t irq_default_handler(int irq, void *dev_id)
{
	printf("%s: unhandled irq %i, disabling\n", __func__, irq);
	irq_disable(irq);
	return IRQ_HANDLED;
}
#endif
