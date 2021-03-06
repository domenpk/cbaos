#ifndef _ARCH_CRT_H_
#define _ARCH_CRT_H_

#include <compiler.h>
#include <types.h>

linker_var_t _ram_start;
linker_var_t _ram_end;

void reset_handler(void);
void unknown_handler(void);
void nmi_handler(void);
void hardfault_handler(void);
void svc_handler(void);
void pendsv_handler(void);
void systick_handler(void);

void __naked_asm generic_exception_handler(void);

#endif
