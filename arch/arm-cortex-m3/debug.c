/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <types.h>
#include <stdio.h>
#include <compiler.h>
//#include <arch/cm3_regs.h>
#include <arch/crt.h>
#include <cbashell.h>


static const char * const exceptions[] = {
	"Unknown",
	"Reset",
	"NMI",
	"Hard Fault",
	"Memory Management",
	"Bus Fault",
	"Usage Fault",
};

static const char * const regnames[16] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
		"r8", "r9", "r10/sl", "r11/fp", "r12/ip", "sp", "lr", "pc" };

/* my precious debugging function
 * HardFault is triggered on ie. invalid memory access.
 * note: on IMPRECISERR, this doesn't help much. it just means that some
 * invalid access occured but possibly before (after/prediction?) PC
 */
#ifdef MACH_LPC13XX
#include <device.h>
#include <mach/lpc13xx_regs.h>
#include <drivers/lpc_serial_proper.h>
#endif
#define HFSR    *((volatile u32 *)0xe000ed2c)
#define MMAR    *((volatile u32 *)0xe000ed34)
#define BFAR    *((volatile u32 *)0xe000ed38)
void generic_exception_handler_c(u32 *oldstack, u32 *newstack, unsigned exception)
{
	int oldstack_valid = (oldstack >= (u32*)&_ram_start && oldstack < (u32*)&_ram_end);
	int i;

#ifdef MACH_LPC13XX
	asm volatile ("cpsid i\n");
	static struct device tty0;
	static struct lpc_uart_data uart_data;	

	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_IOCON;
	LPC_IOCON->PIO1_6 = 1; /* uart RXD */
	LPC_IOCON->PIO1_7 = 1; /* uart TXD */

	LPC_SYSCON->SYSAHBCLKCTRL &= ~SYSAHBCLKCTRL_UART;
	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_UART;
	LPC_SYSCON->UARTCLKDIV = 1; /* main clock / 1 */

	uart_data.base = (void*)LPC_UART_BASE;
	uart_data.flags = LPC_UART_FDR;
	uart_data.pclk = CONFIG_FCPU;
	uart_data.baudrate = 115200;

	tty0.name = "tty0";
	tty0.drv = &serial_driver;
	if (tty0.drv->probe(&tty0, &uart_data) == 0)
		device_register(&tty0);


	static FILE _stdout;
	int r = fopen(&_stdout, "/dev/tty0", O_NONBLOCK);
	if (r != 0) {
		/* no console */
		fopen(&_stdout, "/dev/null", O_NONBLOCK);
	}
	stdin = &_stdout;
	stdout = &_stdout;
	stderr = &_stdout;
#endif

	if (exception < ALEN(exceptions))
		printf("%s was called, dumping regs:\n", exceptions[exception]);
	else
		printf("Exception #%i was caled, dumping regs:\n", exception);

	printf("new_lr\t0x%08x\n", newstack[8]);
	if (oldstack_valid) {
		for (i=0; i<4; i++)
			printf("%s\t0x%08x\t%i\n", regnames[i], oldstack[i], oldstack[i]);
	} else {
		printf("sp\t%p (newstack:%p)\n", oldstack, newstack);
		printf("sp is invalid, so some registers are missing\n");
	}
	for (i=0; i<8; i++)
		printf("%s\t0x%08x\t%i\n", regnames[i+4], newstack[i], newstack[i]);

	if (oldstack_valid) {
		printf("r12/ip\t0x%08x\t%i\n", oldstack[4], oldstack[4]);
		printf("sp\t%p (newstack:%p)\n", oldstack, newstack);
		printf("lr\t0x%08x\n", oldstack[5]);
		printf("pc\t0x%08x\n", oldstack[6]);
		printf("xPSR\t0x%08x\n", oldstack[7]);
	}

	printf("HFSR\t0x%08x\n", HFSR);
	if (MMAR != 0xe000ed34)
		printf("MMAR\t0x%08x\n", MMAR);
	if (BFAR != 0xe000ed38)
		printf("BFAR\t0x%08x\n", BFAR);

	task_printall();

#if 0
	printf("\nentering cbashell\n");
	cbashell_init();
	while (1) {
		int c;
		c = getchar();
		if (c >= 0)
			cbashell_charraw(c);
	}
#endif
}
