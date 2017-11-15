/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <errno.h>
#include <string.h>

#include <device.h>
#include <driver.h>
#include <stdio.h>


#if 0
int fopen(FILE *fd, const char *path, int flags)
{
	if (fd == NULL) {
		printf("%s, fd == NULL, path:%s\n", __func__, path);
		return -EINVAL;
	}

	if (strncmp(path, "/dev/", 5) == 0) {
		struct device *dev;

		dev = device_find(path+5);
		if (!dev)
			return -ENODEV;
		fd->dev = dev;
		dev->flags = flags;
		return dev->drv->open(dev, flags);
	}
	return -EINVAL;
}
#endif

void fclose(FILE *fd)
{
	fd->drv->close(fd);
	fd = NULL;
}

int fwrite(const void *buf, size_t size, size_t count, FILE *fd)
{
	return fd->drv->write(fd, buf, size*count);
}

#if 0
int fwriteall(FILE *fd, const void *buf, size_t count)
{
	int r;
	int size = count;

	do {
		r = fd->drv->write(fd, buf, count);
		if (r <= 0)
			return r;
		count -= r;
		buf += r;
	} while (count > 0);

	return size;
}
#endif

int fread(void *buf, size_t size, size_t count, FILE *fd)
{
	return fd->drv->read(fd, buf, count);
}

#if 0
int ioctl(FILE *fd, enum ioctl cmd, int arg)
{
	if (fd->drv->ioctl)
		return fd->drv->ioctl(fd, cmd, arg);
	return -ENOSYS;
}
#endif

int fputc(int c, FILE *f)
{
	// TODO is this the right place? prolly not :P
	if (c == '\n') {
		int r = '\r';
		fwrite(&r, 1, 1, f);
	}
	if (fwrite(&c, 1, 1, f) == 1)
		return c;
	return EOF;
}

int fgetc(FILE *f)
{
	char c;
	if (fread(&c, 1, 1, f) == 1)
		return c;
	return EOF;
}

int getchar(void)
{
	return fgetc(global_fds[0]);
}

char *fgets(char *s, int size, FILE *f)
{
	int pos = 0;
	if (size <= 0)
		return NULL;

	while (pos < size-1) {
		int r = fread(&s[pos], 1, 1, f);

		/* EOF */
		if (r == 0)
			break;
		if (r < 0)
			continue;

		if (s[pos++] == '\n')
			break;
	}
	s[pos] = '\0';
	return s;
}
