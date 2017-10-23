#ifndef _ARCH_WDT_H_
#define _ARCH_WDT_H_

#include <types.h>

struct lpc13xx_wdt {
	volatile u32 WDMOD;     /* 0x0 */
	volatile u32 WDTC;      /* 0x4 */
	volatile u32 WDFEED;    /* 0x8 */
	volatile const u32 WDTV;/* 0xc */
};

#define WDT_ADDR 0x40004000

#define lpc13xx_wdt ((struct lpc13xx_wdt*)WDT_ADDR)

#endif
