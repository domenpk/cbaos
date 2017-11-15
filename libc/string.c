/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <string.h>
#include <ctype.h>

char *strcpy(char *dest, const char *src)
{
	char *tmp = dest;

		while (*src)
		*dest++ = *src++;
	*dest = '\0';

	return tmp;
}

//char *strncpy(char *dest, const char *src);

size_t strlen(const char *s)
{
	int n = 0;
	while (*s++)
		n++;
	return n;
}

// XXX test it actually works
int strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2) {
		if (*s1 == '\0')
			return 0;
		s1++; s2++;
	}
	return *s1-*s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return 0;

	while (*s1 == *s2) {
		if (*s1 == '\0')
			return 0;
		s1++; s2++;
		if (--n == 0)
			return 0;
	}
	return *s1-*s2;
}

int strcasecmp(const char *s1, const char *s2)
{
	while (tolower(*s1) == tolower(*s2)) {
		if (*s1 == '\0')
			return 0;
		s1++; s2++;
	}
	return tolower(*s1)-tolower(*s2);
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
	while (n && tolower(*s1) == tolower(*s2)) {
		if (*s1 == '\0')
			return 0;
		s1++; s2++;
		n--;
	}
	if (n == 0)
		return 0;
	return tolower(*s1)-tolower(*s2);
}

char *strchr(const char *_s, int c)
{
	char *s = (char *)_s;

	while (*s) {
		if (*s == (char)c)
			return s;
		s++;
	}
	return NULL;
}

int memcmp(const void *_s1, const void *_s2, size_t n)
{
	const char *s1 = _s1;
	const char *s2 = _s2;

	if (n == 0)
		return 0;

	while (*s1 == *s2) {
		s1++; s2++;
		if (--n == 0)
			return 0;
	}
	return *s1-*s2;
}

void *memmove(void *_dest, const void *_src, size_t n)
{
	char *dest = _dest;
	const char *src = _src;

	/* overlapping case */
	if (dest > src && dest < src+n) {
		do {
			n--;
			dest[n] = src[n];
		} while (n > 0);
		return _dest;
	}

	while (n--)
		*dest++ = *src++;

	return _dest;
}

void *memcpy(void *_dest, const void *_src, size_t n)
{
	char *dest = _dest;
	const char *src = _src;

	while (n--)
		*dest++ = *src++;

	return _dest;
}

void *memset(void *_s, int c, size_t n)
{
	char *s = _s;

	while (n--)
		*s++ = c;

	return _s;
}
