#ifndef _LPC21XX_H_
#define _LPC21XX_H_

void debug_uart_init(int divider);
void setup_pll(int m, int p);

extern u32 clock_pclk;

#endif
