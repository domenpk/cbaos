#ifndef _STDIO_H_
#define _STDIO_H_

#include <stddef.h>

#define EOF (-1)

#define O_NONBLOCK 1

struct _FILE {
	struct device *dev;
};

typedef struct _FILE FILE;

extern FILE *stdin, *stdout, *stderr;


int fopen(FILE *fd, const char *path, int flags);
void fclose(FILE *fd);
int fwrite(FILE *fd, const void *buf, size_t count);
int fread(FILE *fd, void *buf, size_t count);


int printf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
int fprintf(FILE *stream, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
int sprintf(char *str, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
int snprintf(char *str, size_t len, const char *format, ...) __attribute__ ((format (printf, 3, 4)));

// implement scanf

/* XXX see these actually work */
int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
#define putc(c, s) fputc(c, s)
#define putchar(c) fputc(c, stdout)
int puts(const char *s);

int fgetc(FILE *f);
int getchar(void);

#endif
