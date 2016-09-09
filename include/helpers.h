#ifndef HELPERS_H_
#define HELPERS_H_

#include <types.h>
#include <endianess.h>

#define ALEN(x) (sizeof(x)/sizeof((x)[0]))

void print_array(const char *tag, const u8 *buf, size_t size);

#endif
