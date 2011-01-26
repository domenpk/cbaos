#ifndef _STM32_SERIAL_H_
#define _STM32_SERIAL_H_

#include <types.h>
#include <driver.h>


/* board support will need this to initialize the device */
extern struct driver serial_driver;

struct stm32_uart_data {
	void *base;
	int parent_clk;
	int baudrate;
};


/* internal to uart driver */
struct stm32_uart {
	volatile u32 SR;
	volatile u32 DR;
	volatile u32 BRR;
	volatile u32 CR1;
	volatile u32 CR2;
	volatile u32 CR3;
	volatile u32 GTPR;
};

#endif
