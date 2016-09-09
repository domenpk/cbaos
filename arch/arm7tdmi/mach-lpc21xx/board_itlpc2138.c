/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <board.h>
#include <mach/board_itlpc2138.h>
#include <mach/lpc21xx_regs.h>
#include <drivers/lpc_serial.h>
#include <drivers/lpc_spi.h>
#include <drivers/lpc_ssp.h>
#include <drivers/lpc_i2c.h>

#include <device.h>
#include <interrupt.h>

//#define LPC2138_SSP
#define LPC2138_I2C0
#define LPC2138_I2C1

#define LPC21XX_UART0 0xe000c000
#define LPC21XX_UART1 0xe0010000

#define LPC21XX_SPI0  0xe0020000
#define LPC21XX_SPI1  0xe0068000

#define LPC21XX_I2C0  0xe001c000
#define LPC21XX_I2C1  0xe005c000


static u32 clock_hz = 12*1000*1000;
static u32 clock_pclk;

static int setup_pll(int m, int p)
{
	int tries;

	/* disable MAM */
	MAMTIM = 7;
	MAMCR = 0;

	/* 1 PLL disconnect and disable */
	PLLCON = 0;
	PLLFEED = 0xaa; PLLFEED = 0x55;

	/* 2 PLL setup */
	PLLCFG = (p<<5) | ((m-1)&0x1f);
	PLLFEED = 0xaa; PLLFEED = 0x55;

	/* 3 PLL enable */
	PLLCON |= 0x1;
	PLLFEED = 0xaa; PLLFEED = 0x55;

	/* 4 wait for PLL lock */
	tries = 100000; /* XXX, magic value */
	while (tries--)
		if (PLLSTAT & (1<<10))
			break;
	if (tries == 0)
		return -1;

	/* 5 PLL connect */
	PLLCON |= 0x2;
	PLLFEED = 0xaa; PLLFEED = 0x55;

	clock_hz *= 5;

	/* enable MAM to get decent performance */
	if (clock_hz >= 40*1000*1000)
		MAMTIM = 3;
	else if (clock_hz >= 20*1000*1000)
		MAMTIM = 2;
	else
		MAMTIM = 1;
	MAMCR = 2;

	return 0;
}


void tty0_init()
{
	static struct device tty0;
	static struct lpc_uart_data uart_data;
	u32 tmp;

#if 0
	PCONP |= PCONP_UART1;
	tmp = PINSEL0;
	tmp &= ~(3<<8*2);
	tmp &= ~(3<<9*2);
	tmp |= 1<<8*2; /* P0.8 is now TXD1 */
	tmp |= 1<<9*2; /* P0.9 is now RXD1 */
	PINSEL0 = tmp;

	uart_data.base = (void*)LPC21XX_UART1;
#else
	PCONP |= PCONP_UART0;
	tmp = PINSEL0;
	tmp &= ~(3<<0*2);
	tmp &= ~(3<<1*2);
	tmp |= 1<<0*2; /* P0.8 is now TXD1 */
	tmp |= 1<<1*2; /* P0.9 is now RXD1 */
	PINSEL0 = tmp;

	uart_data.base = (void*)LPC21XX_UART0;
#endif
	uart_data.flags = 0;
	uart_data.pclk = clock_pclk;
	uart_data.baudrate = 115200;

	tty0.name = "tty0";
	tty0.drv = &serial_driver;
	if (tty0.drv->probe(&tty0, &uart_data) == 0)
		device_register(&tty0);
}

void udelay(int x)
{
	//x = x*(CONFIG_FCPU/1000000)/4/6; /* approx. for 60 MHz, w/o MAM: */
	x = x*(CONFIG_FCPU/1000000)/4; /* approx. for 60 MHz, w/o MAM: */
	asm volatile(
		"mov	%0, %1\n\t"
		".udelay_%=:\n\t"
		"subs	%0, %0, #1\n\t"
		"bne	.udelay_%=\n\t"
		: "=r" (x) : "r" (x) : "cc" );
}



static struct lpc_spi_data lpc_spi_data;

struct spi_master spi0 = {
	.spi = (void*)LPC21XX_SPI0,
	.spi_data = &lpc_spi_data,
	/* last_dev implicitly initialized */
};

static int board_spi_init()
{
	u32 tmp;
	PCONP |= PCONP_SPI0;

	tmp = PINSEL0;
	tmp &= ~(3<<4*2);
	tmp &= ~(3<<5*2);
	tmp &= ~(3<<6*2);
	tmp |= 1<<4*2; /* SCK0 */
	tmp |= 1<<5*2; /* MISO0 */
	tmp |= 1<<6*2; /* MOSI0 */
	/* SSEL0 remains GPIO */
	PINSEL0 = tmp;

	VICVectAddr1 = (u32)spi0_handler;
	VICVectCntl1 = 0x20 | IRQ_SPI0;

	lpc_spi_data.pclk = clock_pclk;
	lpc_spi_register(&spi0);
	spi_register_master(&spi0);

	return 0;
}


#ifdef LPC2138_SSP
static struct lpc_ssp_data lpc_ssp_data;

struct spi_master spi1 = {
	.spi = (void*)LPC21XX_SPI1,
	.spi_data = &lpc_ssp_data,
	/* last_dev implicitly initialized */
};

static int board_ssp_init()
{
	u32 tmp;
	PCONP |= PCONP_SPI1;

	tmp = PINSEL1;
	tmp &= ~(3<<1*2);
	tmp &= ~(3<<2*2);
	tmp &= ~(3<<3*2);
	tmp |= 2<<1*2; /* P0_17 SCK1 */
	tmp |= 2<<2*2; /* P0_18 MISO1 */
	tmp |= 2<<3*2; /* P0_19 MOSI1 */
	/* SSEL0 remains GPIO */
	PINSEL1 = tmp;

	VICVectAddr2 = (u32)ssp0_handler;
	VICVectCntl2 = 0x20 | IRQ_SPI1;

	lpc_ssp_data.pclk = clock_pclk;
	lpc_ssp_data.irq = IRQ_SPI1;
	lpc_ssp_register(&spi1);
	spi_register_master(&spi1);

	return 0;
}
#else
static int board_ssp_init() { return 0; }
#endif

#ifdef LPC2138_I2C0
static struct lpc_i2c lpc_i2c_data0;

struct i2c_master i2c0 = {
	.priv = &lpc_i2c_data0,
	.speed = 400000,
};

static int board_i2c0_init()
{
	u32 tmp;
	PCONP |= PCONP_I2C0;

	tmp = PINSEL0;
	tmp &= ~(3<<2*2);
	tmp &= ~(3<<3*2);
	tmp |= 1<<2*2; /* P0_2 SCL0 */
	tmp |= 1<<3*2; /* P0_3 SDA0 */
	PINSEL0 = tmp;

	VICVectAddr3 = (u32)i2c0_handler;
	VICVectCntl3 = 0x20 | IRQ_I2C0;

	lpc_i2c_data0.regs = (void*)LPC21XX_I2C0;
	lpc_i2c_data0.pclk = clock_pclk;
	lpc_i2c_data0.irq = IRQ_I2C0;
	lpc_i2c_data0.bus = 0;
	lpc_i2c_register(&i2c0);
	i2c_register_master(&i2c0);

	return 0;
}
#else
static int board_i2c0_init() { return 0; }
#endif

#ifdef LPC2138_I2C1
static struct lpc_i2c lpc_i2c_data1;

struct i2c_master i2c1 = {
	.priv = &lpc_i2c_data1,
	.speed = 400000,
};

static int board_i2c1_init()
{
	u32 tmp;
	PCONP |= PCONP_I2C1;

	tmp = PINSEL0;
	tmp &= ~(3<<11*2);
	tmp &= ~(3<<14*2);
	tmp |= 3<<11*2; /* P0_11 SCL1 */
	tmp |= 3<<14*2; /* P0_14 SDA1 */
	PINSEL0 = tmp;

	VICVectAddr4 = (u32)i2c1_handler;
	VICVectCntl4 = 0x20 | IRQ_I2C1;

	lpc_i2c_data1.regs = (void*)LPC21XX_I2C1;
	lpc_i2c_data1.pclk = clock_pclk;
	lpc_i2c_data1.irq = IRQ_I2C1;
	lpc_i2c_data1.bus = 1;
	lpc_i2c_register(&i2c1);
	i2c_register_master(&i2c1);

	return 0;
}
#else
static int board_i2c1_init() { return 0; }
#endif

void board_init()
{
	extern void generic_exception_handler(void);

	setup_pll(5, 2);	/* 5*12 = 60, p = [156/2/60, 320/2/60] - 2 -> 01 */

	clock_pclk = clock_hz/4;

	PCONP &= ~PCONP_UART0; /* because on my board i shorted it to UART1, so i can use max232. XXX, didn't help :P */
	board_spi_init();
	board_ssp_init();
	board_i2c0_init();
	board_i2c1_init();

	/* set-up interrupt on P0.15 (T3) transitioning to low - cbashell entry */
	u32 tmp = PINSEL0;
	tmp &= ~(3<<15*2);
	tmp |= 2<<15*2; /* P0_15 EINT2 */
	PINSEL0 = tmp;
	irq_enable(IRQ_EINT2);

	/* install handler for unhandled interrupts */
	VICDefVectAddr = (u32)generic_exception_handler;
}
