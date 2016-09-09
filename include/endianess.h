#ifndef ENDIANESS_H_
#define ENDIANESS_H_

#include <types.h>

static inline u16 get_be16(const u8 *buf)
{
	return buf[0]<<8 | buf[1];
}

static inline u32 get_be32(const u8 *buf)
{
	return buf[0]<<24 | buf[1]<<16 | buf[2]<<8 | buf[3];
}

static inline u8 *put_be16(u8 *buf, u16 value)
{
	buf[0] = value>>8;
	buf[1] = value;
	return buf+2;
}

static inline u8 *put_be32(u8 *buf, u32 value)
{
	buf[0] = value>>24;
	buf[1] = value>>16;
	buf[2] = value>>8;
	buf[3] = value;
	return buf+4;
}


static inline u16 get_le16(const u8 *buf)
{
	return buf[0] | buf[1]<<8;
}

static inline u32 get_le32(const u8 *buf)
{
	return buf[0] | buf[1]<<8 | buf[2]<<16 | buf[3]<<24;
}

static inline u8 *put_le16(u8 *buf, u16 value)
{
	buf[0] = value;
	buf[1] = value>>8;
	return buf+2;
}

static inline u8 *put_le32(u8 *buf, u32 value)
{
	buf[0] = value;
	buf[1] = value>>8;
	buf[2] = value>>16;
	buf[3] = value>>24;
	return buf+4;
}

#endif
