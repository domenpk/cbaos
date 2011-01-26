/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <drivers/ds2482_1w.h>
#include <drivers/1w.h>
#include <drivers/i2c.h>
#include <errno.h>


#define PTR_STATUS     0xf0
#define PTR_READ_DATA  0xe1
#define PTR_CONFIG     0xc3

#define CMD_DEV_RESET    0xf0
#define CMD_SET_READ_PTR 0xe1
#define CMD_WRITE_CONFIG 0xd2
#define CMD_1W_RESET     0xb4
#define CMD_1W_BIT       0x87
#define CMD_1W_WRITE     0xa5
#define CMD_1W_READ      0x96
#define CMD_1W_TRIPLET   0x78

#define STATUS_DIR (1<<7)
#define STATUS_TSB (1<<6)
#define STATUS_SBR (1<<5)
#define STATUS_RST (1<<4)
#define STATUS_LL  (1<<3)
#define STATUS_SD  (1<<2)
#define STATUS_PPD (1<<1)
#define STATUS_1WB (1<<0)

#define CONFIG_1WS (1<<3)
#define CONFIG_SPU (1<<2)
#define CONFIG_PPM (1<<1)
#define CONFIG_APU (1<<0)


static int ds2482_reset(struct w1_master *master)
{
	struct ds2482_data *chip = master->priv;
	u8 c = CMD_DEV_RESET;
	u8 reply;
	int r;

	r = i2c_write_read(chip->i2c, chip->addr, &c, 1, &reply, 1);
	if (r < 0)
		return r;

	if (reply & STATUS_RST)
		return 0;
	return -EINVAL;
}

static int ds2482_set_read_ptr(struct w1_master *master, u8 ptr)
{
	struct ds2482_data *chip = master->priv;
	u8 c[2];
	int r;

	c[0] = CMD_SET_READ_PTR;
	c[1] = ptr;
	r = i2c_write(chip->i2c, chip->addr, c, 2);
	if (r < 0)
		return r;

	return 0;
}

static int ds2482_write_config(struct w1_master *master, u8 config)
{
	struct ds2482_data *chip = master->priv;
	u8 c[2];
	int r;

	c[0] = CMD_WRITE_CONFIG;
	c[1] = ~config << 4 | config;
	r = i2c_write(chip->i2c, chip->addr, c, 2);
	if (r < 0)
		return r;

	return 0;
}

static int ds2482_1w_reset(struct w1_master *master)
{
	struct ds2482_data *chip = master->priv;
	u8 c = CMD_1W_RESET;
	int r;

	r = i2c_write(chip->i2c, chip->addr, &c, 1);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = i2c_read(chip->i2c, chip->addr, &c, 1);
		if (r < 0)
			return r;
		if ((c & STATUS_1WB) == 0)
			break;
	}
	return 0;
}

// bit == 1 also means read time slot
static int ds2482_1w_single_bit(struct w1_master *master, int bit)
{
	struct ds2482_data *chip = master->priv;
	u8 c[2] = { CMD_1W_BIT, bit<<7 };
	int r;

	r = i2c_write(chip->i2c, chip->addr, c, 2);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = i2c_read(chip->i2c, chip->addr, c, 1);
		if (r < 0)
			return r;
		if ((c[0] & STATUS_1WB) == 0)
			break;
	}
	return !!(c[0] & STATUS_SBR);
}

static int ds2482_1w_write(struct w1_master *master, u8 data)
{
	struct ds2482_data *chip = master->priv;
	u8 c[2] = { CMD_1W_WRITE, data };
	int r;

	r = i2c_write(chip->i2c, chip->addr, c, 2);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = i2c_read(chip->i2c, chip->addr, c, 1);
		if (r < 0)
			return r;
		if ((c[0] & STATUS_1WB) == 0)
			break;
	}
	return 0;
}

static int ds2482_1w_read(struct w1_master *master)
{
	struct ds2482_data *chip = master->priv;
	u8 c = CMD_1W_READ;
	int r;

	r = i2c_write(chip->i2c, chip->addr, &c, 1);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = i2c_read(chip->i2c, chip->addr, &c, 1);
		if (r < 0)
			return r;
		if ((c & STATUS_1WB) == 0)
			break;
	}

	r = ds2482_set_read_ptr(master, PTR_READ_DATA);
	if (r < 0)
		return r;

	r = i2c_read(chip->i2c, chip->addr, &c, 1);
	if (r < 0)
		return r;

	return c;
}

/* dir - 0/1 direction to take in case of device conflict */
static int ds2482_1w_triplet(struct w1_master *master, int dir)
{
	struct ds2482_data *chip = master->priv;
	u8 c[2] = { CMD_1W_TRIPLET, dir<<7 };
	int r;

	r = i2c_write(chip->i2c, chip->addr, c, 2);
	if (r < 0)
		return r;

	/* busy polling */
	while (1) {
		r = i2c_read(chip->i2c, chip->addr, c, 1);
		if (r < 0)
			return r;
		if ((c[0] & STATUS_1WB) == 0)
			break;
	}

	/* return value:
	 * bit 0: path taken
	 * bit 1: conflict?
	 */
	int bit1 = !!(c[0] & STATUS_SBR);
	int bit2 = !!(c[0] & STATUS_TSB);

	/* error, no device pulled the bit low */
	if (bit1 == 1 && bit2 == 1)
		return -ENODEV;

	dir = !!(c[0] & STATUS_DIR);

	/* conflict */
	if (bit1 == 0 && bit2 == 0)
		return 2 | dir;
	return dir;
}

int ds2482_1w_register(struct w1_master *master)
{
	//struct ds2482_data *chip = master->priv;
	int r = ds2482_reset(master);
	if (r < 0)
		return r;
	master->w1_reset = ds2482_1w_reset;
	master->w1_triplet = ds2482_1w_triplet;
	master->w1_write = ds2482_1w_write;
	master->w1_read = ds2482_1w_read;

	return 0;
}

