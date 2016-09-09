/* Author: Domen Puncer Kugler <domen@cba.si>.  License: WTFPL, see file LICENSE */

/* see A guide to debouncing: http://www.eng.utah.edu/~cs5780/debouncing.pdf */


/* Feature requests:
 * - how long is the switch held
 */

#include <stdio.h>

#include <drivers/debounce.h>
#include <gpio.h>


/* debounce_periodic should be called every 5ms or so.
 * With 8 pulses this makes it 40ms delay, which should not be noticeable */

/* It is assumed debounce_register is never called after debounce_periodic */

#define DEBOUNCE_PULSES 8

static LIST_DECLARE_INIT(debounce_list);
//static LOCK_DECLARE_INIT(debounce_list_lock);

int debounce_register(struct debounce_info *deb)
{
//	lock(&debounce_list_lock);
	list_add_tail(&debounce_list, &deb->list);
	deb->history = 0xffffff * gpio_get(deb->pin);
//	unlock(&debounce_list_lock);

	return 0;
}

#define DEBOUNCE_BITS_MASK ((1<<(DEBOUNCE_PULSES+1))-1) /* 0x1ff */
#define DEBOUNCE_BITS_FALLING (1<<DEBOUNCE_PULSES)      /* 0x100 */
#define DEBOUNCE_BITS_RISING  ((1<<DEBOUNCE_PULSES)-1)  /* 0x0ff */
int debounce_periodic()
{
//	lock(&debounce_list_lock);
	struct list *it;
	list_for_each(&debounce_list, it) {
		struct debounce_info *deb = list_entry(it, struct debounce_info, list);
		u32 h = (deb->history<<1) | gpio_get(deb->pin);
		h &= DEBOUNCE_BITS_MASK;
		deb->history = h;

		if ((deb->settings & DEBOUNCE_FALLING) && h == DEBOUNCE_BITS_FALLING)
			deb->callback(deb->pin, 0);
		if ((deb->settings & DEBOUNCE_RISING) && h == DEBOUNCE_BITS_RISING)
			deb->callback(deb->pin, 1);
	}
//	unlock(&debounce_list_lock);
	return 0;
}
