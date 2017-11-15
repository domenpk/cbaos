#ifndef MACH_CRT_H_
#define MACH_CRT_H_

void nmi_handler(void);
void hardfault_handler(void);
void svc_handler(void);
void pendsv_handler(void);
void systick_handler(void);

void wakeup_irqhandler(void);
void c_can_irqhandler(void);
void ssp1_irqhandler(void);
void i2c_irqhandler(void);
void timer16_0_irqhandler(void);
void timer16_1_irqhandler(void);
void timer32_0_irqhandler(void);
void timer32_1_irqhandler(void);
void ssp0_irqhandler(void);
void uart_irqhandler(void);

void adc_irqhandler(void);
void wdt_irqhandler(void);
void bod_irqhandler(void);

void pioint3_irqhandler(void);
void pioint2_irqhandler(void);
void pioint1_irqhandler(void);
void pioint0_irqhandler(void);

#endif
