#ifndef _ASSERT_H_
#define _ASSERT_H_

#define assert(expr) \
	do { \
		if (!(expr)) { \
			printf("%s:%i, assert %s failed\n", __func__, __LINE__, #expr); \
		} \
	} while (0)

#endif
