/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>

#include <sched.h>
#include <lock.h>

#include <arch/crt.h>
#ifdef ARCH_ARM_CORTEX_M3
#include <arch/cm3_regs.h>
#else
#include <arch/cm0_regs.h>
#endif


//#define dprintf printf
#define dprintf(...)

/* quick summary of arm-v7m systick:
 * SYSTICK->CONTROL
 *   [16] RO  COUNTFLAG  1 if counted to 0; reg read clears this flag
 *   [2]  RW  CLKSOURCE  1 = core clock
 *   [1]  RW  TICKINT    1 = generate interrupt iff counted to 0
 *   [0]  RW  ENABLE     1 = enabled
 *
 * SYSTICK->RELOAD
 *   [31:24] reads as 0; write only 0
 *   [23:0]  RW  value loaded into ->VALUE, 1 clock edge after counter reaches 0
 *               (value of 0 disables timer on next wrap)
 *
 * SYSTICK->VALUE
 *   [31:0] RW  read - current counter value
 *              write - any write clears to 0, and also clears COUNTFLAG
 *                it then reloads ->RELOAD on next clock
 *
 * Setup sequence after boot:
 * set RELOAD, set VALUE (clear), enable counter
 */

#define COUNTFLAG (1<<16)

/*
 * current_counter and SYSTICK access is protected by this lock to
 * guarantee atomic access.
 */
struct lock sched_timer_lock = LOCK_INIT;
static u32 current_counter;

/* this always needs to be called under sched_timer_lock */
static inline u32 ticks_since_last_interrupt(int update_current_counter)
{
	u32 ret = 0;
	u32 reload = SYSTICK->RELOAD;

	/* this awkward sequence because COUNTFLAG might be set just after
	 * VALUE is read.
	 */
	/* TODO? is execution order guaranteed? regs are volatile */
	u32 ctrl1 = SYSTICK->CTRL;
	u32 value = SYSTICK->VALUE;
	u32 ctrl2 = SYSTICK->CTRL;

	/* one full cycle (either COUNTFLAG became 1, was already 1) */
	if ((ctrl1 & COUNTFLAG) != (ctrl2 & COUNTFLAG)) {
		/* 0, unless COUNTFLAG was set in ctrl1 already
		 * (which means quite a bit of delay for irq */
		value = SYSTICK->VALUE;

		ret += reload + 1;
		if (update_current_counter) {
			current_counter += reload + 1;
		}
	}

	/* before first clock edge, value remains 0 */
	if (value != 0) {
		ret += reload + 1 - value;
	}
	dprintf("%s(%d): %d\n", __func__, update_current_counter, ret/72);

	return ret;
}

/* returns current scheduler timer value in ticks */
u32 arch_ticks_now()
{
	/* every call to this function updates current_counter.
	 * we have to do this because read from SYSTICK->CONTROL clears
	 * COUNTFLAG (second func invoke would return different value).
	 */

	lock(&sched_timer_lock);
	
	u32 ret = current_counter;
	ret += ticks_since_last_interrupt(1);

	unlock(&sched_timer_lock);
	return ret;
}

static int systick_whole;
static int systick_remainder;

void arch_sched_start_timer()
{
	/* core clock, enable interrupt */
	SYSTICK->RELOAD = 0;
	SYSTICK->VALUE = 0;
#if defined BOARD_QEMU_NETDUINO2 || defined BOARD_QEMU_NETDUINO2_CM0
	/* div by 0 if using core clock on some QEMU platforms */
	SYSTICK->CTRL = 1<<1;
#else
	SYSTICK->CTRL = 1<<2 | 1<<1;
#endif

	/* set SysTick pending to get scheduled asap */
	ICSR = ICSR_PENDSTSET;
}

void arch_sched_now()
{
	lock(&sched_timer_lock);
	SYSTICK->RELOAD = 0;
	SYSTICK->VALUE = 0;
	ICSR = ICSR_PENDSTSET;
	systick_whole = 0; /* TODO can this actually happen? */
	unlock(&sched_timer_lock);
}

void arch_sched_next_interrupt(int offset)
{
	dprintf("%s(%d/%d)\n", __func__, offset/72, offset);
	/* initial 0->RELOAD transition is one tick also */
	if (offset > 1) {
		offset--;
	}

	systick_whole = offset >> 24;
	systick_remainder = offset & 0xffffff;

	lock(&sched_timer_lock);
	/* need to update current_counter, before setting new */
	current_counter += ticks_since_last_interrupt(0);

	if (systick_whole) {
		SYSTICK->RELOAD = 0xffffff;
	} else {
		SYSTICK->RELOAD = systick_remainder;
	}
	SYSTICK->VALUE = 0; /* clear, restart from RELOAD */
	SYSTICK->CTRL |= 1<<0; /* enable systick timer */

	/* in case SysTick became pending very recently, ignore, we've
	 * updated current_counter already */
	ICSR = ICSR_PENDSTCLR;

	unlock(&sched_timer_lock);
}

void __interrupt systick_handler()
{
	lock(&sched_timer_lock);

	/* only update if not already updated before */
	if (SYSTICK->CTRL & COUNTFLAG) {
		current_counter += SYSTICK->RELOAD + 1;
		dprintf("%s: adding %d ticks\n", __func__, (SYSTICK->RELOAD+1)/72);
	}

	if (systick_whole) {
		/* last part? then load the remainder */
		if (--systick_whole == 0) {
			if (systick_remainder == 0) {
				goto time_reached;
			}
			SYSTICK->RELOAD = systick_remainder;
			SYSTICK->VALUE = 0;
		}
		unlock(&sched_timer_lock);
		return;
	}
 time_reached:
	/* disable timer */
	SYSTICK->RELOAD = 0;
	SYSTICK->VALUE = 0;
	unlock(&sched_timer_lock);

	// TODO this should just pass current_counter
	sched_interrupt();
}
