#ifndef _ARCH_GPIO_H_
#define _ARCH_GPIO_H_

#define GPIO_MARK 0x800
#define GPIO_HYST (1<<5)
#define IOCON_REG(x) ((x)/4<<12)
#define IOCON_SET(x) ((x)<<20)

#define GPIO_0_0_RESET		(0x000 | GPIO_MARK | IOCON_REG(0x0c) | IOCON_SET(0xc0))
#define GPIO_0_0		(0x000 | GPIO_MARK | IOCON_REG(0x0c) | IOCON_SET(0xc1|GPIO_HYST))
#define GPIO_0_1		(0x001 | GPIO_MARK | IOCON_REG(0x10) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_0_1_CLKOUT		(0x001 | GPIO_MARK | IOCON_REG(0x10) | IOCON_SET(0xc1))
#define GPIO_0_1_CT32B0_MAT2	(0x001 | GPIO_MARK | IOCON_REG(0x10) | IOCON_SET(0xc2))
#define GPIO_0_2		(0x002 | GPIO_MARK | IOCON_REG(0x1c) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_0_2_SSEL0		(0x002 | GPIO_MARK | IOCON_REG(0x1c) | IOCON_SET(0xc1))
#define GPIO_0_2_CT16B0_CAP0	(0x002 | GPIO_MARK | IOCON_REG(0x1c) | IOCON_SET(0xc2))
#define GPIO_0_3		(0x003 | GPIO_MARK | IOCON_REG(0x2c) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_0_4		(0x004 | GPIO_MARK | IOCON_REG(0x30) | IOCON_SET(0x100)) /* open drain pin */
#define GPIO_0_4_SCL		(0x004 | GPIO_MARK | IOCON_REG(0x30) | IOCON_SET(0x1))
#define GPIO_0_5		(0x005 | GPIO_MARK | IOCON_REG(0x34) | IOCON_SET(0x100)) /* open drain pin */
#define GPIO_0_5_SDA		(0x005 | GPIO_MARK | IOCON_REG(0x34) | IOCON_SET(0x1))
#define GPIO_0_6		(0x006 | GPIO_MARK | IOCON_REG(0x4c) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_0_6_SCK0		(0x006 | GPIO_MARK | IOCON_REG(0x4c) | IOCON_SET(0xc2)) /* + SCKLOC setting! */
#define GPIO_0_7		(0x007 | GPIO_MARK | IOCON_REG(0x50) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_0_7_CTS		(0x007 | GPIO_MARK | IOCON_REG(0x50) | IOCON_SET(0xc1))
#define GPIO_0_8		(0x008 | GPIO_MARK | IOCON_REG(0x60) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_0_8_MISO0		(0x008 | GPIO_MARK | IOCON_REG(0x60) | IOCON_SET(0xc1))
#define GPIO_0_8_CT16B0_MAT0	(0x008 | GPIO_MARK | IOCON_REG(0x60) | IOCON_SET(0xc2))
#define GPIO_0_9		(0x009 | GPIO_MARK | IOCON_REG(0x64) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_0_9_MOSI0		(0x009 | GPIO_MARK | IOCON_REG(0x64) | IOCON_SET(0xc1))
#define GPIO_0_9_CT16B0_MAT1	(0x009 | GPIO_MARK | IOCON_REG(0x64) | IOCON_SET(0xc2))
#define GPIO_0_9_SWO		(0x009 | GPIO_MARK | IOCON_REG(0x64) | IOCON_SET(0xc3))
#define GPIO_0_10_SWCLK		(0x00a | GPIO_MARK | IOCON_REG(0x68) | IOCON_SET(0xc0))
#define GPIO_0_10		(0x00a | GPIO_MARK | IOCON_REG(0x68) | IOCON_SET(0xc1|GPIO_HYST))
#define GPIO_0_10_SCK0		(0x00a | GPIO_MARK | IOCON_REG(0x68) | IOCON_SET(0xc2)) /* + SCKLOC setting! */
#define GPIO_0_10_CT16B0_MAT2	(0x00a | GPIO_MARK | IOCON_REG(0x68) | IOCON_SET(0xc3))
#define GPIO_0_11		(0x00b | GPIO_MARK | IOCON_REG(0x74) | IOCON_SET(0xc1|GPIO_HYST))
#define GPIO_0_11_AD0		(0x00b | GPIO_MARK | IOCON_REG(0x74) | IOCON_SET(0x42))
#define GPIO_0_11_CT32B0_MAT3	(0x00b | GPIO_MARK | IOCON_REG(0x74) | IOCON_SET(0xc3))

#define GPIO_1_0		(0x100 | GPIO_MARK | IOCON_REG(0x78) | IOCON_SET(0xc1|GPIO_HYST))
#define GPIO_1_0_AD1		(0x100 | GPIO_MARK | IOCON_REG(0x78) | IOCON_SET(0x42))
#define GPIO_1_0_CT32B1_CAP0	(0x100 | GPIO_MARK | IOCON_REG(0x78) | IOCON_SET(0xc3))
#define GPIO_1_1		(0x101 | GPIO_MARK | IOCON_REG(0x7c) | IOCON_SET(0xc1|GPIO_HYST))
#define GPIO_1_1_AD2		(0x101 | GPIO_MARK | IOCON_REG(0x7c) | IOCON_SET(0x42))
#define GPIO_1_1_CT32B1_MAT0	(0x101 | GPIO_MARK | IOCON_REG(0x7c) | IOCON_SET(0xc3))
#define GPIO_1_2		(0x102 | GPIO_MARK | IOCON_REG(0x80) | IOCON_SET(0xc1|GPIO_HYST))
#define GPIO_1_2_AD3		(0x102 | GPIO_MARK | IOCON_REG(0x80) | IOCON_SET(0x42))
#define GPIO_1_2_CT32B1_MAT1	(0x102 | GPIO_MARK | IOCON_REG(0x80) | IOCON_SET(0xc3))
#define GPIO_1_3_SWDIO		(0x103 | GPIO_MARK | IOCON_REG(0x90) | IOCON_SET(0xc0))
#define GPIO_1_3		(0x103 | GPIO_MARK | IOCON_REG(0x90) | IOCON_SET(0xc1|GPIO_HYST))
#define GPIO_1_3_AD4		(0x103 | GPIO_MARK | IOCON_REG(0x90) | IOCON_SET(0x42))
#define GPIO_1_3_CT32B1_MAT2	(0x103 | GPIO_MARK | IOCON_REG(0x90) | IOCON_SET(0xc3))
#define GPIO_1_4		(0x104 | GPIO_MARK | IOCON_REG(0x94) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_1_4_AD5		(0x104 | GPIO_MARK | IOCON_REG(0x94) | IOCON_SET(0x41))
#define GPIO_1_4_CT32B1_MAT3	(0x104 | GPIO_MARK | IOCON_REG(0x94) | IOCON_SET(0xc2))
#define GPIO_1_5		(0x105 | GPIO_MARK | IOCON_REG(0xa0) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_1_5_RTS		(0x105 | GPIO_MARK | IOCON_REG(0xa0) | IOCON_SET(0xc1))
#define GPIO_1_5_CT32B0_CAP0	(0x105 | GPIO_MARK | IOCON_REG(0xa0) | IOCON_SET(0xc2))
#define GPIO_1_6		(0x106 | GPIO_MARK | IOCON_REG(0xa4) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_1_6_UART_RXD	(0x106 | GPIO_MARK | IOCON_REG(0xa4) | IOCON_SET(0xc1))
#define GPIO_1_6_CT32B0_MAT0	(0x106 | GPIO_MARK | IOCON_REG(0xa4) | IOCON_SET(0xc2))
#define GPIO_1_7		(0x107 | GPIO_MARK | IOCON_REG(0xa8) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_1_7_UART_TXD	(0x107 | GPIO_MARK | IOCON_REG(0xa8) | IOCON_SET(0xc1))
#define GPIO_1_7_CT32B0_MAT1	(0x107 | GPIO_MARK | IOCON_REG(0xa8) | IOCON_SET(0xc2))
#define GPIO_1_8		(0x108 | GPIO_MARK | IOCON_REG(0x14) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_1_8_CT16B1_CAP0	(0x108 | GPIO_MARK | IOCON_REG(0x14) | IOCON_SET(0xc1))
#define GPIO_1_9		(0x109 | GPIO_MARK | IOCON_REG(0x38) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_1_9_CT16B1_MAT0	(0x109 | GPIO_MARK | IOCON_REG(0x38) | IOCON_SET(0xc1))
#define GPIO_1_10		(0x10a | GPIO_MARK | IOCON_REG(0x6c) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_1_10_AD6		(0x10a | GPIO_MARK | IOCON_REG(0x6c) | IOCON_SET(0x41))
#define GPIO_1_10_CT16B1_MAT1	(0x10a | GPIO_MARK | IOCON_REG(0x6c) | IOCON_SET(0xc2))
#define GPIO_1_11		(0x10b | GPIO_MARK | IOCON_REG(0x98) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_1_11_AD7		(0x10b | GPIO_MARK | IOCON_REG(0x98) | IOCON_SET(0x41))

#define GPIO_2_0		(0x200 | GPIO_MARK | IOCON_REG(0x08) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_0_DTR		(0x200 | GPIO_MARK | IOCON_REG(0x08) | IOCON_SET(0xc1)) /* no DTRSEL, both are driven */
#define GPIO_2_0_SSEL1		(0x200 | GPIO_MARK | IOCON_REG(0x08) | IOCON_SET(0xc2))
#define GPIO_2_1		(0x201 | GPIO_MARK | IOCON_REG(0x28) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_1_DSR		(0x201 | GPIO_MARK | IOCON_REG(0x28) | IOCON_SET(0xc1)) /* + DSRLOC */
#define GPIO_2_1_SCK1		(0x201 | GPIO_MARK | IOCON_REG(0x28) | IOCON_SET(0xc2))
#define GPIO_2_2		(0x202 | GPIO_MARK | IOCON_REG(0x5c) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_2_DCD		(0x202 | GPIO_MARK | IOCON_REG(0x5c) | IOCON_SET(0xc1)) /* + DCD_LOC */
#define GPIO_2_2_MISO1		(0x202 | GPIO_MARK | IOCON_REG(0x5c) | IOCON_SET(0xc2))
#define GPIO_2_3		(0x203 | GPIO_MARK | IOCON_REG(0x8c) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_3_RI		(0x203 | GPIO_MARK | IOCON_REG(0x8c) | IOCON_SET(0xc1)) /* + RILOC */
#define GPIO_2_3_MOSI1		(0x203 | GPIO_MARK | IOCON_REG(0x8c) | IOCON_SET(0xc2))
#define GPIO_2_4		(0x204 | GPIO_MARK | IOCON_REG(0x40) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_5		(0x205 | GPIO_MARK | IOCON_REG(0x44) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_6		(0x206 | GPIO_MARK | IOCON_REG(0x00) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_7		(0x207 | GPIO_MARK | IOCON_REG(0x20) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_8		(0x208 | GPIO_MARK | IOCON_REG(0x24) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_9		(0x209 | GPIO_MARK | IOCON_REG(0x54) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_10		(0x20a | GPIO_MARK | IOCON_REG(0x58) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_11		(0x20b | GPIO_MARK | IOCON_REG(0x70) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_2_11_SCK		(0x20b | GPIO_MARK | IOCON_REG(0x70) | IOCON_SET(0xc1)) /* + SCKLOC setting! */

#define GPIO_3_0		(0x300 | GPIO_MARK | IOCON_REG(0x84) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_3_0_DTR		(0x300 | GPIO_MARK | IOCON_REG(0x84) | IOCON_SET(0xc1)) /* no DTRSEL, both are driven */
#define GPIO_3_1		(0x301 | GPIO_MARK | IOCON_REG(0x88) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_3_1_DSR		(0x301 | GPIO_MARK | IOCON_REG(0x88) | IOCON_SET(0xc1)) /* + DSRLOC */
#define GPIO_3_2		(0x302 | GPIO_MARK | IOCON_REG(0x9c) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_3_2_DCD		(0x302 | GPIO_MARK | IOCON_REG(0x9c) | IOCON_SET(0xc1)) /* + DCDLOC */
#define GPIO_3_3		(0x303 | GPIO_MARK | IOCON_REG(0xac) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_3_3_RI		(0x303 | GPIO_MARK | IOCON_REG(0xac) | IOCON_SET(0xc1)) /* + RILOC */
#define GPIO_3_4		(0x304 | GPIO_MARK | IOCON_REG(0x3c) | IOCON_SET(0xc0|GPIO_HYST))
#define GPIO_3_5		(0x305 | GPIO_MARK | IOCON_REG(0x48) | IOCON_SET(0xc0|GPIO_HYST))


#define GPIO_PORT(pin) (((pin) >> 8) & 0x3)
#define GPIO_PIN(pin) ((pin) & 0x1f)

#define GPIO_PPIN(port, pin) (GPIO_MARK | ((port) << 8) | (pin)) /* port and pin merged */

#endif
