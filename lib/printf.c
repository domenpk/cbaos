/*
 * CBA printf 20151112
 * Author: Domen Puncer Kugler <domen@cba.si>
 * License: WTFPL, see file LICENSE
 *
 * printf, sprintf, snprintf implementations
 * The aim is small code size (.text), and dynamic memory consumption/stack.
 *
 * The basic (MODE=1) implementation handles int formats %[diuoxX] and %[scp%].
 * The elaborate one handles the same ones with all the modifiers, flags etc.
 *
 * You probably want to define your own putchar() to print to serial or sth.
 */

/*
 * config options:
 * #define MODE 0 - full printf (w/o floating point)
 * #define MODE 1 - only support the basic %csiduoxXp, no precision, padding, 
 * 	width etc. and maximum int sized numbers
 * #define SIZE 0 - int sized
 * #define SIZE 1 - long sized
 * #define SIZE 2 - long long sized numbers
 *
 * they produce quite different binaries, code size in bytes:
 * arch  SIZE=2 |SIZE=0| MODE=1
 * x86-64: 2315 | 2286 |  923
 * arm:    2095 | 1843 |  632
 * avr:    3136 | 1926 |  544
 */


/* "features"
 * - %.2s for NULL - what does std say?
 * - %c for 0 in MODE=0 it prints nothing.
 */

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>

//#define INT_MAX (0x7fffffff)

#define MODE 0
#define SIZE 0

#if 1
#define _printf printf
#define _fprintf fprintf
#define _sprintf sprintf
#define _snprintf snprintf
#endif


#if SIZE == 0
#define pint int
#elif SIZE == 1
#define pint long
#else /* 2 */
#define pint long long
#endif
#define PLEN (sizeof(pint)*4)


/* this belongs elsewhere */
static int isdigit(unsigned char c)
{
	return (c >= '0' && c <= '9');
}

static void printchar(char **str, int *len, int c)
{
	if (--(*len) > 0) {
		/* this is an ugly hack... strings are <=INT_MAX/2, FILE* > */
		if (*len > INT_MAX/2) {
			FILE *f = (FILE*)str;
			fputc(c, f);
		} else {
			*(*str)++ = c;
		}
	}
}

#ifndef MODE
#define MODE 0
#endif
#if MODE == 0
static void printstr(char **str, int *len, const char *s, int width, char pad, int prec, const char *sbefore)
{
	if (sbefore)
		while (*sbefore && prec-- && width-- > 0)
			printchar(str, len, *sbefore++);

	if (width > 0 && pad != '-') {
		int ilen = width - strlen(s);
		while (ilen-- > 0 && width-- > 0)
			printchar(str, len, pad);
	}
	while (*s && prec--) {
		printchar(str, len, *s++);
		width--;
	}

	if (pad == '-')
		while (width-- > 0)
			printchar(str, len, ' ');
}

static void printint(char **str, int *len, unsigned pint x, int base, char hexlet, int width, int sign, char pad)
{
	char store[PLEN]; /* for 32-bit int: '0'+11 octal digits+'\0 | "0x\0" */
	char *out = &store[PLEN];
	char *outbefore = &store[3];
	char **extrachars;

	*--out = '\0';
	*--outbefore = '\0';
	/* simplifies %# handling */
	if (!x) {
		*--out = '0';
		if (sign == '+' || sign == ' ')
			*--out = sign;
		printstr(str, len, out, width, pad, -1, NULL);
		return;
	}
	do {
		int digit = x % base + '0';
		if (digit >= 10+'0')
			digit += hexlet;

		*--out = digit;
		x /= base;
	} while (x);

	/* 0-padding means '-' or 0x is before padding, for other paddings it's after */
	if (pad == '0')
		extrachars = &outbefore;
	else
		extrachars = &out;

	if (sign == '#') {
		if (base == 16)
			*--*extrachars = 'x'-'0'+9+hexlet;
		sign = '0'; /* next if will handle this */
	}
	if (sign)
		*--*extrachars = sign;

	printstr(str, len, out, width, pad, -1, outbefore);
}

static unsigned scan_number(const char **format)
{
	unsigned n = 0;
	char c = **format;
	while (isdigit(c)) {
		n = n*10 + c-'0';
		(*format)++;
		c = **format;
	}
	return n;
}

int __printf(char **str, int olen, const char *format, va_list va)
{
	char c;
	int len = olen;

	while ((c = *format++)) {
		int width = 0;
		int sign = 0;
		int pad = ' ';
		int prec = -1;

		if (c != '%') {
			printchar(str, &len, c);
			continue;
		}
		do {
			c = *format++;

			if (c == ' ' || c == '+' || c == '#')
				sign = c;
			else if (c == '0' || c == '-')
				pad = c;
			else
				break;
		} while (1);

		if (isdigit(c)) {
			format--;
			width = scan_number(&format);
			c = *format++;
		}
		if (c == '.') {
			prec = scan_number(&format);
			c = *format++;
		}

		if (c == '\0')
			break;
		if (c == '%')
			printchar(str, &len, c);
		else if (c == 's') {
			const char *s = va_arg(va, const char *);
			if (!s)
				s = "(null)";
			printstr(str, &len, s, width, pad, prec, NULL);
		}
		else if (c == 'c') {
			char c[2] = { va_arg(va, int), '\0' };
			printstr(str, &len, c, width, pad, prec, NULL);
		}
		else {
			pint x;
			pint h = 0;
			while (c == 'h') {
				h++;
				c = *format++;
			}
			while (c == 'l') {
				h--;
				c = *format++;
			}
			if (h >= 0)
				x = (unsigned)va_arg(va, int);
			else if (h == -1)
				x = (unsigned long)va_arg(va, long);
			else /*if (h == -2)*/
				x = (unsigned long long)va_arg(va, long long);

			if (c == 'i' || c == 'd') {
				if (h == 0)
					x = (int)x;
				else if (h == 1)
					x = (short int)x;
				else if (h == 2)
					x = (signed char)x;
				else if (h == -1)
					x = (signed long)x;
				else /* -2 */
					x = (signed pint)x;

				if (x < 0) {
					sign = '-';
					x = -x;
				}
				printint(str, &len, x, 10, 0, width, sign, pad);
				continue;
			}
			if (h == 1)
				x = (unsigned short int)x;
			else if (h == 2)
				x = (unsigned char)x;

			if (c == 'u')
				printint(str, &len, x, 10, 0, width, sign, pad);
			else if (c == 'o')
				printint(str, &len, x, 8, 0, width, sign, pad);
			else if (c == 'x')
				printint(str, &len, x, 16, 'a'-'0'-10, width, sign, pad);
			else if (c == 'X')
				printint(str, &len, x, 16, 'A'-'0'-10, width, sign, pad);
			else if (c == 'p')
				printint(str, &len, x, 16, 'a'-'0'-10, width, '#', pad);
		}
	}

	return olen-len;
}
#elif MODE == 1
static void printstr(char **str, int *len, const char *s)
{
	while (*s) {
		printchar(str, len, *s++);
	}
}

static void printint(char **str, int *len, unsigned int x, int base, char hexlet)
{
	char store[PLEN]; /* for 32-bit int: '0'+11 octal digits+'\0 | "0x\0" */
	char *out = &store[PLEN];

	*--out = '\0';
	/* simplifies %# handling */
	if (!x) {
		printchar(str, len, '0');
		return;
	}
	do {
		int digit = x % base + '0';
		if (digit >= 10+'0')
			digit += hexlet;

		*--out = digit;
		x /= base;
	} while (x);

	printstr(str, len, out);
}

int __printf(char **str, int olen, const char *format, va_list va)
{
	char c;
	int len = olen;

	while ((c = *format++)) {
		if (c != '%') {
			printchar(str, &len, c);
			continue;
		}

 ignored_char:
		c = *format++;

		if (c == '\0')
			break;
		if (c == '%')
			printchar(str, &len, c);
		else if (c == 's') {
			const char *s = va_arg(va, const char *);
			if (!s)
				s = "(null)";
			printstr(str, &len, s);
		}
		else if (c == 'c') {
			char c = va_arg(va, int);
			printchar(str, &len, c);
		}
		else {
			int x;
			if (strchr("iduoxXp", c) == NULL)
				goto ignored_char;

			/* needs to be used when evaluated */
			x = (unsigned)va_arg(va, int);

			if (c == 'i' || c == 'd') {
				if (x < 0) {
					printchar(str, &len, '-');
					x = -x;
				}
				c = 'u';
			}

			if (c == 'u')
				printint(str, &len, x, 10, 0);
			else if (c == 'o')
				printint(str, &len, x, 8, 0);
			else if (c == 'x')
				printint(str, &len, x, 16, 'a'-'0'-10);
			else if (c == 'X')
				printint(str, &len, x, 16, 'A'-'0'-10);
			else if (c == 'p') {
				printstr(str, &len, "0x");
				printint(str, &len, x, 16, 'a'-'0'-10);
			}
		}
	}

	return olen-len;
}
#endif

int _printf(const char *format, ...)
{
	int r;
	va_list va;
	va_start(va, format);
	r = __printf((char**)stdout, INT_MAX, format, va);
	va_end(va);
	return r;
}

/* maybe also somewhat handle stream etc. */
int _fprintf(FILE *stream, const char *format, ...)
{
	int r;
	va_list va;
	va_start(va, format);
	r = __printf((char**)stream, INT_MAX, format, va);
	va_end(va);
	return r;
}

int _sprintf(char *str, const char *format, ...)
{
	int r;
	va_list va;
	va_start(va, format);
	r = __printf(&str, INT_MAX/2, format, va);
	va_end(va);
	*str = '\0';
	return r;
}

int _snprintf(char *str, size_t len, const char *format, ...)
{
	int r;
	va_list va;
	va_start(va, format);
	r = __printf(&str, len, format, va);
	va_end(va);
	*str = '\0';
	return r;
}
