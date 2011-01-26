#ifndef _LPC_SERIAL_H_
#define _LPC_SERIAL_H_

#include <types.h>
#include <driver.h>


/* board support will need this to initialize the device */
extern struct driver serial_driver;

#define LPC_UART_FDR (1<<0)
struct lpc_uart_data {
	void *base;
	u32 flags;
	int pclk;
	int baudrate;
};


/* internal to uart driver */
struct lpc_uart {
	union {
		volatile u32 RBR;
		volatile u32 THR;
		volatile u32 DLL;
	};
	union {
		volatile u32 IER;
		volatile u32 DLM;
	};
	union {
		volatile u32 IIR;
		volatile u32 FCR;
	};
	volatile u32 LCR;
	volatile u32 MCR;       /* 0x10, for uarts with modem */
	volatile u32 LSR;
	volatile u32 MSR;       /* for uarts with modem */
	volatile u32 SCR;
	volatile u32 ACR;       /* 0x20 */
	volatile u32 _dummy2;
	volatile u32 FDR;       /* for uarts with fractional rate divider */
	volatile u32 _dummy3;
	volatile u32 TER;       /* 0x30 */
};

#endif
