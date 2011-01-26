/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <drivers/hd44780.h>
#include <board.h>
#include <gpio.h>

/* there are actually read commands/data too, but we won't use that.. for now... maybe BF is important */

static void lcd_put(struct hd44780_data *lcd, int rs, int data)
{
	gpio_set(lcd->pins.rs, rs);
	gpio_set(lcd->pins.rw, 0);
	udelay(lcd->Te);
	gpio_set(lcd->pins.e, 1);
	udelay(lcd->Te);
	gpio_set(lcd->pins.d4, data & 1);
	gpio_set(lcd->pins.d5, data>>1 & 1);
	gpio_set(lcd->pins.d6, data>>2 & 1);
	gpio_set(lcd->pins.d7, data>>3 & 1);
	gpio_set(lcd->pins.e, 0);
}

int lcd_read(struct hd44780_data *lcd, int rs)
{
	int tmp;
	gpio_init(lcd->pins.d4, GPIO_INPUT, 0);
	gpio_init(lcd->pins.d5, GPIO_INPUT, 0);
	gpio_init(lcd->pins.d6, GPIO_INPUT, 0);
	gpio_init(lcd->pins.d7, GPIO_INPUT, 0);

	gpio_set(lcd->pins.rs, rs);
	gpio_set(lcd->pins.rw, 1);
	udelay(lcd->Te);
	gpio_set(lcd->pins.e, 1);
	udelay(lcd->Te);

	tmp = 0;
	tmp |= gpio_get(lcd->pins.d4) << 4;
	tmp |= gpio_get(lcd->pins.d5) << 5;
	tmp |= gpio_get(lcd->pins.d6) << 6;
	tmp |= gpio_get(lcd->pins.d7) << 7;
	gpio_set(lcd->pins.e, 0);

	udelay(lcd->Te);
	gpio_set(lcd->pins.e, 1);
	udelay(lcd->Te);

	tmp |= gpio_get(lcd->pins.d4) << 0;
	tmp |= gpio_get(lcd->pins.d5) << 1;
	tmp |= gpio_get(lcd->pins.d6) << 2;
	tmp |= gpio_get(lcd->pins.d7) << 3;
	gpio_set(lcd->pins.e, 0);

	gpio_init(lcd->pins.d4, GPIO_OUTPUT, 0);
	gpio_init(lcd->pins.d5, GPIO_OUTPUT, 0);
	gpio_init(lcd->pins.d6, GPIO_OUTPUT, 0);
	gpio_init(lcd->pins.d7, GPIO_OUTPUT, 0);

	return tmp;
}

static void lcd_cmd(struct hd44780_data *lcd, int cmd)
{
	int timeout = 1000;
	while (--timeout) {
		/* bits 0-6 are address, that might be useful too */
		if ((lcd_read(lcd, 0) & 0x80) == 0)
			break;
	}
	if (timeout == 0)
		printf("%s, timeouted at cmd %x\n", __func__, cmd);

	lcd_put(lcd, 0, cmd>>4);
	lcd_put(lcd, 0, cmd&0xf);
}

static void lcd_data(struct hd44780_data *lcd, int cmd)
{
	int timeout = 1000;
	while (--timeout) {
		/* bits 0-6 are address, that might be useful too */
		if ((lcd_read(lcd, 0) & 0x80) == 0)
			break;
	}
	if (timeout == 0)
		printf("%s, timeouted at cmd %x\n", __func__, cmd);

	lcd_put(lcd, 1, cmd>>4);
	lcd_put(lcd, 1, cmd&0xf);
}

static void lcd_init(struct hd44780_data *lcd)
{
	gpio_init(lcd->pins.rs, GPIO_OUTPUT, 0);
	gpio_init(lcd->pins.rw, GPIO_OUTPUT, 0);
	gpio_init(lcd->pins.e, GPIO_OUTPUT, 0);
	gpio_init(lcd->pins.d4, GPIO_OUTPUT, 0);
	gpio_init(lcd->pins.d5, GPIO_OUTPUT, 0);
	gpio_init(lcd->pins.d6, GPIO_OUTPUT, 0);
	gpio_init(lcd->pins.d7, GPIO_OUTPUT, 0);

	/* reset sequence */
	lcd_put(lcd, 0, 3);
	udelay(4100);
	lcd_put(lcd, 0, 3);
	udelay(100);

	lcd_put(lcd, 0, 3);
	udelay(37);
	lcd_put(lcd, 0, 2);
	udelay(37);

	/* ok, in 4-bit mode now */
	int tmp = 0;
	if (lcd->caps & HD44780_CAPS_2LINES)
		tmp |= 1<<3;
	if (lcd->caps & HD44780_CAPS_5X10)
		tmp |= 1<<2;
	lcd_cmd(lcd, CMD_FUNCTION_SET | tmp);
	lcd_cmd(lcd, CMD_DISPLAY_ON_OFF); /* display, cursor and blink off */
	lcd_cmd(lcd, CMD_CLEAR);

	lcd_cmd(lcd, CMD_ENTRY_MODE | HD44780_ENTRY_INC);
}


static void lcd_clear(struct hd44780_data *lcd)
{
	lcd_cmd(lcd, CMD_CLEAR);
}

static void lcd_home(struct hd44780_data *lcd)
{
	lcd_cmd(lcd, CMD_HOME);
}

static void lcd_entry_mode(struct hd44780_data *lcd, int mode)
{
	lcd_cmd(lcd, CMD_ENTRY_MODE | (mode&0x3));
}

static void lcd_onoff(struct hd44780_data *lcd, int features)
{
	lcd_cmd(lcd, CMD_DISPLAY_ON_OFF | (features&0x7));
}

static void lcd_shift(struct hd44780_data *lcd, int shift)
{
	lcd_cmd(lcd, CMD_SHIFT | (shift&0xc));
}

static void lcd_set_position(struct hd44780_data *lcd, int pos)
{
	lcd_cmd(lcd, CMD_DDRAM_ADDR | (pos&0x7f));
}

static void lcd_print(struct hd44780_data *lcd, const char *str)
{
	while (*str)
		lcd_data(lcd, *str++);
}


const struct hd44780_driver hd44780_driver = {
	.init = lcd_init,
	.clear = lcd_clear,
	.home = lcd_home,
	.entry_mode = lcd_entry_mode,
	.onoff = lcd_onoff,
	.shift = lcd_shift,
	.set_position = lcd_set_position,
	.write = lcd_data,
	.print = lcd_print,

	.read = lcd_read,
	.write_cmd = lcd_cmd,
};
