/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sched.h>
#include <lock.h>
#include <endianess.h>
#include <timekeeping.h>

#include <gpio.h>
#include <drivers/spi.h>
#include <drivers/enc28j60.h>

#include <net.h>
#include <net/netpacket.h>


/* debugging prints */
#define dprintf(...)
//#define dprintf printf


#define ENC28J60REG(dummy_byte, bank, size, addr) (((dummy_byte)<<24) | ((bank)<<16) | ((size)<<8) | (addr))
#define ENC28J60REG_DUMMY(reg) (((reg)>>24) & 0x1)
#define ENC28J60REG_BANK(reg)  (((reg)>>16) & 0x7)
#define ENC28J60REG_SIZE(reg)  (((reg)>>8) & 0x3)
#define ENC28J60REG_ADDR(reg)  (((reg)>>0) & 0x1f)

#define ERDPT           ENC28J60REG(0, 0, 2, 0x00)
#define EWRPT           ENC28J60REG(0, 0, 2, 0x02)
#define ETXST           ENC28J60REG(0, 0, 2, 0x04)
#define ETXND           ENC28J60REG(0, 0, 2, 0x06)
#define ERXST           ENC28J60REG(0, 0, 2, 0x08)
#define ERXND           ENC28J60REG(0, 0, 2, 0x0a)
#define ERXRDPT         ENC28J60REG(0, 0, 2, 0x0c)
#define ERXWRPT         ENC28J60REG(0, 0, 2, 0x0e)
#define EDMAST          ENC28J60REG(0, 0, 2, 0x10)
#define EDMAND          ENC28J60REG(0, 0, 2, 0x12)
#define EDMADST         ENC28J60REG(0, 0, 2, 0x14)
#define EDMACS          ENC28J60REG(0, 0, 2, 0x16)

#define E1              ENC28J60REG(0, 1, 1, 0x00)
#define EPMCS           ENC28J60REG(0, 1, 2, 0x10)
#define EPMO            ENC28J60REG(0, 1, 2, 0x14)
#define ERXFCON         ENC28J60REG(0, 1, 1, 0x18)
#define EPKTCNT         ENC28J60REG(0, 1, 1, 0x19)

#define MACON1          ENC28J60REG(1, 2, 1, 0x00)
#define MACON2          ENC28J60REG(1, 2, 1, 0x01)
#define MACON3          ENC28J60REG(1, 2, 1, 0x02)
#define MACON4          ENC28J60REG(1, 2, 1, 0x03)
#define MABBIPG         ENC28J60REG(1, 2, 1, 0x04)
#define MAIPG           ENC28J60REG(1, 2, 2, 0x06)
#define MACLCON1        ENC28J60REG(1, 2, 1, 0x08)
#define MACLCON2        ENC28J60REG(1, 2, 1, 0x09)
#define MAMXFL          ENC28J60REG(1, 2, 2, 0x0a)
#define MAPHSUP         ENC28J60REG(1, 2, 1, 0x0d)
#define MICON           ENC28J60REG(1, 2, 1, 0x11)
#define MICMD           ENC28J60REG(1, 2, 1, 0x12)
#define MIREGADR        ENC28J60REG(1, 2, 1, 0x14)
#define MIWR            ENC28J60REG(1, 2, 2, 0x16)
#define MIRD            ENC28J60REG(1, 2, 2, 0x18)

#define MAADR1          ENC28J60REG(1, 3, 1, 0x00)
#define MAADR0          ENC28J60REG(1, 3, 1, 0x01)
#define MAADR3          ENC28J60REG(1, 3, 1, 0x02)
#define MAADR2          ENC28J60REG(1, 3, 1, 0x03)
#define MAADR5          ENC28J60REG(1, 3, 1, 0x04)
#define MAADR4          ENC28J60REG(1, 3, 1, 0x05)
#define EBSTSD          ENC28J60REG(0, 3, 1, 0x06)
#define EBSTCON         ENC28J60REG(0, 3, 1, 0x07)
#define EBSTCS          ENC28J60REG(0, 3, 2, 0x08)
#define MISTAT          ENC28J60REG(1, 3, 1, 0x0a)
#define EREVID          ENC28J60REG(0, 3, 1, 0x12)
#define ECOCON          ENC28J60REG(0, 3, 1, 0x15)
#define EFLOCON         ENC28J60REG(0, 3, 1, 0x17)
#define EPAUS           ENC28J60REG(0, 3, 2, 0x18)

/* all banks */
#define EIE             ENC28J60REG(0, 4, 1, 0x1b)
#define EIR             ENC28J60REG(0, 4, 1, 0x1c)
#define ESTAT           ENC28J60REG(0, 4, 1, 0x1d)
#define ECON2           ENC28J60REG(0, 4, 1, 0x1e)
#define ECON1           ENC28J60REG(0, 4, 1, 0x1f)

/* bits */
#define ESTAT_CLKRDY	(1<<0)
#define ESTAT_TXABRT	(1<<1)
#define ESTAT_RXBUSY	(1<<2)
#define ESTAT_LATECOL	(1<<4)
#define ESTAT_INT	(1<<7)

#define ECON1_RXEN	(1<<2)
#define ECON1_TXRTS	(1<<3)
#define ECON1_CSUMEN	(1<<4)
#define ECON1_DMAST	(1<<5)
#define ECON1_RXRST	(1<<6)
#define ECON1_TXRST	(1<<7)

#define ECON2_PKTDEC	(1<<6)

#define EIR_TXERIF	(1<<0)
#define EIR_RXERIF	(1<<1)
#define EIR_WOLIF	(1<<2)
#define EIR_TXIF	(1<<3)


/* initialize receive buffer to 0-0xfff */
#define ERXST_VALUE	0x0
#define ERXND_VALUE	0xfff
#define ETXST_VALUE	0x1000
#define ETXND_MAX_VALUE	(0x1fff-7) /* for status vector */

/*
enc init
enc reg 0x200 1
enc reg 0x208 1
enc reg 0x20a 0xfff
enc reg 0x20c 1
enc reg 0x1020101 0
enc reg 0x1020100 1
enc reg 0x1020102 0xf0
enc reg 0x11f 0x4

^ MACON2
  MACON1
  MACON3

on RX EPKTCNT++, ERXWRPT+=x
enc reg 0x10119
enc reg 0x20e

enc reg 0x10118 - default is ok
*/


#define ENC_TIME 1000
static u32 enc_time;

struct enc28j60_dev {
	struct spi_device *spi;
	int bank;
};

struct enc28j60_dev enc;

int enc28j60_reg_write(struct enc28j60_dev *enc, u32 reg, u32 value);
int enc28j60_reg_read(struct enc28j60_dev *enc, u32 reg);

static int enc28j60_switch_bank(struct enc28j60_dev *enc, int bank)
{
	int ret = 0;

	if (bank != 4 && bank != enc->bank) {
		ret = enc28j60_reg_write(enc, ECON1, (enc28j60_reg_read(enc, ECON1) & ~0x3) | bank);

		enc->bank = bank;
	}
	return ret;
}

int enc28j60_reg_read(struct enc28j60_dev *enc, u32 reg)
{
	int dummy = ENC28J60REG_DUMMY(reg);
	int bank = ENC28J60REG_BANK(reg);
	int size = ENC28J60REG_SIZE(reg);
	u32 value = 0;
	int i;

	enc28j60_switch_bank(enc, bank);

	for (i=0; i<size; i++) {
		struct spi_transfer transfer;
		u8 buf[3];
		buf[0] = ENC28J60REG_ADDR(reg+i);

		transfer.tx_buf = buf;
		transfer.rx_buf = buf;
		transfer.len = 2+dummy;

		int ret = spi_transfer(enc->spi, &transfer);
		if (ret < 0)
			return ret;
		value |= buf[1+dummy] << (8*i);
	}

	return value;
}

static int _enc28j60_reg_write_common(struct enc28j60_dev *enc, u32 reg, u32 value, u8 _cmd)
{
	int bank = ENC28J60REG_BANK(reg);
	int size = ENC28J60REG_SIZE(reg);
	int i;
	int ret = 0;

	enc28j60_switch_bank(enc, bank);

	for (i=0; i<size; i++) {
		struct spi_transfer transfer;
		u8 buf[2];
		buf[0] = _cmd | ENC28J60REG_ADDR(reg+i);
		buf[1] = (value >> (8*i)) & 0xff;

		transfer.tx_buf = buf;
		transfer.rx_buf = buf;
		transfer.len = 2;
		
		ret = spi_transfer(enc->spi, &transfer);
		if (ret < 0)
			break;
	}

	return ret;
}

int enc28j60_reg_write(struct enc28j60_dev *enc, u32 reg, u32 value)
{
	return _enc28j60_reg_write_common(enc, reg, value, 0x40);
}

/* only for E regs! */
static int enc28j60_reg_write_bits_set(struct enc28j60_dev *enc, u32 reg, u32 value)
{
	return _enc28j60_reg_write_common(enc, reg, value, 0x80);
}
static int enc28j60_reg_write_bits_clear(struct enc28j60_dev *enc, u32 reg, u32 value)
{
	return _enc28j60_reg_write_common(enc, reg, value, 0xa0);
}

/* AUTOINC needs to be enabled for this. buf must be 1 longer, first byte is cmd */
static int enc28j60_read(struct enc28j60_dev *enc, u8 *buf, int len)
{
	struct spi_transfer transfer;
	buf[0] = 0x3a; /* RBM, read buffer memory */

	transfer.tx_buf = buf;
	transfer.rx_buf = buf;
	transfer.len = len;

	int ret = spi_transfer(enc->spi, &transfer);
	return ret;
}

/* AUTOINC needs to be enabled for this. buf must be 1 longer, first byte is cmd */
static int enc28j60_write(struct enc28j60_dev *enc, u8 *buf, int len)
{
	struct spi_transfer transfer;
	buf[0] = 0x7a; /* WBM, write buffer memory */

	transfer.tx_buf = buf;
	transfer.rx_buf = buf;
	transfer.len = len;

	int ret = spi_transfer(enc->spi, &transfer);
	return ret;
}

static int enc28j60_reset(struct enc28j60_dev *enc)
{
	struct spi_transfer transfer;
	u8 buf[1];
	buf[0] = 0xff; /* system command - soft reset */

	transfer.tx_buf = buf;
	transfer.rx_buf = buf;
	transfer.len = 1;

	int ret = spi_transfer(enc->spi, &transfer);
	return ret;
}

static int enc28j60_sanity_check(struct enc28j60_dev *enc)
{
	if ((enc28j60_reg_read(enc, ECON2)&0xf8) != 0x80)
		return -1;
	if ((enc28j60_reg_read(enc, ERDPT)&0x1fff) != 0x5fa)
		return -2;
	return 0;
}


static struct spi_device dev = {
//	.mode = 0,
//	.cs_pin = gpio_cs,
	.clock = 10*1000*1000, /* 10 MHz max */
};


static struct sem packet_ready_sem;
static int enc28j60_irqhandler(int pin)
{
	up(&packet_ready_sem);
	enc_time = time_ms();
	return 0;
}

int enc28j60_setup(struct spi_master *spi, int gpio_cs, int gpio_int)
{
	dev.cs_pin = gpio_cs;
	spi_register_device(spi, &dev);
	enc.spi = &dev;
	enc.bank = 0;

	if (enc28j60_reset(&enc) < 0)
		return -1;

	if (enc28j60_sanity_check(&enc) < 0)
		return -1;

	/* clear bank, although reset should do that already */
	if (enc28j60_reg_write(&enc, ECON1, 0) < 0)
		return -1;
	enc.bank = 0;

	if (enc28j60_reg_write(&enc, MACON2, 0x80) < 0)
		return -1;
	if (enc28j60_reg_write(&enc, MACON1, 0) < 0)
		return -1;

	/* OST. 300us wait time until MAC, MII or PHY regs can be accessed */
	/* ERRATA 2. CLKRDY set early - wait at least 1ms */
	msleep(1);

	/* initialize pointers */
	if (enc28j60_reg_write(&enc, ERDPT, ERXST_VALUE) < 0)
		return -1;
	/* ERRATA 5.  Receive buffer must start at 0000h */
	if (enc28j60_reg_write(&enc, ERXST, ERXST_VALUE) < 0)
		return -1;
	if (enc28j60_reg_write(&enc, ERXND, ERXND_VALUE) < 0)
		return -1;
	if (enc28j60_reg_write(&enc, ERXRDPT, ERXND_VALUE) < 0)
		return -1;
	/* clear MARST */
	if (enc28j60_reg_write(&enc, MACON2, 0) < 0)
		return -1;
	/* MARXEN */
	if (enc28j60_reg_write(&enc, MACON1, 1) < 0)
		return -1;
	/* configure padding, crc */
	if (enc28j60_reg_write(&enc, MACON3, 0x30) < 0)
	//if (enc28j60_reg_write(&enc, MACON3, 0x32) < 0)
		return -1;

	/* set MAC */
	if (enc28j60_reg_write(&enc, MAADR5, netconfig.mac[0]) < 0)
		return -1;
	if (enc28j60_reg_write(&enc, MAADR4, netconfig.mac[1]) < 0)
		return -1;
	if (enc28j60_reg_write(&enc, MAADR3, netconfig.mac[2]) < 0)
		return -1;
	if (enc28j60_reg_write(&enc, MAADR2, netconfig.mac[3]) < 0)
		return -1;
	if (enc28j60_reg_write(&enc, MAADR1, netconfig.mac[4]) < 0)
		return -1;
	if (enc28j60_reg_write(&enc, MAADR0, netconfig.mac[5]) < 0)
		return -1;

	sem_init(&packet_ready_sem, 0);
	gpio_init(gpio_int, GPIO_INPUT, 0);
	gpio_irq_register(gpio_int, GPIO_IRQ_EDGE_FALLING, enc28j60_irqhandler);

	/* enable RX interrupt (INTIE, PKTIE) */
	if (enc28j60_reg_write_bits_set(&enc, EIE, 0xc0) < 0)
		return -1;
	/* enable reception */
	if (enc28j60_reg_write_bits_set(&enc, ECON1, ECON1_RXEN) < 0)
		return -1;

	return 0;
}

static void enc28j60_debug(void)
{
	printf("%s: ECON1:%x (0x08 should be clear on success or abort)\n", __func__, enc28j60_reg_read(&enc, ECON1));
	printf("%s: ECON2:%x\n", __func__, enc28j60_reg_read(&enc, ECON2));
	printf("%s: EIE:%x\n", __func__, enc28j60_reg_read(&enc, EIE));
	printf("%s: EIR:%x\n", __func__, enc28j60_reg_read(&enc, EIR));
	printf("%s: ESTAT:%x\n", __func__, enc28j60_reg_read(&enc, ESTAT));
//	printf("%s: MACLCON1:%x\n", __func__, enc28j60_reg_read(&enc, MACLCON1));
//	printf("%s: MACLCON2:%x\n", __func__, enc28j60_reg_read(&enc, MACLCON2));
//	printf("%s: ETXST:%x\n", __func__, enc28j60_reg_read(&enc, ETXST));
//	printf("%s: ETXND:%x\n", __func__, enc28j60_reg_read(&enc, ETXND));
//	printf("%s: EWRPT:%x\n", __func__, enc28j60_reg_read(&enc, EWRPT));
	printf("%s: EREVID:%x\n", __func__, enc28j60_reg_read(&enc, EREVID));
}

int enc28j60_raw_send(u8 *buf, unsigned len)
{
	int retries;

	unsigned txnd = ETXST_VALUE+len-1-1;
	if (txnd > ETXND_MAX_VALUE) {
		/* Must check len before EWRPT below. Wrapping doesn't occur for WRITE */
		printf("%s: txnd invalid %x\n", __func__, txnd);
		return -1;
	}
	/* 1. program ETXST to unused location */
	if (enc28j60_reg_write(&enc, ETXST, ETXST_VALUE) < 0)
		return -1;

	buf[1] = 0x00; /* control byte */;
	//buf[1] = 0x07; /* override MACON3 */;

	dprintf("TX sending eth packet\n");
	if (enc28j60_reg_write(&enc, EWRPT, ETXST_VALUE) < 0)
		return -1;
	/* 2. WBM command with all the packet data */
	if (enc28j60_write(&enc, buf, len) < 0)
		return -1;

	/* 3. program ETXND */
	if (enc28j60_reg_write(&enc, ETXND, txnd) < 0)
		return -1;

	/* set irq stuff here */
	/* 4. partial, clear EIR.TXIF */
	if (enc28j60_reg_write_bits_set(&enc, EIR, EIR_TXIF) < 0)
		return -1;

	/* 5. start: ECON1.TXRTS = 1 */
	if (enc28j60_reg_write_bits_set(&enc, ECON1, ECON1_TXRTS) < 0)
		return -1;

	dprintf("TX waiting for tx to finish\n");
	retries = 0;
	while (++retries < 10) {
		int tmp = enc28j60_reg_read(&enc, ECON1);
		if (tmp < 0)
			return -1;
		if ((tmp & ECON1_TXRTS) == 0)
			break;
		msleep(10);
	}
	u8 stat = enc28j60_reg_read(&enc, ESTAT);
	/*
	 * ESTAT	INT	r	r	LATECOL	-	RXBUSY	TXABRT	CLKRDY
	 * ECON1	TXRST	RXRST	DMAST	CSUMEN	TXRTS	RXEN	BSEL1	BSEL0
	 * EIR		-	PKTIF	DMAIF	LINKIF	TXIF	WOLIF	TXERIF	RXERIF
	 */
	if (retries >= 10 || (stat&ESTAT_TXABRT)) {
		printf("error TX fail (%d), stat:%x\n", retries, stat);
		enc28j60_debug();
		if (enc28j60_reg_write(&enc, ERDPT, ETXST_VALUE+len-1) < 0)
			return -1;
		if (enc28j60_read(&enc, buf, 1+7) < 0)
			return -1;
		printf("status vector (only valid if ECON1&0x08=0)%x %x  %x %x  %x %x  %x\n",
				buf[1], buf[2], buf[3], buf[4],
				buf[5], buf[6], buf[7]);
		/* ECON1.TXRTS = 0 (cancel transmission) */
		if (enc28j60_reg_write_bits_clear(&enc, ECON1, ECON1_TXRTS) < 0)
			return -1;

		/* TXERIF = 0 */
		if (enc28j60_reg_write_bits_clear(&enc, EIR, EIR_TXERIF) < 0)
			return -1;
		if (enc28j60_reg_write_bits_clear(&enc, ESTAT, ESTAT_TXABRT) < 0)
			return -1;
		enc28j60_debug();

		printf("errata 12\n");
		/* ERRATA 12. Transmit abort may stall transmit logic - reset TX */
		if (enc28j60_reg_write_bits_set(&enc, ECON1, ECON1_TXRST) < 0)
			return -1;
		if (enc28j60_reg_write_bits_clear(&enc, ECON1, ECON1_TXRST) < 0)
			return -1;
		if (enc28j60_reg_write_bits_clear(&enc, EIR, EIR_TXERIF) < 0)
			return -1;
		enc28j60_debug();

		return -1;
	}

	dprintf("TX done (%d). status: %x\n", retries, stat);
	return 0;
}

int enc28j60_process_packet(void)
{
	static u8 _buf[512];
	struct netpacket *packet = (struct netpacket*)_buf;
	u8 *buf = _buf+1;
	static int debug_timeout;
	static u16 next_rdpt = ERXST_VALUE; /* keeps track of RDPT across calls */

	u32 now = time_ms();
	if (!(!enc_time || time_after_eq(now, enc_time))) {
		return -EAGAIN;
	}
	enc_time = now + ENC_TIME;

	// FIXME somehow we need better event handling.
	// Maybe one semaphore for everything in that thread?
//	int ret = down(&packet_ready_sem, ms2ticks(1000));
	//ret = ret; /* if it's timeout, we also check, so same behaviour */

	if (enc28j60_reg_read(&enc, EPKTCNT) < 1) {
		/* no pending packets */
		if (debug_timeout++ > 1000) {
			printf("XXX RX no packet after 1000 tries\n");
			debug_timeout = 0;
			enc28j60_debug();
			printf("PKTCNT: %i\n", enc28j60_reg_read(&enc, EPKTCNT));
			printf("ERDPT: %#x\n", enc28j60_reg_read(&enc, ERDPT));
			printf("ERXRDPT: %#x\n", enc28j60_reg_read(&enc, ERXRDPT));
			printf("ERXWRPT: %#x\n", enc28j60_reg_read(&enc, ERXWRPT));
		}
		return -EAGAIN;
	}
	debug_timeout = 0;

	dprintf("ERDPT: %#x\n", enc28j60_reg_read(&enc, ERDPT));
	dprintf("ERXRDPT: %#x\n", enc28j60_reg_read(&enc, ERXRDPT));
	dprintf("ERXWRPT: %#x\n", enc28j60_reg_read(&enc, ERXWRPT));

	/* Must set ERDPT, since it could be changed by error handling in TX */
	if (enc28j60_reg_write(&enc, ERDPT, next_rdpt) < 0)
		return -1;
	/* Wrapping does occur if going past ERXND, so that's fine */
	if (enc28j60_read(&enc, _buf, 1+6) < 0)
		return -1;
	next_rdpt = get_le16(&buf[0]);
	u32 status = get_le32(&buf[2]);

	u16 len = status & 0xffff;
	if (len > sizeof(_buf)-offsetof(struct netpacket, ethernet))
		len = sizeof(_buf)-offsetof(struct netpacket, ethernet);

	dprintf(".next: %#x, status: %#08x, len:%i\t", next_rdpt, status, len);

	if (enc28j60_read(&enc, &packet->hw_dependent[1], 1+len) < 0)
		return -1;

	packet->len = len;
	netpacket_handle(packet);

	/* free the buf on enc side */
	/* ERRATA 14. Even values in ERXRDPT may corrupt receive buffer */
	u16 rxnd = next_rdpt-1;
	if (next_rdpt == 0)
		rxnd = ERXND_VALUE;
	if (enc28j60_reg_write(&enc, ERXRDPT, rxnd) < 0)
		return -1;

	/* this also automatically clears EIR[PKTIF], when PKTCNT reaches 0 */
	/* NOTE ERRATA 6. PKTIF is unreliable, but interrupt is triggered properly */
	if (enc28j60_reg_write_bits_set(&enc, ECON2, ECON2_PKTDEC) < 0)
		return -1;

	/* increment semaphore because we have more packets */
	if (enc28j60_reg_read(&enc, EPKTCNT) > 0) {
		up(&packet_ready_sem);
		enc_time = now;
	}

	dprintf("PKTCNT: %i\n", enc28j60_reg_read(&enc, EPKTCNT));
	dprintf("ERDPT: %#x\n", enc28j60_reg_read(&enc, ERDPT));
	dprintf("ERXRDPT: %#x\n", enc28j60_reg_read(&enc, ERXRDPT));
	dprintf("ERXWRPT: %#x\n", enc28j60_reg_read(&enc, ERXWRPT));

	return 0;
}
