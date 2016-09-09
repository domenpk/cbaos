#ifndef _ARCH_TMR_H_
#define _ARCH_TMR_H_

#include <types.h>

struct lpc13xx_tmr {
	volatile u32 IR;        /* 0x0 */
	volatile u32 TCR;       /* 0x4 */
	volatile u32 TC;        /* 0x8 */
	volatile u32 PR;        /* 0xc */

	volatile u32 PC;        /* 0x10 */
	volatile u32 MCR;       /* 0x14 */
	union {
		struct {
			volatile u32 MR0;       /* 0x18 */
			volatile u32 MR1;       /* 0x1c */

			volatile u32 MR2;       /* 0x20 */
			volatile u32 MR3;       /* 0x24 */
		};
		volatile u32 MR[4];
	};
	volatile u32 CCR;       /* 0x28 */
	volatile const u32 CR0; /* 0x2c */
	u32 _reserved0[3];
	volatile u32 EMR;       /* 0x3c */
	u32 _reserved1[12];
	volatile u32 CTCR;      /* 0x70 */
	volatile u32 PWMC;      /* 0x74 */
};

#define TMR16B0_ADDR 0x4000c000
#define TMR16B1_ADDR 0x40010000
#define TMR32B0_ADDR 0x40014000
#define TMR33B1_ADDR 0x40018000

#endif
