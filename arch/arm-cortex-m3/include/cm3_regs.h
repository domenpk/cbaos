#ifndef _CM3_REGS_
#define _CM3_REGS_

#include <types.h>

struct DWT {
	volatile u32 CTRL;      /* 0x00 */
	volatile u32 CYCCNT;
	volatile u32 CPICNT;
	volatile u32 EXCCNT;
	volatile u32 SLEEPCNT;  /* 0x10 */
	volatile u32 LSUCNT;
	volatile u32 FOLDCNT;
	volatile u32 PCSR;
	volatile u32 COMP0;     /* 0x20 */
	volatile u32 MASK0;
	volatile u32 FUNCTION0;
	volatile u32 _dummy0;
	volatile u32 COMP1;     /* 0x30 */
	volatile u32 MASK1;
	volatile u32 FUNCTION1;
	volatile u32 _dummy1;
	volatile u32 COMP2;     /* 0x40 */
	volatile u32 MASK2;
	volatile u32 FUNCTION2;
	volatile u32 _dummy2;
	volatile u32 COMP3;     /* 0x50 */
	volatile u32 MASK3;
	volatile u32 FUNCTION3;
	volatile u32 _dummy3;
};

struct SYSTICK {
	volatile u32 CTRL;      /* 0x00 */
	volatile u32 RELOAD;
	volatile u32 VALUE;
	volatile u32 CALIB;
};

struct NVIC {
	volatile u32 ISER[16];  /* 0xe000e100 */
	volatile u32 _dummy0[16];
	volatile u32 ICER[16];  /* 0xe000e180 */
	volatile u32 _dummy1[16];
	volatile u32 ISPR[16];  /* 0xe000e200 */
	volatile u32 _dummy2[16];
	volatile u32 ICPR[16];  /* 0xe000e280 */
	volatile u32 _dummy3[16];
	volatile u32 IABR[16];  /* 0xe000e300 - datasheet is wrong about the size? */
	volatile u32 _dummy4[16+32];
	volatile u32 IPR[124];  /* 0xe000e400 */
};

#define DWT        ((struct DWT *)0xe0001000)

#define SYSTICK ((struct SYSTICK*)0xe000e010)
#define NVIC      ((struct NVIC *)0xe000e100)

/* NVIC System Control Block */
#define ICSR    *((volatile u32 *)0xe000ed04)
#define ICSR_PENDSVSET  (1<<28)
#define ICSR_PENDSVCLR  (1<<27)
#define ICSR_PENDSTSET  (1<<26)
#define ICSR_PENDSTCLR  (1<<25)

#define AIRCR   *((volatile u32 *)0xe000ed0c)

#define CFSR    *((volatile u32 *)0xe000ed28)
#define HFSR    *((volatile u32 *)0xe000ed2c)
#define AIRCR   *((volatile u32 *)0xe000ed0c)
#define DFSR    *((volatile u32 *)0xe000ed30)
#define MMAR    *((volatile u32 *)0xe000ed34)
#define BFAR    *((volatile u32 *)0xe000ed38)
#define AFSR    *((volatile u32 *)0xe000ed3c)

#define DEMCR   *((volatile u32 *)0xe000edfc)
#define DEMCR_TRCENA    (1<<24)

#endif
