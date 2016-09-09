#ifndef BIP_BUF_H_
#define BIP_BUF_H_
/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */

#include <types.h>

struct bip_buf {
	u8 *data;
	unsigned size;
	volatile unsigned head; /* points to first free byte */
	volatile unsigned tail; /* points to first used byte */
	volatile unsigned tailwrap;
	/* extreme case tail == head: all free;
	 * tail == head+1: all full (last byte can't be used)
	 * ____tailxxxxxxxxhead_____
	 * xxxxhead________tailxxxxx
	 */
};

void bip_buf_init(struct bip_buf *cb, void *buf, unsigned size);

void* bip_buf_alloc(struct bip_buf *cb, unsigned len);
/* len here must be exactly the same as for _alloc() */
void bip_buf_alloc_commit(struct bip_buf *cb, unsigned len);

void* bip_buf_getdata(struct bip_buf *cb);
void bip_buf_free(struct bip_buf *cb, unsigned len);

#endif
