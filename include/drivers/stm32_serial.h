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

#endif
