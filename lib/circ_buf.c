/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <circ_buf.h>
#include <string.h>
#include <errno.h>

void circ_buf_init(struct circ_buf *cb, u8 *buf, unsigned size)
{
	cb->data = buf;
	cb->size = size;
	cb->head = cb->tail = 0;
}

unsigned circ_buf_len(struct circ_buf *cb)
{
	unsigned tmp;
	tmp = cb->size + cb->head - cb->tail;
	if (tmp >= cb->size)
		tmp -= cb->size;
	return tmp;
}

unsigned circ_buf_put(struct circ_buf *cb, const u8 *data, unsigned len)
{
	unsigned size;
	unsigned len_til_end;
	
	size = cb->size - 1 - circ_buf_len(cb);
	if (len > size)
		len = size;

	len_til_end = cb->size - cb->head;
	if (len_til_end > len)
		len_til_end = len;

	memcpy(cb->data+cb->head, data, len_til_end);
	memcpy(cb->data, data+len_til_end, len-len_til_end);

	cb->head += len;
	if (cb->head >= cb->size)
		cb->head -= cb->size;
	return len;
}

unsigned circ_buf_get(struct circ_buf *cb, u8 *data, unsigned len)
{
	unsigned size;
	unsigned len_til_end;
	
	size = circ_buf_len(cb);
	if (len > size)
		len = size;

	len_til_end = cb->size - cb->tail;
	if (len_til_end > len)
		len_til_end = len;

	memcpy(data, cb->data+cb->tail, len_til_end);
	memcpy(data+len_til_end, cb->data, len-len_til_end);

	cb->tail += len;
	if (cb->tail >= cb->size)
		cb->tail -= cb->size;
	return len;
}

/* for convenience */
int circ_buf_put_one(struct circ_buf *cb, u8 data)
{
	if (circ_buf_put(cb, &data, 1) == 1)
		return 0;
	return -EAGAIN;
}

int circ_buf_get_one(struct circ_buf *cb)
{
	u8 data;
	if (circ_buf_get(cb, &data, 1) == 1)
		return data;
	return -EINVAL;
}
