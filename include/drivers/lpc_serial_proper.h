#ifndef _LPC_SERIAL_PROPER_H_
#define _LPC_SERIAL_PROPER_H_

#include <types.h>
#include <driver.h>
#include <sem.h>
#include <circ_buf.h>
#include <drivers/lpc_serial.h>


/* board support will need this to initialize the device */
extern struct driver serial_driver_proper;

struct lpc_uart_proper_data {
	void *base;
	u32 flags;
	int pclk;
	/* some of these could be in generic serial driver? */
	int baudrate;
	int parity; /* 0 - none, 1 - odd, 2 - even */
	int irq;
	struct circ_buf rx_buf;
	struct circ_buf tx_buf;
	struct mutex rx_mutex;
	struct mutex tx_mutex;

	volatile int debug_thrNe;
	volatile int debug_rx_overrun;
	volatile int debug_rx_bytes;
};


#endif
