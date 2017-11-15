#ifndef INIT_H_
#define INIT_H_

#include <cbaos.h>

#define MAX_FDS 3 /* TODO add full support some day */
extern struct device *global_fds[MAX_FDS];

void init(void);
ssize_t _write(int fd, const void *buf, size_t count);
ssize_t _read(int fd, void *buf, size_t count);

#endif
