/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <errno.h>
#include <types.h>
#include <sem.h>
#include <drivers/i2c.h>


void i2c_state_machine(struct i2c_master *master, int state)
{
	struct i2c_transfer *tr = master->current_transfer;

//	printf("%s, state:%x\n", __func__, state);
	switch (state) {
	case 0x08: /* start condition has been transmitted */
	case 0x10: /* repeadet start has been transmitted */
		master->write(master, tr->addr);
		master->pos = 0;
		break;
	case 0x20: /* sla+w has been transmitted, not ack has been received */
	case 0x30: /* data has been transmitted, not ack has been received */
//		printf("%s, nack received while writing\n", __func__);
		master->xfer_error = -EINVAL;
		goto end_transfer;
		break;
	case 0x18: /* sla+w has been transmitted */
	case 0x28: /* data has been transmitted */
		if (master->pos < tr->len)
			master->write(master, tr->data[master->pos++]);
		else
			goto next_transfer;
		break;
	case 0x38: /* artibration lost while sending sla+r/w or data bytes */
		printf("%s, arbitration lost\n", __func__);
		master->xfer_error = -ENXIO;
		goto end_transfer;
		break;
	case 0x48: /* sla+r has been transmitted, not ack has been received */
//		printf("%s, nack received while sla+r\n", __func__);
		master->xfer_error = -EINVAL;
		goto end_transfer;
		break;

	case 0x50: /* data has been received, ack has been returned */
	case 0x58: /* data has been received, not ack has been returned */
		tr->data[master->pos++] = master->read(master);
	case 0x40: /* sla+r has been transmitted */
		if (master->pos < tr->len)
			master->ack(master, 1); /* ack when next byte is received */
		else
			goto next_transfer;
		break;
	}

	return;

 next_transfer:
	master->transfers_to_go--;
	if (master->transfers_to_go) {
		master->current_transfer++;
		master->restart(master);
	} else {
		master->xfer_error = 0;
		goto end_transfer;
	}
	return;

 end_transfer:
	master->stop(master);
	up(&master->xfer_sem);
	return;
}

int i2c_xfer(struct i2c_master *master, struct i2c_transfer *transfer, int num_xfers)
{
	if (down(&master->sem, ms2ticks(1000)) < 0) {
		printf("%s, timeouted\n", __func__);
		return -EAGAIN;
	}

	sem_init(&master->xfer_sem, 0);
	master->current_transfer = transfer;
	master->transfers_to_go = num_xfers;

	/* XXX this shouldn't be needed, but 1w bridge at 0x1b seems to cause 0x1c_w to timeout */
	master->init(master);

	master->start(master);

	if (down(&master->xfer_sem, ms2ticks(1000)) < 0) {
		printf("%s, timeouted, hw error?\n", __func__);
		master->init(master);
		up(&master->sem);
		return -EINVAL;
	}
	int err = master->xfer_error;
	up(&master->sem);

	return err;
}

int i2c_write(struct i2c_master *master, u8 addr, u8 *data, int len)
{
	struct i2c_transfer transfer;

	transfer.addr = addr << 1;
	transfer.data = data;
	transfer.len = len;

	return i2c_xfer(master, &transfer, 1);
}

int i2c_read(struct i2c_master *master, u8 addr, u8 *data, int len)
{
	struct i2c_transfer transfer;

	transfer.addr = addr << 1 | 1;
	transfer.data = data;
	transfer.len = len;

	return i2c_xfer(master, &transfer, 1);
}

int i2c_write_read(struct i2c_master *master, u8 addr, u8 *data, int len, u8 *rxdata, int rxlen)
{
	struct i2c_transfer transfer[2];

	transfer[0].addr = addr << 1;
	transfer[0].data = data;
	transfer[0].len = len;

	transfer[1].addr = addr << 1 | 1;
	transfer[1].data = rxdata;
	transfer[1].len = rxlen;

	return i2c_xfer(master, transfer, 2);
}

void i2c_register_master(struct i2c_master *master)
{
	sem_init(&master->sem, 1); /* one user at a time */
	if (master->speed == 0)
		master->speed = 100000;

	master->init(master);
}

