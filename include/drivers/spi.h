#ifndef _SPI_H_
#define _SPI_H_

#include <types.h>
#include <sem.h>


#define SPI_CPHA        (1<<0)
#define SPI_CPOL        (1<<1)
#define SPI_MODE_0      0
#define SPI_MODE_1      SPI_CPHA /* data sampled on 2nd clock transition */
#define SPI_MODE_2      SPI_CPOL /* clock inactive high */
#define SPI_MODE_3      (SPI_CPOL | SPI_CPHA)

#define SPI_LSB         (1<<2)



struct spi_device {
	int mode;
	int cs_pin;
	int clock;
	struct spi_master *master;
};

struct spi_transfer {
	const u8 *tx_buf;
	u8 *rx_buf;
	int len;
	int error; /* 0 for success, error code otherwise */
	struct sem sem; /* internal, so task can wait for transfer to finish */
};

struct spi_master {
	void *spi;
	void *spi_data;

	int (*transfer)(struct spi_device *device, struct spi_transfer *transfer);
	int (*change_device)(struct spi_device *device);

	/* internal members */
	struct spi_device *last_dev;
	struct sem sem; /* protects against concurrent usage */
};


int spi_transfer_async(struct spi_device *device, struct spi_transfer *msg);
int spi_transfer(struct spi_device *device, struct spi_transfer *msg);
void spi_transfer_finished(struct spi_device *device, struct spi_transfer *msg);
void spi_register_master(struct spi_master *master);
int spi_register_device(struct spi_master *master, struct spi_device *device);


#endif
