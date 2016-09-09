#include <timekeeping.h>
#include <arch/sched.h>
#include <types.h>


static u32 jiffies_up;
static u32 jiffies_lo;

void timekeeping(u32 now)
{
	if (now < jiffies_lo)
		jiffies_up++;
	jiffies_lo = now;
}

u32 time_ms(void)
{
	return jiffies_up * MSEC_IN_TICKSOVF + ticks2ms(jiffies_lo);
}
