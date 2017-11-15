#ifndef SEMIHOSTING_H_
#define SEMIHOSTING_H_

/* docs: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0471m/pge1358787048379.html */
// r0 is op type; r1 is command block

#define SEMIHOSTING_OPEN   1
#define SEMIHOSTING_CLOSE  2
#define SEMIHOSTING_WRITEC 3
#define SEMIHOSTING_WRITE0 4
#define SEMIHOSTING_WRITE  5
#define SEMIHOSTING_CLOCK  0x10

// semihosting(SEMIHOSTING_WRITE0, "hello world from cbaos\n");

static int semihosting(int cmd, void *arg)
{
	register int _r0 asm("r0") = cmd;
	register void * _r1 asm("r1") = arg;
	asm volatile (
			"svc 0xab\n\t"
			: "+r"(_r0) : "r"(_r1) : "memory");
	return _r0;
}

#endif
