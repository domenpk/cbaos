/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <sched.h>

#ifdef BOARD_ITLPC2138
#include <mach/lpc21xx_gpio.h>
#elif defined BOARD_STM32F103X4_MINE && defined CONFIG_LCDTEST
#include <mach/stm32_gpio.h>
#endif

#include <drivers/hd44780.h>

void test_hd44780_func(u32 arg)
{
	int i = 0;
	struct hd44780_data lcd;
#ifdef BOARD_ITLPC2138
	lcd.pins.rs = GPIO_0_20;
	lcd.pins.rw = GPIO_0_22;
	lcd.pins.e = GPIO_0_21;
	lcd.pins.d4 = GPIO_0_16;
	lcd.pins.d5 = GPIO_0_17;
	lcd.pins.d6 = GPIO_0_18;
	lcd.pins.d7 = GPIO_0_19;
	lcd.Te = 1;
	lcd.caps = HD44780_CAPS_2LINES;
#elif defined BOARD_STM32F103X4_MINE && defined CONFIG_LCDTEST
	lcd.pins.rs = GPIO_PB5;
	lcd.pins.rw = GPIO_PA12;
	lcd.pins.e = GPIO_PA11;
	lcd.pins.d4 = GPIO_PB15;
	lcd.pins.d5 = GPIO_PB14;
	lcd.pins.d6 = GPIO_PB13;
	lcd.pins.d7 = GPIO_PB12;
	lcd.Te = 1;
	lcd.caps = HD44780_CAPS_2LINES;
#endif

	hd44780_driver.init(&lcd);
	hd44780_driver.onoff(&lcd, HD44780_ONOFF_DISPLAY_ON);
	hd44780_driver.print(&lcd, "yay, it works");

	msleep(2000);
	hd44780_driver.clear(&lcd);

	while (1) {
		char buf[2];

		hd44780_driver.set_position(&lcd, (i<16?HD44780_LINE_OFFSET:0) + i%16);
		sprintf(buf, "%X", i%16);
		hd44780_driver.print(&lcd, buf);

		hd44780_driver.set_position(&lcd, (i>=16?HD44780_LINE_OFFSET:0) + i%16);
		hd44780_driver.print(&lcd, " ");

		msleep(1000);
		i++;
		if (i == 32)
			i = 0;
	}
}
