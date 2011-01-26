#ifndef _I2C_H_
#define _I2C_H_

#include <types.h>
#include <sem.h>


struct i2c_transfer {
	u8 addr; /* this one already includes r/w bit */
	u8 *data;
	int len;
};

struct i2c_master {
	void *priv;
	int speed; /* in Hz */
	struct sem sem; /* protects against concurrent usage */

	void (*init)(struct i2c_master *);
	void (*start)(struct i2c_master *);
	void (*restart)(struct i2c_master *);
	void (*stop)(struct i2c_master *);
	void (*ack)(struct i2c_master *, int ack);
	u8 (*read)(struct i2c_master *);
	void (*write)(struct i2c_master *, u8 data);

	struct i2c_transfer *current_transfer;
	int transfers_to_go;
	int pos;
	int xfer_error;
	struct sem xfer_sem; /* up when transfers are finished */
};


/* called by driver state handler */
void i2c_state_machine(struct i2c_master *master, int state);

/* called by application */
int i2c_xfer(struct i2c_master *master, struct i2c_transfer *transfer, int num_xfers);
int i2c_write(struct i2c_master *master, u8 addr, u8 *data, int len);
int i2c_read(struct i2c_master *master, u8 addr, u8 *data, int len);
int i2c_write_read(struct i2c_master *master, u8 addr, u8 *data, int len, u8 *rxdata, int rxlen);

/* called on master init */
void i2c_register_master(struct i2c_master *master);


#endif
