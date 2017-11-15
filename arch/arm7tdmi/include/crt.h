#ifndef _ARCH_CRT_H_
#define _ARCH_CRT_H_

#include <compiler.h>
#include <types.h>

linker_var_t _ram_start;
linker_var_t _ram_end;

void reset_handler(void);
void undefined_handler(void);
void swi_handler(void);
void dabort_handler(void);
void pabort_handler(void);
void fiq_handler(void);

void generic_exception_handler(void);

#endif
