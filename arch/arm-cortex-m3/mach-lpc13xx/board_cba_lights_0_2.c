/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <board.h>
#include <mach/lpc13xx_regs.h>
#include <mach/lpc13xx_interrupt.h>
#include <gpio.h>
#include <mach/lpc13xx_gpio.h>
#include <interrupt.h>
#include <drivers/lpc_serial.h>
#include <drivers/lpc_serial_proper.h>
#include <drivers/lpc_ssp.h>
#include <drivers/lpc_i2c.h>
#include <device.h>

#ifdef CONFIG_USBDONGLE
#define LPC1343_SERIAL /* fancy serial, disables the console */
#else
#define LPC1343_SSP
#define LPC1343_I2C
#endif


void tty0_init()
{
#ifdef LPC1343_SERIAL
#ifdef CONFIG_USBDONGLE_PCB1 /* SPI pins are used on PCB1 */
	/* printf over SPI */
	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_IOCON;
	LPC_IOCON->PIO0_8 = 1; /* MISO */
	LPC_IOCON->PIO0_9 = 1; /* MOSI */
	LPC_IOCON->SCKLOC = 0; /* SCK on P0_10 */
	LPC_IOCON->JTAG_TCK_PIO0_10 = 2; /* SCK */
	LPC_IOCON->PIO0_2 = 1; /* SSEL */

	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_SSP;
	LPC_SYSCON->SSPCLKDIV = 1;

	LPC_SYSCON->PRESETCTRL |= 1<<0;

	extern struct driver serial_spi_driver;
	static struct device tty0;
	tty0.name = "tty0";
	tty0.drv = &serial_spi_driver;
	if (tty0.drv->probe(&tty0, NULL) == 0)
		device_register(&tty0);
#endif
#else
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
#endif
}

#ifdef LPC1343_SERIAL
static void serial_init()
{
	// if this is enabed the tty0 above needs to be disabled
	static struct device serial;
	static struct lpc_uart_proper_data uart_data;
	static u8 buf_rx_data[1024];
	static u8 buf_tx_data[64+1];

	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_IOCON;
	LPC_IOCON->PIO1_6 = 1; /* uart RXD */
	LPC_IOCON->PIO1_7 = 1; /* uart TXD */

	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_UART;
	LPC_SYSCON->UARTCLKDIV = 1; /* main clock / 1 */

	uart_data.base = (void*)LPC_UART_BASE;
	uart_data.flags = LPC_UART_FDR;
	uart_data.pclk = CONFIG_FCPU;
	uart_data.baudrate = 115200;
	uart_data.irq = IRQ_UART;
	circ_buf_init(&uart_data.rx_buf, buf_rx_data, sizeof(buf_rx_data));
	circ_buf_init(&uart_data.tx_buf, buf_tx_data, sizeof(buf_tx_data));

	serial.name = "serial";
	serial.drv = &serial_driver_proper;
	if (serial.drv->probe(&serial, &uart_data) == 0)
		device_register(&serial);
}
#else
static void serial_init() { }
#endif

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
	gpio_init(GPIO_0_8_MISO, 0, 0);
	gpio_init(GPIO_0_9_MOSI, 0, 0);
//	LPC_IOCON->PIO0_8 = 1; /* MISO */
//	LPC_IOCON->PIO0_9 = 1; /* MOSI */
	LPC_IOCON->SCKLOC = 0; /* SCK on P0_10 */
	//gpio_init(GPIO_0_10_SCK, 0, 0);
	LPC_IOCON->SCKLOC = 2; /* SCK on P0_6 */
	gpio_init(GPIO_0_6_SCK, 0, 0);
//	LPC_IOCON->JTAG_TCK_PIO0_10 = 2; /* SCK */

	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_SSP;
	LPC_SYSCON->SSPCLKDIV = 1;

	LPC_SYSCON->PRESETCTRL |= 1<<0;

	lpc_ssp_data.pclk = CONFIG_FCPU;
	lpc_ssp_data.irq = IRQ_SSP;
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
	lpc_i2c_data0.irq = IRQ_I2C;
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
#elif CONFIG_FCPU == 12000000
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

#if CONFIG_FCPU != 12000000
	/* select PLL output for main clock */
	LPC_SYSCON->MAINCLKSEL = MAINCLKSEL_PLL;
	LPC_SYSCON->MAINCLKUEN = 0;
	LPC_SYSCON->MAINCLKUEN = 1;
#endif

	serial_init();
	board_ssp_init();
	board_i2c0_init();

	/* set-up interrupt on P2_9 transitioning to low - cbashell entry */
#if 0
	// XXX this doesn't work btw.
	gpio_init(GPIO_2_9, GPIO_INPUT, 0);
	LPC_IOCON->PIO2_9 = 0x50; /* pull-up */
	LPC_GPIO2->IEV |= 0<<9; /* falling edge */
	LPC_GPIO2->IE |= 1<<9; /* enable interrupt */
	irq_enable(IRQ_WAKEUP_2_9);
#endif
}
