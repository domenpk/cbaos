#include <types.h>
#include <compiler.h>
#include <gpio.h>
#include <interrupt.h>
#include <mach/stm32_gpio.h>
#include <mach/stm32_regs.h>
#include <board.h>
#include <string.h>
#include <stdio.h>

#include <drivers/hd44780.h>


/*
lcd interface:
3  PC14 (RS)
4  PC15 (R/W)
5  PD0  (E)
10 PA0  (DB4)
11 PA1  (DB5)
12 PA2  (DB6)
13 PA3  (DB7)

spi (tmp124):
25 PB12 (CS)
26 PB13 (SCK)
27 PB14 (MISO)

rotary:
41 PB5  (ROT_SW)
42 PB6  (ROT_L)
43 PB7  (ROT_R)

45 PB8  (LCD_BACKLIGHT) (TIM4_CH3)
46 PB9  (RELAY_SWITCH)
*/

#define PIN_ROT_SW	GPIO_PB5
#define PIN_ROT_L	GPIO_PB6
#define PIN_ROT_R	GPIO_PB7

#define PIN_LCD_BACKLIGHT	GPIO_PB8
#define PIN_RELAY_SWITCH	GPIO_PB9



int main()
{
	struct hd44780_data lcd;
	lcd.pins.rs = GPIO_PC14;
	lcd.pins.rw = GPIO_PC15;
	lcd.pins.e = GPIO_PD0;
	lcd.pins.d4 = GPIO_PA0;
	lcd.pins.d5 = GPIO_PA1;
	lcd.pins.d6 = GPIO_PA2;
	lcd.pins.d7 = GPIO_PA3;
	lcd.Te = 1;
	lcd.caps = HD44780_CAPS_2LINES;

	hd44780_driver.init(&lcd);
	hd44780_driver.onoff(&lcd, HD44780_ONOFF_DISPLAY_ON);
	hd44780_driver.print(&lcd, "yay");

	gpio_init(PIN_LCD_BACKLIGHT, GPIO_OUTPUT, 0);

	while (1) {
		gpio_set(PIN_LCD_BACKLIGHT, 1);
		udelay(500*1000);
		gpio_set(PIN_LCD_BACKLIGHT, 0);
		udelay(500*1000);
	}

	return 0;
}
