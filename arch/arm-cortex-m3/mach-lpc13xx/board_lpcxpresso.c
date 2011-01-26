/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <board.h>
#include <mach/lpc13xx_regs.h>
#include <drivers/lpc_serial.h>
#include <drivers/lpc_ssp.h>
#include <drivers/lpc_i2c.h>
#include <device.h>

#define LPC1343_SSP
#define LPC1343_I2C


// XXX -> some .h
#define SYSPLLCLKSEL_IRC 0
#define SYSPLLCLKSEL_SYSTEM 1
#define SYSPLLCLKSEL_WDT 2

#define MAINCLKSEL_IRC 0
#define MAINCLKSEL_PLLIN 1
#define MAINCLKSEL_WDT 2
#define MAINCLKSEL_PLL 3

#define PDRUNCFG_IRCOUT_PD (1<<0)
#define PDRUNCFG_IRC_PD (1<<1)
#define PDRUNCFG_SYSPLL_PD (1<<7)

#define FLASHCFG *((volatile unsigned int *)0x4003c010)


void tty0_init()
{
	static struct device tty0;
	static struct lpc_uart_data uart_data;	

	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_IOCON;
	LPC_IOCON->PIO1_6 = 1; /* uart RXD */
	LPC_IOCON->PIO1_7 = 1; /* uart TXD */

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
}

void udelay(int x)
{
	x = x*(CONFIG_FCPU/1000000)/4;
	asm volatile(
		"mov	%0, %1\n\t"
		".udelay_%=:\n\t"
		"subs	%0, %0, #1\n\t"		/* 1 cycle */
		"bne	.udelay_%=\n\t"	/* 3 cycles */
		: "=r" (x) : "r" (x) : "cc" );
}

#ifdef LPC1343_SSP
static struct lpc_ssp_data lpc_ssp_data;

struct spi_master spi0 = {
	.spi = (void*)LPC_SSP_BASE,
	.spi_data = &lpc_ssp_data,
	/* last_dev implicitly initialized */
};

static int board_ssp_init()
{

	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_IOCON;
	LPC_IOCON->PIO0_8 = 1; /* MISO */
	LPC_IOCON->PIO0_9 = 1; /* MOSI */
	LPC_IOCON->SCKLOC = 0; /* SCK on P0_10 */
	LPC_IOCON->JTAG_TCK_PIO0_10 = 2; /* SCK */

	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_SSP;
	LPC_SYSCON->SSPCLKDIV = 1;

	LPC_SYSCON->PRESETCTRL |= 1<<0;

	lpc_ssp_data.pclk = CONFIG_FCPU;
	lpc_ssp_data.irq = SSP_IRQn;
	lpc_ssp_register(&spi0);
	spi_register_master(&spi0);

	return 0;
}
#else
static int board_ssp_init() { return 0; }
#endif

#ifdef LPC1343_I2C
static struct lpc_i2c lpc_i2c_data0;

struct i2c_master i2c0 = {
	.priv = &lpc_i2c_data0,
	.speed = 400000,
};

static int board_i2c0_init()
{
	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_IOCON;
	LPC_IOCON->PIO0_4 = 1; /* SCL */
	LPC_IOCON->PIO0_5 = 1; /* SDA */

	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_I2C;

	LPC_SYSCON->PRESETCTRL |= 1<<1;

	lpc_i2c_data0.regs = (void*)LPC_I2C_BASE;
	lpc_i2c_data0.pclk = CONFIG_FCPU; /* system clock, which could be different from main clock */
	lpc_i2c_data0.irq = I2C_IRQn;
	lpc_i2c_data0.bus = 0;
	lpc_i2c_register(&i2c0);
	i2c_register_master(&i2c0);

	return 0;
}
#else
static int board_i2c0_init() { return 0; }
#endif

void board_init()
{
	/* power up IRC */
	LPC_SYSCON->PDRUNCFG &= ~(PDRUNCFG_IRCOUT_PD | PDRUNCFG_IRC_PD);

	/* select IRC for main clock */
	LPC_SYSCON->MAINCLKSEL = MAINCLKSEL_IRC;
	LPC_SYSCON->MAINCLKUEN = 0;
	LPC_SYSCON->MAINCLKUEN = 1;

	/* IRC clock for PLL */
	LPC_SYSCON->SYSPLLCLKSEL = SYSPLLCLKSEL_IRC;
	LPC_SYSCON->SYSPLLCLKUEN = 0;
	LPC_SYSCON->SYSPLLCLKUEN = 1;

	/* for mode 1: Fclkout = M * Fclkin = Fcco/(2*P)
	 * Fclkin (IRC) = 12MHz, Fcco e [156, 320] */
#if CONFIG_FCPU == 72000000
	LPC_SYSCON->SYSPLLCTRL = 1<<5 /* P=2 */ | 5<<0 /* M=6 */; /* set clock to 72 MHz */
#elif CONFIG_FCPU == 48000000
	LPC_SYSCON->SYSPLLCTRL = 1<<5 /* P=2 */ | 3<<0 /* M=4 */; /* set clock to 48 MHz */
#else
#error "invalid CONFIG_FCPU"
#endif

#if CONFIG_FCPU > 40000000
	FLASHCFG = (FLASHCFG & ~3) | 2; /* 3 system clocks */
#elif CONFIG_FCPU > 20000000
	FLASHCFG = (FLASHCFG & ~3) | 1; /* 2 system clocks */
#else
	FLASHCFG = (FLASHCFG & ~3) | 0; /* 1 system clocks */
#endif

	/* power up SYSPLL */
	LPC_SYSCON->PDRUNCFG &= ~PDRUNCFG_SYSPLL_PD;
	/* wait for PLL lock */
	while ((LPC_SYSCON->SYSPLLSTAT & 1) == 0)
		;

	/* select PLL output for main clock */
	LPC_SYSCON->MAINCLKSEL = MAINCLKSEL_PLL;
	LPC_SYSCON->MAINCLKUEN = 0;
	LPC_SYSCON->MAINCLKUEN = 1;

	board_ssp_init();
	board_i2c0_init();
}
