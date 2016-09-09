/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stddef.h>
#include <string.h>

#include <device.h>

static LIST_DECLARE_INIT(device_list);

int device_register(struct device *dev)
{
	list_add_tail(&device_list, &dev->list);
	return 0;
}

void device_unregister(struct device *dev){
	list_del(&dev->list);
}

struct device *device_find(const char *name)
{
	struct list *it;
	list_for_each(&device_list, it) {
		struct device *dev = list_entry(it, struct device, list);
		if (strcmp(name, dev->name) == 0)
			return dev;
	}
	return NULL;
}
