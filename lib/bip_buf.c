/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <bip_buf.h>
/* googling a bit about this reveals this is called Bip Buffer. reinvent 13y after, yay :P */


void bip_buf_init(struct bip_buf *cb, void *buf, unsigned size)
{
	cb->data = buf;
	cb->size = cb->tailwrap = size;
	cb->head = cb->tail = 0;
}

void* bip_buf_alloc(struct bip_buf *cb, unsigned len)
{
	/* reinit, so it's less chance of len >= cb->size failing */
	// this is racy
//	if (cb->head == cb->tail)
//		bip_buf_init(cb, cb->data, cb->size);

	unsigned tmp; /* how much is available? */
	unsigned newh = cb->head;
	unsigned tail = cb->tail; /* tail is only read once, consumer could increase it, but that doesn't affect us - no race */

	if (cb->head >= tail) {
		tmp = cb->size - cb->head - 1;
		if (tmp < len) {
			if (tail == 0)
				return NULL;
			tmp = tail - 1;
			newh = 0;
		}
	} else {
		tmp = tail - cb->head - 1;
	}
	if (len > tmp)
		return NULL;

	return &cb->data[newh];
}

/* len here must be exactly the same as for _alloc() */
void bip_buf_alloc_commit(struct bip_buf *cb, unsigned len)
{
	unsigned newh = cb->head + len;
	if (newh >= cb->size) {
		newh = len; /* occupy 0->len */
		cb->tailwrap = cb->head;
	}
	cb->head = newh;
}

void* bip_buf_getdata(struct bip_buf *cb)
{
	if (cb->head == cb->tail)
		return NULL;
	if (cb->tail == cb->tailwrap) {
		/* tailwrap should not be racy, since either producer or
		 * consumer is at cb->size->0 transition, both can't be.
		 */
		cb->tail = 0;
		cb->tailwrap = cb->size;
	}
	return &cb->data[cb->tail];
}

void bip_buf_free(struct bip_buf *cb, unsigned len)
{
	unsigned newt = cb->tail + len;
	cb->tail = newt;
}
