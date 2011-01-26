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

int fread(FILE *fd, void *buf, size_t count)
{
	return fd->dev->drv->read(fd->dev, buf, count);
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

// TODO fgets
