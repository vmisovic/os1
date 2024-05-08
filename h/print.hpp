#ifndef PRINT_HPP
#define PRINT_HPP

#include "../lib/console.h"
#include "../lib/hw.h"

#ifdef __cplusplus
extern "C" {
#endif

void printInt(long int n);
void printHex(uint64 n);
void printString(const char *str);

#ifdef __cplusplus
}
#endif

#endif
