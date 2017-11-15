#ifndef _STDIO_H_
#define _STDIO_H_

#include <init.h>
#include <types.h>
#include <device.h>
#include <stddef.h>
#include <ioctl.h>

#define EOF (-1)

/* FILE -> struct device  for cbaos purposes */
typedef struct device FILE;
// TODO elsewhere
static __inline int fileno(FILE *file)
{ return file-global_fds[0]; }

#if 0
extern int _write(int fd, const void *buf, size_t count);
extern int _read(int fd, char *buf, size_t count);
static __inline int write(int fd, const void *buf, size_t count)
{ return _write(fd, buf, count); }
static __inline int read(int fd, char *buf, size_t count)
{ return _read(fd, buf, count); }
#else
static __inline ssize_t write(int fd, const void *buf, size_t count)
{ return _write(fd, buf, count); }
static __inline ssize_t read(int fd, void *buf, size_t count)
{ return _read(fd, buf, count); }
#endif

#define stdin  global_fds[0]
#define stdout global_fds[1]
#define stderr global_fds[2]


//extern FILE *stdin, *stdout, *stderr;

int fopen(FILE *fd, const char *path, int flags);
void fclose(FILE *fd);
int fwrite(const void *buf, size_t size, size_t count, FILE *f);
//int fwriteall(FILE *fd, const void *buf, size_t count);
int fread(void *buf, size_t size, size_t count, FILE *f);
//int ioctl(FILE *fd, enum ioctl cmd, int arg);

int printf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
int fprintf(FILE *stream, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
int sprintf(char *str, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
int snprintf(char *str, size_t len, const char *format, ...) __attribute__ ((format (printf, 3, 4)));

/* XXX see these actually work */
int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
#define putc(c, s) fputc(c, s)
#define putchar(c) fputc(c, stdout)
int puts(const char *s);

int fgetc(FILE *f);
int getchar(void);
char *fgets(char *s, int size, FILE *f);

#endif
