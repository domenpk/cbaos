#ifndef STRING_H_
#define STRING_H_

#include <stddef.h>

char *strcpy(char *dest, const char *src);
//char *strncpy(char *dest, const char *src);
size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
char *strchr(const char *_s, int c);
int memcmp(const void *_s1, const void *_s2, size_t n);
void *memmove(void *_dest, const void *_src, size_t n);
void *memcpy(void *_dest, const void *_src, size_t n);
void *memset(void *_s, int c, size_t n);

#endif
