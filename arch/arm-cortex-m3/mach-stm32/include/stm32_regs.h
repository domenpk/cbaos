#ifndef _STM32_REGS_H_
#define _STM32_REGS_H_

#include <types.h>

#define STM32_FLASH_BASE   0x40022000
#define STM32_RCC_BASE     0x40021000
#define STM32_USART1_BASE  0x40013800
#define STM32_GPIO_BASE(x) (0x40010800+0x400*(x))
#define STM32_EXTI_BASE    0x40010400
#define STM32_AFIO_BASE    0x40010000
#define STM32_PWR_BASE     0x40007000
#define STM32_I2C1_BASE    0x40005400
#define STM32_I2C2_BASE    0x40005800


struct stm32_i2c_regs {
	volatile u32 CR1;
	volatile u32 CR2;
	volatile u32 OAR1;
	volatile u32 OAR2;
	volatile u32 DR;
	volatile u32 SR1;
	volatile u32 SR2;
	volatile u32 CCR;
	volatile u32 TRISE;
};
#define STM32_I2C1 ((struct stm32_i2c_regs*)STM_I2C1_BASE)
#define STM32_I2C2 ((struct stm32_i2c_regs*)STM_I2C2_BASE)

struct stm32_rcc_regs {
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
#define STM32_RCC ((struct stm32_rcc_regs*)STM32_RCC_BASE)

struct stm32_gpio_regs {
	volatile u32 CRL;
	volatile u32 CRH;
	volatile u32 IDR;
	volatile u32 ODR;
	volatile u32 BSRR;
	volatile u32 BRR;
	volatile u32 LCKR;
};
#define STM32_GPIO(x) ((struct stm32_gpio_regs*)STM32_GPIO_BASE(x))

struct stm32_exti_regs {
	volatile u32 IMR;
	volatile u32 EMR;
	volatile u32 RTSR;
	volatile u32 FTSR;
	volatile u32 SWIER;
	volatile u32 PR;
};
#define STM32_EXTI ((struct stm32_exti_regs*)STM32_EXTI_BASE)

struct stm32_afio_regs {
	volatile u32 EVCR;
	volatile u32 MAPR;
	volatile u32 EXTICR1;
	volatile u32 EXTICR2;
	volatile u32 EXTICR3;
	volatile u32 EXTICR4;
};
#define STM32_AFIO ((struct stm32_afio_regs*)STM32_AFIO_BASE)

struct stm32_usart_regs {
	volatile u32 SR;
	volatile u32 DR;
	volatile u32 BRR;
	volatile u32 CR1;
	volatile u32 CR2;
	volatile u32 CR3;
	volatile u32 GTPR;
};
#define STM32_USART1 ((struct stm32_usart_regs*)STM32_USART1_BASE)

#define FLASH_ACR *(volatile u32 *)0x40022000

#define APB1_DAC        (1<<29)
#define APB1_PWR        (1<<28)
#define APB1_BKP        (1<<27)
#define APB1_CAN        (1<<25)
#define APB1_USB        (1<<23)
#define APB1_I2C2       (1<<22)
#define APB1_I2C1       (1<<21)
#define APB1_UART5      (1<<20)
#define APB1_UART4      (1<<19)
#define APB1_USART3     (1<<18)
#define APB1_USART2     (1<<17)
#define APB1_SPI3       (1<<15)
#define APB1_SPI2       (1<<14)
#define APB1_WWDG       (1<<11)
#define APB1_TIM7       (1<<5)
#define APB1_TIM6       (1<<4)
#define APB1_TIM5       (1<<3)
#define APB1_TIM4       (1<<2)
#define APB1_TIM3       (1<<1)
#define APB1_TIM2       (1<<0)

#define APB2_ADC3       (1<<15)
#define APB2_USART1     (1<<14)
#define APB2_TIM8       (1<<13)
#define APB2_SPI1       (1<<12)
#define APB2_TIM1       (1<<11)
#define APB2_ADC2       (1<<10)
#define APB2_ADC1       (1<<9)
#define APB2_IOPG       (1<<8)
#define APB2_IOPF       (1<<7)
#define APB2_IOPE       (1<<6)
#define APB2_IOPD       (1<<5)
#define APB2_IOPC       (1<<4)
#define APB2_IOPB       (1<<3)
#define APB2_IOPA       (1<<2)
#define APB2_AFIO       (1<<0)


#define REMAP_USART2    (1<<3)
#define REMAP_USART1    (1<<2)
#define REMAP_I2C1      (1<<1)
#define REMAP_SPI1      (1<<0)

enum STM32_IRQ {
	IRQ_WWDG = 0,
	IRQ_PVD = 1,
	IRQ_TAMPER = 2,
	IRQ_RTC = 3,
	IRQ_FLASH = 4,
	IRQ_RCC = 5,
	IRQ_EXTI0 = 6,
	IRQ_EXTI1 = 7,
	IRQ_EXTI2 = 8,
	IRQ_EXTI3 = 9,
	IRQ_EXTI4 = 10,
	IRQ_DMA1_CH1 = 11,
	IRQ_DMA1_CH2 = 12,
	IRQ_DMA1_CH3 = 13,
	IRQ_DMA1_CH4 = 14,
	IRQ_DMA1_CH5 = 15,
	IRQ_DMA1_CH6 = 16,
	IRQ_DMA1_CH7 = 17,
	IRQ_ADC1_2 = 18,
	IRQ_CAN1_TX = 19,
	IRQ_CAN1_RX0 = 20,
	IRQ_CAN1_RX1 = 21,
	IRQ_CAN1_SCE = 22,
	IRQ_EXTI9_5 = 23,
	IRQ_TIM1_BRK = 24,
	IRQ_TIM1_UP = 25,
	IRQ_TIM1_TRG_COM = 26,
	IRQ_TIM1_CC = 27,
	IRQ_TIM2 = 28,
	IRQ_TIM3 = 29,
	IRQ_TIM4 = 30,
	IRQ_I2C1_EV = 31,
	IRQ_I2C1_ER = 32,
	IRQ_I2C2_EV = 33,
	IRQ_I2C2_ER = 34,
	IRQ_SPI1 = 35,
	IRQ_SPI2 = 36,
	IRQ_USART1 = 37,
	IRQ_USART2 = 38,
	IRQ_USART3 = 39,
	IRQ_EXTI15_10 = 40,
	IRQ_RTC_ALARM = 41,
	IRQ_OTG_FS_WKUP = 42,

	IRQ_TIM5 = 50,
	IRQ_SPI3 = 51,
	IRQ_UART4 = 52,
	IRQ_UART5 = 53,
	IRQ_TIM6 = 54,
	IRQ_TIM7 = 55,
	IRQ_DMA2_CH1 = 56,
	IRQ_DMA2_CH2 = 57,
	IRQ_DMA2_CH3 = 58,
	IRQ_DMA2_CH4 = 59,
	IRQ_DMA2_CH5 = 60,
	IRQ_ETH = 61,
	IRQ_ETH_WKUP = 62,
	IRQ_CAN2_TX = 63,
	IRQ_CAN2_RX0 = 64,
	IRQ_CAN2_RX1 = 65,
	IRQ_CAN2_SCE = 66,
	IRQ_OTG_FS = 67,
};

#endif
