#ifndef _STM32_REGS_H_
#define _STM32_REGS_H_

#include <types.h>

#define STM32_FLASH_BASE   0x40022000
#define STM32_RCC_BASE     0x40021000
#define STM32_USART1_BASE  0x40013800
#define STM32_GPIO_BASE(x) (0x40010800+0x400*(x))
#define STM32_AFIO_BASE    0x40010000
#define STM32_PWR_BASE     0x40007000


struct stm32_rcc {
	volatile u32 CR;
	volatile u32 CFGR;
	volatile u32 CIR;
	volatile u32 APB2RSTR;
	volatile u32 APB1RSTR;
	volatile u32 APBHENR;
	volatile u32 APB2ENR;
	volatile u32 APB1ENR;
	volatile u32 DBCR;
	volatile u32 CSR;
};
#define STM32_RCC ((struct stm32_rcc*)STM32_RCC_BASE)

struct stm32_gpio {
	volatile u32 CRL;
	volatile u32 CRH;
	volatile u32 IDR;
	volatile u32 ODR;
	volatile u32 BSRR;
	volatile u32 BRR;
	volatile u32 LCKR;
};
#define STM32_GPIO(x) ((struct stm32_gpio*)STM32_GPIO_BASE(x))

struct stm32_afio {
	volatile u32 EVCR;
	volatile u32 MAPR;
	volatile u32 EXTICR1;
	volatile u32 EXTICR2;
	volatile u32 EXTICR3;
	volatile u32 EXTICR4;
};

#define FLASH_ACR *(volatile u32 *)0x40022000

#endif
