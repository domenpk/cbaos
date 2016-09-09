#ifndef _LPC11XX_REGS_
#define _LPC11XX_REGS_

/* include what comes with CMSIS */
#include <mach/LPC11xx.h>

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


#define SYSAHBCLKCTRL_SYS        (1<<0)
#define SYSAHBCLKCTRL_ROM        (1<<1)
#define SYSAHBCLKCTRL_RAM        (1<<2)
#define SYSAHBCLKCTRL_FLASHREG   (1<<3)
#define SYSAHBCLKCTRL_FLASHARRAY (1<<4)
#define SYSAHBCLKCTRL_I2C        (1<<5)
#define SYSAHBCLKCTRL_GPIO       (1<<6)
#define SYSAHBCLKCTRL_CT16B0     (1<<7)
#define SYSAHBCLKCTRL_CT16B1     (1<<8)
#define SYSAHBCLKCTRL_CT32B0     (1<<9)
#define SYSAHBCLKCTRL_CT32B1     (1<<10)
#define SYSAHBCLKCTRL_SSP        (1<<11)
#define SYSAHBCLKCTRL_UART       (1<<12)
#define SYSAHBCLKCTRL_ADC        (1<<13)
//#define SYSAHBCLKCTRL_USB_REG    (1<<14)
#define SYSAHBCLKCTRL_WDT        (1<<15)
#define SYSAHBCLKCTRL_IOCON      (1<<16)
#define SYSAHBCLKCTRL_CAN        (1<<17)
#define SYSAHBCLKCTRL_SSP1       (1<<18)

#endif
