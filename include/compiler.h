#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <helpers.h>

/* only gcc stuff for now */
#define __alias(f)	__attribute__((weak, alias (#f)))

#ifndef __naked_asm
#define __naked_asm	__attribute__((naked))
#endif

#ifndef __interrupt
#define __interrupt	__attribute__((interrupt))
#endif

#ifndef __noreturn
#define __noreturn	__attribute__((noreturn))
#endif

#ifndef __unused
#define __unused	__attribute__((unused))
#endif

#ifndef __inline
#define __inline	__attribute__((always_inline)) inline
#endif

#ifndef __packed
#define __packed	__attribute__((packed))
#endif

#endif
