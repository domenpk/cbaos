#include <stdio.h>
#include <string.h>
#include <limits.h>

/*
 * CBA printf 20151112
 * Author: Domen Puncer Kugler <domen@cba.si>
 * License: WTFPL, see file LICENSE
 */


#define ALEN(x) (sizeof(x)/sizeof(x[0]))

struct test_result {
	const char *result;
	size_t len;
};

static const struct test_result sresults[] = {
        { "(null)", 6 }, { "(null", 6 },
        { "a long string", 13 }, { "a lon", 13 },
        { "", 0 }, { "", 0 },
        { "test", 4 }, { "test", 4 },
        { "    (null)", 10 }, { "    (", 10 },
        { "a long string", 13 }, { "a lon", 13 },
        { "          ", 10 }, { "     ", 10 },
        { "      test", 10 }, { "     ", 10 },
        { "(null)", 6 }, { "(null", 6 },
        { "a long s", 8 }, { "a lon", 8 },
        { "", 0 }, { "", 0 },
        { "test", 4 }, { "test", 4 },
        { "(null)  ", 8 }, { "(null", 8 },
        { "a long string", 13 }, { "a lon", 13 },
        { "        ", 8 }, { "     ", 8 },
        { "test    ", 8 }, { "test ", 8 },
        { "", 0 }, { "", 0 },
        { "", 0 }, { "", 0 },
        { "", 0 }, { "", 0 },
        { "", 0 }, { "", 0 },
        { "", 0 }, { "", 0 },
        { "", 0 }, { "", 0 },
        { "", 0 }, { "", 0 },
        { "", 0 }, { "", 0 },	
};

static const struct test_result iresults[] = {
        { "0", 1 }, { "0", 1 },
        { "-1", 2 }, { "-1", 2 },
        { "1", 1 }, { "1", 1 },
        { "2147483647", 10 }, { "21474", 10 },
        { "-2147483648", 11 }, { "-2147", 11 },
        { "-1", 2 }, { "-1", 2 },
        { "127", 3 }, { "127", 3 },
        { "-128", 4 }, { "-128", 4 },
        { "255", 3 }, { "255", 3 },
        { "0", 1 }, { "0", 1 },
        { "-1", 2 }, { "-1", 2 },
        { "1", 1 }, { "1", 1 },
        { "2147483647", 10 }, { "21474", 10 },
        { "-2147483648", 11 }, { "-2147", 11 },
        { "-1", 2 }, { "-1", 2 },
        { "127", 3 }, { "127", 3 },
        { "-128", 4 }, { "-128", 4 },
        { "255", 3 }, { "255", 3 },
        { "0", 1 }, { "0", 1 },
        { "4294967295", 10 }, { "42949", 10 },
        { "1", 1 }, { "1", 1 },
        { "2147483647", 10 }, { "21474", 10 },
        { "2147483648", 10 }, { "21474", 10 },
        { "4294967295", 10 }, { "42949", 10 },
        { "127", 3 }, { "127", 3 },
        { "4294967168", 10 }, { "42949", 10 },
        { "255", 3 }, { "255", 3 },
        { "0", 1 }, { "0", 1 },
        { "37777777777", 11 }, { "37777", 11 },
        { "1", 1 }, { "1", 1 },
        { "17777777777", 11 }, { "17777", 11 },
        { "20000000000", 11 }, { "20000", 11 },
        { "37777777777", 11 }, { "37777", 11 },
        { "177", 3 }, { "177", 3 },
        { "37777777600", 11 }, { "37777", 11 },
        { "377", 3 }, { "377", 3 },
        { "0", 1 }, { "0", 1 },
        { "ffffffff", 8 }, { "fffff", 8 },
        { "1", 1 }, { "1", 1 },
        { "7fffffff", 8 }, { "7ffff", 8 },
        { "80000000", 8 }, { "80000", 8 },
        { "ffffffff", 8 }, { "fffff", 8 },
        { "7f", 2 }, { "7f", 2 },
        { "ffffff80", 8 }, { "fffff", 8 },
        { "ff", 2 }, { "ff", 2 },
        { "0", 1 }, { "0", 1 },
        { "FFFFFFFF", 8 }, { "FFFFF", 8 },
        { "1", 1 }, { "1", 1 },
        { "7FFFFFFF", 8 }, { "7FFFF", 8 },
        { "80000000", 8 }, { "80000", 8 },
        { "FFFFFFFF", 8 }, { "FFFFF", 8 },
        { "7F", 2 }, { "7F", 2 },
        { "FFFFFF80", 8 }, { "FFFFF", 8 },
        { "FF", 2 }, { "FF", 2 },
        { "  0", 3 }, { "  0", 3 },
        { " -1", 3 }, { " -1", 3 },
        { "  1", 3 }, { "  1", 3 },
        { "2147483647", 10 }, { "21474", 10 },
        { "-2147483648", 11 }, { "-2147", 11 },
        { " -1", 3 }, { " -1", 3 },
        { "127", 3 }, { "127", 3 },
        { "-128", 4 }, { "-128", 4 },
        { "255", 3 }, { "255", 3 },
        { "    0", 5 }, { "    0", 5 },
        { "   -1", 5 }, { "   -1", 5 },
        { "    1", 5 }, { "    1", 5 },
        { "2147483647", 10 }, { "21474", 10 },
        { "-2147483648", 11 }, { "-2147", 11 },
        { "   -1", 5 }, { "   -1", 5 },
        { "  127", 5 }, { "  127", 5 },
        { " -128", 5 }, { " -128", 5 },
        { "  255", 5 }, { "  255", 5 },
        { "           0", 12 }, { "     ", 12 },
        { "  4294967295", 12 }, { "  429", 12 },
        { "           1", 12 }, { "     ", 12 },
        { "  2147483647", 12 }, { "  214", 12 },
        { "  2147483648", 12 }, { "  214", 12 },
        { "  4294967295", 12 }, { "  429", 12 },
        { "         127", 12 }, { "     ", 12 },
        { "  4294967168", 12 }, { "  429", 12 },
        { "         255", 12 }, { "     ", 12 },
        { "0", 1 }, { "0", 1 },
        { "37777777777", 11 }, { "37777", 11 },
        { "1", 1 }, { "1", 1 },
        { "17777777777", 11 }, { "17777", 11 },
        { "20000000000", 11 }, { "20000", 11 },
        { "37777777777", 11 }, { "37777", 11 },
        { "177", 3 }, { "177", 3 },
        { "37777777600", 11 }, { "37777", 11 },
        { "377", 3 }, { "377", 3 },
        { "   0", 4 }, { "   0", 4 },
        { "ffffffff", 8 }, { "fffff", 8 },
        { "   1", 4 }, { "   1", 4 },
        { "7fffffff", 8 }, { "7ffff", 8 },
        { "80000000", 8 }, { "80000", 8 },
        { "ffffffff", 8 }, { "fffff", 8 },
        { "  7f", 4 }, { "  7f", 4 },
        { "ffffff80", 8 }, { "fffff", 8 },
        { "  ff", 4 }, { "  ff", 4 },
        { "    0", 5 }, { "    0", 5 },
        { "FFFFFFFF", 8 }, { "FFFFF", 8 },
        { "    1", 5 }, { "    1", 5 },
        { "7FFFFFFF", 8 }, { "7FFFF", 8 },
        { "80000000", 8 }, { "80000", 8 },
        { "FFFFFFFF", 8 }, { "FFFFF", 8 },
        { "   7F", 5 }, { "   7F", 5 },
        { "FFFFFF80", 8 }, { "FFFFF", 8 },
        { "   FF", 5 }, { "   FF", 5 },
        { "000", 3 }, { "000", 3 },
        { "-01", 3 }, { "-01", 3 },
        { "001", 3 }, { "001", 3 },
        { "2147483647", 10 }, { "21474", 10 },
        { "-2147483648", 11 }, { "-2147", 11 },
        { "-01", 3 }, { "-01", 3 },
        { "127", 3 }, { "127", 3 },
        { "-128", 4 }, { "-128", 4 },
        { "255", 3 }, { "255", 3 },
        { "00000", 5 }, { "00000", 5 },
        { "-0001", 5 }, { "-0001", 5 },
        { "00001", 5 }, { "00001", 5 },
        { "2147483647", 10 }, { "21474", 10 },
        { "-2147483648", 11 }, { "-2147", 11 },
        { "-0001", 5 }, { "-0001", 5 },
        { "00127", 5 }, { "00127", 5 },
        { "-0128", 5 }, { "-0128", 5 },
        { "00255", 5 }, { "00255", 5 },
        { "000000000000", 12 }, { "00000", 12 },
        { "004294967295", 12 }, { "00429", 12 },
        { "000000000001", 12 }, { "00000", 12 },
        { "002147483647", 12 }, { "00214", 12 },
        { "002147483648", 12 }, { "00214", 12 },
        { "004294967295", 12 }, { "00429", 12 },
        { "000000000127", 12 }, { "00000", 12 },
        { "004294967168", 12 }, { "00429", 12 },
        { "000000000255", 12 }, { "00000", 12 },
        { "0", 1 }, { "0", 1 },
        { "37777777777", 11 }, { "37777", 11 },
        { "1", 1 }, { "1", 1 },
        { "17777777777", 11 }, { "17777", 11 },
        { "20000000000", 11 }, { "20000", 11 },
        { "37777777777", 11 }, { "37777", 11 },
        { "177", 3 }, { "177", 3 },
        { "37777777600", 11 }, { "37777", 11 },
        { "377", 3 }, { "377", 3 },
        { "0000", 4 }, { "0000", 4 },
        { "ffffffff", 8 }, { "fffff", 8 },
        { "0001", 4 }, { "0001", 4 },
        { "7fffffff", 8 }, { "7ffff", 8 },
        { "80000000", 8 }, { "80000", 8 },
        { "ffffffff", 8 }, { "fffff", 8 },
        { "007f", 4 }, { "007f", 4 },
        { "ffffff80", 8 }, { "fffff", 8 },
        { "00ff", 4 }, { "00ff", 4 },
        { "00000", 5 }, { "00000", 5 },
        { "FFFFFFFF", 8 }, { "FFFFF", 8 },
        { "00001", 5 }, { "00001", 5 },
        { "7FFFFFFF", 8 }, { "7FFFF", 8 },
        { "80000000", 8 }, { "80000", 8 },
        { "FFFFFFFF", 8 }, { "FFFFF", 8 },
        { "0007F", 5 }, { "0007F", 5 },
        { "FFFFFF80", 8 }, { "FFFFF", 8 },
        { "000FF", 5 }, { "000FF", 5 },
        { "00000", 5 }, { "00000", 5 },
        { "037777777777", 12 }, { "03777", 12 },
        { "00001", 5 }, { "00001", 5 },
        { "017777777777", 12 }, { "01777", 12 },
        { "020000000000", 12 }, { "02000", 12 },
        { "037777777777", 12 }, { "03777", 12 },
        { "00177", 5 }, { "00177", 5 },
        { "037777777600", 12 }, { "03777", 12 },
        { "00377", 5 }, { "00377", 5 },
        { "00000000", 8 }, { "00000", 8 },
        { "0xffffffff", 10 }, { "0xfff", 10 },
        { "0x000001", 8 }, { "0x000", 8 },
        { "0x7fffffff", 10 }, { "0x7ff", 10 },
        { "0x80000000", 10 }, { "0x800", 10 },
        { "0xffffffff", 10 }, { "0xfff", 10 },
        { "0x00007f", 8 }, { "0x000", 8 },
        { "0xffffff80", 10 }, { "0xfff", 10 },
        { "0x0000ff", 8 }, { "0x000", 8 },
        { "000000000", 9 }, { "00000", 9 },
        { "0XFFFFFFFF", 10 }, { "0XFFF", 10 },
        { "0X0000001", 9 }, { "0X000", 9 },
        { "0X7FFFFFFF", 10 }, { "0X7FF", 10 },
        { "0X80000000", 10 }, { "0X800", 10 },
        { "0XFFFFFFFF", 10 }, { "0XFFF", 10 },
        { "0X000007F", 9 }, { "0X000", 9 },
        { "0XFFFFFF80", 10 }, { "0XFFF", 10 },
        { "0X00000FF", 9 }, { "0X000", 9 },
        { "    0", 5 }, { "    0", 5 },
        { "037777777777", 12 }, { "03777", 12 },
        { "   01", 5 }, { "   01", 5 },
        { "017777777777", 12 }, { "01777", 12 },
        { "020000000000", 12 }, { "02000", 12 },
        { "037777777777", 12 }, { "03777", 12 },
        { " 0177", 5 }, { " 0177", 5 },
        { "037777777600", 12 }, { "03777", 12 },
        { " 0377", 5 }, { " 0377", 5 },
        { "       0", 8 }, { "     ", 8 },
        { "0xffffffff", 10 }, { "0xfff", 10 },
        { "     0x1", 8 }, { "     ", 8 },
        { "0x7fffffff", 10 }, { "0x7ff", 10 },
        { "0x80000000", 10 }, { "0x800", 10 },
        { "0xffffffff", 10 }, { "0xfff", 10 },
        { "    0x7f", 8 }, { "    0", 8 },
        { "0xffffff80", 10 }, { "0xfff", 10 },
        { "    0xff", 8 }, { "    0", 8 },
        { "0", 1 }, { "0", 1 },
        { "0XFFFFFFFF", 10 }, { "0XFFF", 10 },
        { "0X1", 3 }, { "0X1", 3 },
        { "0X7FFFFFFF", 10 }, { "0X7FF", 10 },
        { "0X80000000", 10 }, { "0X800", 10 },
        { "0XFFFFFFFF", 10 }, { "0XFFF", 10 },
        { "0X7F", 4 }, { "0X7F", 4 },
        { "0XFFFFFF80", 10 }, { "0XFFF", 10 },
        { "0XFF", 4 }, { "0XFF", 4 },
        { "0    ", 5 }, { "0    ", 5 },
        { "-1   ", 5 }, { "-1   ", 5 },
        { "1    ", 5 }, { "1    ", 5 },
        { "2147483647", 10 }, { "21474", 10 },
        { "-2147483648", 11 }, { "-2147", 11 },
        { "-1   ", 5 }, { "-1   ", 5 },
        { "127  ", 5 }, { "127  ", 5 },
        { "-128 ", 5 }, { "-128 ", 5 },
        { "255  ", 5 }, { "255  ", 5 },
        { "0       ", 8 }, { "0    ", 8 },
        { "-1      ", 8 }, { "-1   ", 8 },
        { "1       ", 8 }, { "1    ", 8 },
        { "2147483647", 10 }, { "21474", 10 },
        { "-2147483648", 11 }, { "-2147", 11 },
        { "-1      ", 8 }, { "-1   ", 8 },
        { "127     ", 8 }, { "127  ", 8 },
        { "-128    ", 8 }, { "-128 ", 8 },
        { "255     ", 8 }, { "255  ", 8 },
        { "0       ", 8 }, { "0    ", 8 },
        { "0xffffffff", 10 }, { "0xfff", 10 },
        { "0x1     ", 8 }, { "0x1  ", 8 },
        { "0x7fffffff", 10 }, { "0x7ff", 10 },
        { "0x80000000", 10 }, { "0x800", 10 },
        { "0xffffffff", 10 }, { "0xfff", 10 },
        { "0x7f    ", 8 }, { "0x7f ", 8 },
        { "0xffffff80", 10 }, { "0xfff", 10 },
        { "0xff    ", 8 }, { "0xff ", 8 },
        { "0       ", 8 }, { "0    ", 8 },
        { "037777777777", 12 }, { "03777", 12 },
        { "01      ", 8 }, { "01   ", 8 },
        { "017777777777", 12 }, { "01777", 12 },
        { "020000000000", 12 }, { "02000", 12 },
        { "037777777777", 12 }, { "03777", 12 },
        { "0177    ", 8 }, { "0177 ", 8 },
        { "037777777600", 12 }, { "03777", 12 },
        { "0377    ", 8 }, { "0377 ", 8 },
        { "0", 1 }, { "0", 1 },
        { "FFFFFFFF", 8 }, { "FFFFF", 8 },
        { "1", 1 }, { "1", 1 },
        { "7FFFFFFF", 8 }, { "7FFFF", 8 },
        { "80000000", 8 }, { "80000", 8 },
        { "FFFFFFFF", 8 }, { "FFFFF", 8 },
        { "7F", 2 }, { "7F", 2 },
        { "FFFFFF80", 8 }, { "FFFFF", 8 },
        { "FF", 2 }, { "FF", 2 },
        { "   +0", 5 }, { "   +0", 5 },
        { "   -1", 5 }, { "   -1", 5 },
        { "   +1", 5 }, { "   +1", 5 },
        { "+2147483647", 11 }, { "+2147", 11 },
        { "-2147483648", 11 }, { "-2147", 11 },
        { "   -1", 5 }, { "   -1", 5 },
        { " +127", 5 }, { " +127", 5 },
        { " -128", 5 }, { " -128", 5 },
        { " +255", 5 }, { " +255", 5 },
        { "    0", 5 }, { "    0", 5 },
        { "   -1", 5 }, { "   -1", 5 },
        { "    1", 5 }, { "    1", 5 },
        { " 2147483647", 11 }, { " 2147", 11 },
        { "-2147483648", 11 }, { "-2147", 11 },
        { "   -1", 5 }, { "   -1", 5 },
        { "  127", 5 }, { "  127", 5 },
        { " -128", 5 }, { " -128", 5 },
        { "  255", 5 }, { "  255", 5 },
        { " 0", 2 }, { " 0", 2 },
        { "-1", 2 }, { "-1", 2 },
        { " 1", 2 }, { " 1", 2 },
        { " 2147483647", 11 }, { " 2147", 11 },
        { "-2147483648", 11 }, { "-2147", 11 },
        { "-1", 2 }, { "-1", 2 },
        { " 127", 4 }, { " 127", 4 },
        { "-128", 4 }, { "-128", 4 },
        { " 255", 4 }, { " 255", 4 },
        { "0", 1 }, { "0", 1 },
        { "-1", 2 }, { "-1", 2 },
        { "1", 1 }, { "1", 1 },
        { "-1", 2 }, { "-1", 2 },
        { "0", 1 }, { "0", 1 },
        { "-1", 2 }, { "-1", 2 },
        { "127", 3 }, { "127", 3 },
        { "-128", 4 }, { "-128", 4 },
        { "255", 3 }, { "255", 3 },
        { "0", 1 }, { "0", 1 },
        { "-1", 2 }, { "-1", 2 },
        { "1", 1 }, { "1", 1 },
        { "-1", 2 }, { "-1", 2 },
        { "0", 1 }, { "0", 1 },
        { "-1", 2 }, { "-1", 2 },
        { "127", 3 }, { "127", 3 },
        { "-128", 4 }, { "-128", 4 },
        { "-1", 2 }, { "-1", 2 },
        { "0", 1 }, { "0", 1 },
        { "65535", 5 }, { "65535", 5 },
        { "1", 1 }, { "1", 1 },
        { "65535", 5 }, { "65535", 5 },
        { "0", 1 }, { "0", 1 },
        { "65535", 5 }, { "65535", 5 },
        { "127", 3 }, { "127", 3 },
        { "65408", 5 }, { "65408", 5 },
        { "255", 3 }, { "255", 3 },
        { "0", 1 }, { "0", 1 },
        { "ff", 2 }, { "ff", 2 },
        { "1", 1 }, { "1", 1 },
        { "ff", 2 }, { "ff", 2 },
        { "0", 1 }, { "0", 1 },
        { "ff", 2 }, { "ff", 2 },
        { "7f", 2 }, { "7f", 2 },
        { "80", 2 }, { "80", 2 },
        { "ff", 2 }, { "ff", 2 },
        { "0", 1 }, { "0", 1 },
        { "177777", 6 }, { "17777", 6 },
        { "1", 1 }, { "1", 1 },
        { "177777", 6 }, { "17777", 6 },
        { "0", 1 }, { "0", 1 },
        { "177777", 6 }, { "17777", 6 },
        { "177", 3 }, { "177", 3 },
        { "177600", 6 }, { "17760", 6 },
        { "377", 3 }, { "377", 3 },	
};

static const struct test_result cresults[] = {
        { "  a", 3 }, { "  a", 3 },
        { "   ", 3 }, { "   ", 3 },
        { "  +", 3 }, { "  +", 3 },
        { "a", 1 }, { "a", 1 },
        { " ", 1 }, { " ", 1 },
        { "+", 1 }, { "+", 1 },
        { "a  ", 3 }, { "a  ", 3 },
        { "   ", 3 }, { "   ", 3 },
        { "+  ", 3 }, { "+  ", 3 },
        { "a", 1 }, { "a", 1 },
        { " ", 1 }, { " ", 1 },
        { "+", 1 }, { "+", 1 },	
};

static const long itests[] = { 0, -1, 1, INT_MAX, INT_MIN, UINT_MAX, SCHAR_MAX, SCHAR_MIN, UCHAR_MAX, };
static const char *iformats[] = {
		"%i", "%d", "%u", "%o", "%x", "%X",
		"%3i", "%5d", "%12u", "%1o", "%4x", "%5X",
		"%03i", "%05d", "%012u", "%01o", "%04x", "%05X",
		"%#05o", "%#08x", "%#09X", "%#5o", "%#8x", "%#X",
		"%-5i", "%-8d", "%#-8x", "%-#8o", "%-1X",
		"%+5i", "% 5i", "% i",
		"%hi", "%hhi", "%hu", "%hhx", "%ho",
};

static const long stests[] = { (long)NULL, (long)"a long string", (long)"", (long)"test" };
//char *stests[] = { NULL, "a long string", "", "test" };
static const char *sformats[] = { "%s", "%10s", "%.8s", "%-8s", "%.s", "%.0s", };
static const long ctests[] = { (char)'a', (char)' ', (char)'+', };
static const char *cformats[] = { "%3c", "%.3c", "%-3c", "%c", };

struct test {
	const char **formats;
	size_t nformats;
	const long *tests;
	size_t ntests;
	const struct test_result *results;
};

struct test tests[] = {
	{
		.formats = sformats, .nformats = ALEN(sformats),
		.tests   = stests,   .ntests   = ALEN(stests),
		.results = sresults
	},
	{
		.formats = iformats, .nformats = ALEN(iformats),
		.tests   = itests,   .ntests   = ALEN(itests),
		.results = iresults
	},
	{
		.formats = cformats, .nformats = ALEN(cformats),
		.tests   = ctests,   .ntests   = ALEN(ctests),
		.results = cresults
	},
};

static int printf_tests_quiet = 0;

void printf_run_tests(struct test *tests, int n)
{
	int t, i, j;

	for (t=0; t<n; t++) {
		if (!printf_tests_quiet)
			printf("%s %d\n", __func__, t);
		const char **formats = tests[t].formats;
		size_t nformats = tests[t].nformats;
		const long *test = tests[t].tests;
		size_t ntest = tests[t].ntests;
		const struct test_result *results = tests[t].results;

		for (j=0; j<nformats; j++) {
			for (i=0; i<ntest; i++) {
#if 0
				/* print reference output */
				char test1[64];
				int l1;
				l1 = sprintf(test1, formats[j], test[i]);
				printf("\t{ \"%s\", %d }, ", test1, l1);
				l1 = snprintf(test1, 6, formats[j], test[i]);
				printf("{ \"%s\", %d },\n", test1, l1);
#else
				/* actually test */
				char test1[64];
				int l1;

				const char *res = results[(j*ntest + i)*2].result;
				int reslen = results[(j*ntest + i)*2].len;
				l1 = sprintf(test1, formats[j], test[i]);
				if (l1 != reslen || strcmp(test1, res) != 0) {
					printf("sprintf fail: %d:%s:%d, \"%s\" != \"%s\"\n",
							t, formats[j], i, test1, res);
				}

				res = results[(j*ntest + i)*2+1].result;
				reslen = results[(j*ntest + i)*2+1].len;
				l1 = snprintf(test1, 6, formats[j], test[i]);
				if (l1 != reslen || strcmp(test1, res) != 0) {
					printf("snprintf fail: %d:%s:%d, \"%s\" != \"%s\"\n",
							t, formats[j], i, test1, res);
				}
#endif
			}
		}
	}
}


//#define printf_tests main
int printf_tests(void)
{
	/*
	run_tests_l();
	run_tests_ll();
	*/
	printf_run_tests(tests, ALEN(tests));
	return 0;
}

#if 0
void run_tests_l()
{
	int i, j;
	char *formats[] = { "%li", "%lu" };
	long tests[] = { LONG_MAX, 0L, LONG_MIN, -1L, };
	int nformats = ALEN(formats);
	int ntests = ALEN(tests);
	if (!printf_tests_quiet)
		printf("%s\n", __func__);

	for (j=0; j<nformats; j++) {
		printf(" test %s: .", formats[j]);
		for (i=0; i<ntests; i++) 
			printf(formats[j], tests[i]), printf(". .");
		printf("\n");

		_printf("_test %s: .", formats[j]);
		for (i=0; i<ntests; i++)
			_printf(formats[j], tests[i]), printf(". .");
		printf("\n");
	}
}
#ifdef ARCH_UNIX
void run_tests_ll()
{
	int i, j;
	char *formats[] = { "%lli", "%llu" };
	long long tests[] = { LLONG_MAX, 0LL, LLONG_MIN, -1LL, };
	int nformats = ALEN(formats);
	int ntests = ALEN(tests);
	if (!printf_tests_quiet)
		printf("%s\n", __func__);

	for (j=0; j<nformats; j++) {
		printf(" test %s: .", formats[j]);
		for (i=0; i<ntests; i++) 
			printf(formats[j], tests[i]), printf(". .");
		printf("\n");

		_printf("_test %s: .", formats[j]);
		for (i=0; i<ntests; i++)
			_printf(formats[j], tests[i]), printf(". .");
		printf("\n");
	}
}
#endif

#endif
