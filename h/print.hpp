#ifndef PRINT_HPP
#define PRINT_HPP

#include "../h/console.hpp"
#include "../lib/hw.h"

namespace kernel {

#define PRINT_ERROR true
#define PRINT_INFO true
#define PRINT_ECALL true
#define PRINT_TIMER_SWITCH false
#define PRINT_DISPATCH false
#define PRINT_MEMORY false
#define PRINT_THREAD false
#define PRINT_SLEEPY false

void printInt(long int n, bool enabled = true);
void printHex(uint64 n, bool enabled = true);
void printString(const char *str, bool enabled = true);

}

#endif
