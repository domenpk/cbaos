/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <drivers/spi.h>
#include <errno.h>
#include <stdio.h>
#include <sem.h>


int spi_transfer_async(struct spi_device *device, struct spi_transfer *msg)
{
	struct spi_master *master = device->master;

	if (down(&master->sem, ms2ticks(1000)) < 0) {
		printf("%s, timeouted\n", __func__);
		return -EAGAIN;
	}

	/* check if another device, then you must call change_device */
	if (master->last_dev != device) {
		master->change_device(device);
		master->last_dev = device;
	}

	sem_init(&msg->sem, 0);
	msg->error = 0;

	return master->transfer(device, msg);
}

int spi_transfer(struct spi_device *device, struct spi_transfer *msg)
{
	if (spi_transfer_async(device, msg) < 0)
		return -EFAULT;

	if (down(&msg->sem, ms2ticks(1000)) < 0) {
		printf("%s, timeouted\n", __func__);
		return -EAGAIN;
	}

	return msg->error;
}

/* this must be called when the spi transfer is finished... from spi master irq, ie. */
void spi_transfer_finished(struct spi_device *device, struct spi_transfer *msg)
{
	up(&msg->sem);
	up(&device->master->sem);
}

void spi_register_master(struct spi_master *master)
{
	sem_init(&master->sem, 1); /* one user at a time */
}

int spi_register_device(struct spi_master *master, struct spi_device *device)
{
	/* it doesn't make much sense to complicate, i think */
	device->master = master;
	return 0;
}
