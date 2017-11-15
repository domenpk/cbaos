/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <ctype.h>
#include <string_extra.h>

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

