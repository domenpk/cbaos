#ifndef ENC28J60_H_
#define ENC28J60_H_

#include <drivers/spi.h>

int enc28j60_setup(struct spi_master *spi, int gpio_cs, int gpio_int);
int enc28j60_raw_send(u8 *buf, unsigned len);
int enc28j60_process_packet(void);

#endif
