#include <helpers.h>
#include <stdio.h>

void print_array(const char *tag, const u8 *buf, size_t size)
{
	int i;
	printf("%s:", tag);
	for (i=0; i<size; i++)
		printf(" %02x", buf[i]);
	printf("\n");
}
