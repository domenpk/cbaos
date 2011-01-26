/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <string.h>

#include <cbashell.h>


int main()
{
	void is_it_working(void);
	is_it_working();
	void benchmark(void);
	//benchmark();
	void blinky(void);
	//blinky();
	void semtest(void);
	//semtest();
	void adctest(void);
	//adctest(); /* needs usb_test too */
	int usb_test(void);
	//usb_test();

	return 0;
}
