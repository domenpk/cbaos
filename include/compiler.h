#ifndef _COMPILER_H_
#define _COMPILER_H_

/* only gcc stuff for now */
#define __alias(f)	__attribute__((weak, alias (#f)))
#define __naked		__attribute__((naked))
#define __interrupt	__attribute__((interrupt))
#define __noreturn	__attribute__((noreturn))

#define ALEN(x) (sizeof(x)/sizeof((x)[0]))

#endif
