#ifndef ARCH_CRT_H_
#define ARCH_CRT_H_

#include <compiler.h>
#include <types.h>

linker_var_t _ram_start;
linker_var_t _ram_end;

void reset_handler();

#endif
