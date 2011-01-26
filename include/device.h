#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <list.h>

struct driver;
struct device {
	struct list list;
	const char *name;
	struct driver *drv;
	void *priv; /* private data for device */
};

int device_register(struct device *dev);
void device_unregister(struct device *dev);
struct device *device_find(const char *name);

#endif
