#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <stddef.h>
#include <ioctl.h>
#include <fcntl.h>

struct device;
struct driver {
	int (*probe)(struct device *, void *data);
	void (*shutdown)(struct device *);
	int (*open)(struct device *, int flags);
	void (*close)(struct device *);
	int (*read)(struct device *, void *, size_t);
	int (*write)(struct device *, const void *, size_t);
	int (*ioctl)(struct device *, enum ioctl, int arg);
};

#endif
