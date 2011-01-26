#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

/* irq flags */
// not yet implemented!
#define IRQF_SHARED      (1<<0)

typedef enum irqreturn {
	IRQ_NONE,
	IRQ_HANDLED,
} irqreturn_t;


int irq_request(unsigned int irq, irqreturn_t (*handler)(int irq, void *dev_id), unsigned long flags, void *dev_id);
void irq_free(unsigned int irq, void *dev_id);

void irq_enable(int irq);
void irq_disable(int irq);
void irq_ack(int irq);

#endif
