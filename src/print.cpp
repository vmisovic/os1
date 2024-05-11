#include "../h/print.hpp"

namespace kernel {

// not tested fully
void printInt(long int n)
{
	if (n < 0) {
		__putc('-');
		n = -n;
	}

	long int m;
	for (m = 10; m < n; m *= 10);
	while (m > 1) {
		m /= 10;
		__putc('0' + n / m);
		n = n % m;
	}
}

void printHex(uint64 n)
{
	__putc('0');
	__putc('x');
	for (short i = 15; i >= 0; --i) {
		uint64 val = (n >> 4 * i) & 0xf;
		__putc((val < 10)? '0' + val: 'a' + val - 10);
	}
}

void printString(const char *str)
{
	while(*str != '\0') {
		__putc(*str);
		str++;
	}
}

}
