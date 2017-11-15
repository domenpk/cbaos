#ifndef TYPES_H_
#define TYPES_H_

#include <stddef.h> /* size_t and so on */
#ifdef LIBC_NEWLIB
#include <sys/types.h>
#endif
#ifdef LIBC_GLIBC
#include <sys/unistd.h>
#endif

#ifndef offsetof
#define offsetof(type, member) (unsigned)(&((type*)0)->member)
#endif

#define linker_var_t extern struct not_defined

#ifndef u8
#define u8 unsigned char
#endif
#ifndef u16
#define u16 unsigned short
#endif
#ifndef u32
#define u32 unsigned int
#endif
#ifndef u64
#define u64 unsigned long long
#endif

#ifndef s8
#define s8 signed char
#endif
#ifndef s16
#define s16 signed short
#endif
#ifndef s32
#define s32 signed int
#endif
#ifndef s64
#define s64 signed long long
#endif

#endif
