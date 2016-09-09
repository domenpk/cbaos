#ifndef _LOCK_H_
#define _LOCK_H_

#include <arch/lock.h>

#define LOCK_DECLARE_INIT(name)	\
	struct lock (name) = LOCK_INIT

#endif
