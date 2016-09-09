/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <errno.h>
#include <string.h>

#include <device.h>
#include <driver.h>
#include <stdio.h>

FILE *stdin, *stdout, *stderr;


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

void fclose(FILE *fd)
{
	fd->dev->drv->close(fd->dev);
	fd->dev = NULL;
}

int fwrite(FILE *fd, const void *buf, size_t count)
{
	return fd->dev->drv->write(fd->dev, buf, count);
}

int fwriteall(FILE *fd, const void *buf, size_t count)
{
	int r;
	int size = count;

	do {
		r = fd->dev->drv->write(fd->dev, buf, count);
		if (r <= 0)
			return r;
		count -= r;
		buf += r;
	} while (count > 0);

	return size;
}

int fread(FILE *fd, void *buf, size_t count)
{
	return fd->dev->drv->read(fd->dev, buf, count);
}

int ioctl(FILE *fd, enum ioctl cmd, int arg)
{
	if (fd->dev->drv->ioctl)
		return fd->dev->drv->ioctl(fd->dev, cmd, arg);
	return -ENOSYS;
}

int fputc(int c, FILE *f)
{
	// TODO is this the right place? prolly not :P
	if (c == '\n') {
		int r = '\r';
		fwrite(f, &r, 1);
	}
	if (fwrite(f, &c, 1) == 1)
		return c;
	return EOF;
}

int fgetc(FILE *f)
{
	char c;
	if (fread(f, &c, 1) == 1)
		return c;
	return EOF;
}

int getchar(void)
{
	return fgetc(stdin);
}

char *fgets(char *s, int size, FILE *f)
{
	int pos = 0;
	if (size <= 0)
		return NULL;

	while (pos < size-1) {
		int r = fread(f, &s[pos], 1);

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
