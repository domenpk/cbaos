#ifndef _ARCH_CRT_H_
#define _ARCH_CRT_H_

#include <compiler.h>

extern void _ram_end;

void __naked reset_handler();
void __naked unknown_handler();
void __naked nmi_handler();
void __naked hardfault_handler();
void __naked memmanage_handler();
void __naked busfault_handler();
void __naked usagefault_handler();
void __naked svc_handler();
void __naked debugmon_handler();
void __naked pendsv_handler();
void __naked systick_handler();

#endif
